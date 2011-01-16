/*
	Audio File Library
	Copyright (C) 2001, Silicon Graphics, Inc.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA  02111-1307  USA.
*/

/*
	IRCAM.cpp

	This file contains routines for parsing Berkeley/IRCAM/CARL
	format files.
*/

#include "config.h"
#include "IRCAM.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Marker.h"
#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "byteorder.h"
#include "util.h"

/*
	Here ircam_mips_magic refers to little-endian MIPS, not SGI IRIX,
	which uses big-endian MIPS.
*/
const uint8_t _af_ircam_vax_magic[4] = {0x64, 0xa3, 0x01, 0x00},
	_af_ircam_sun_magic[4] = {0x64, 0xa3, 0x02, 0x00},
	_af_ircam_mips_magic[4] = {0x64, 0xa3, 0x03, 0x00},
	_af_ircam_next_magic[4] = {0x64, 0xa3, 0x04, 0x00};

_AFfilesetup _af_ircam_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_IRCAM,		/* fileFormat */
	true,			/* trackSet */
	true,			/* instrumentSet */
	true,			/* miscellaneousSet */
	1,			/* trackCount */
	NULL,			/* tracks */
	0,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

bool IRCAMFile::recognize(File *fh)
{
	uint8_t buffer[4];

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(buffer, 4, fh) != 4)
		return false;

	/* Check to see if the file's magic number matches. */
	if (memcmp(buffer, _af_ircam_vax_magic, 4) == 0 ||
		memcmp(buffer, _af_ircam_sun_magic, 4) == 0 ||
		memcmp(buffer, _af_ircam_mips_magic, 4) == 0 ||
		memcmp(buffer, _af_ircam_next_magic, 4) == 0)
	{
		return true;
	}

	return false;
}

AFfilesetup IRCAMFile::completeSetup(AFfilesetup setup)
{
	TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "BICSF file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	track = &setup->tracks[0];

	if (track->sampleFormatSet)
	{
		if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
		{
			_af_error(AF_BAD_SAMPFMT,
				"BICSF format does not support unsigned data");
			return AF_NULL_FILESETUP;
		}

		if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP &&
			track->f.sampleWidth != 16)
		{
			_af_error(AF_BAD_WIDTH,
				"BICSF format supports only 16-bit width for "
				"two's complement audio data");
			return AF_NULL_FILESETUP;
		}

		if (track->f.sampleFormat == AF_SAMPFMT_DOUBLE)
		{
			_af_error(AF_BAD_SAMPFMT,
				"BICSF format does not support "
				"double-precision floating-point data");
			return AF_NULL_FILESETUP;
		}
	}

	if (track->rateSet && track->f.sampleRate <= 0.0)
	{
		_af_error(AF_BAD_RATE,
			"invalid sample rate %.30g for BICSF file",
			track->f.sampleRate);
		return AF_NULL_FILESETUP;
	}

	if (track->channelCountSet && track->f.channelCount != 1 &&
		track->f.channelCount != 2 && track->f.channelCount != 4)
	{
		_af_error(AF_BAD_CHANNELS,
			"invalid channel count (%d) for BICSF format "
			"(1, 2, or 4 channels only)",
			track->f.channelCount);
		return AF_NULL_FILESETUP;
	}

	if (track->compressionSet &&
		track->f.compressionType != AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"BICSF format does not support compression");
		return AF_NULL_FILESETUP;
	}

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "BICSF file cannot have AES data");
		return AF_NULL_FILESETUP;
	}

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_NUMMARKS, "BICSF format does not support markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_NUMINSTS, "BICSF format does not support instruments");
		return AF_NULL_FILESETUP;
	}

	/* XXXmpruett: We don't support miscellaneous chunks for now. */
	if (setup->miscellaneousSet && setup->miscellaneousCount != 0)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "BICSF format does not currently support miscellaneous chunks");
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &_af_ircam_default_filesetup, true);
}

status IRCAMFile::readInit(AFfilesetup setup)
{
	uint8_t magic[4];

	float maxAmp = 1.0;

	bool isSwapped, isLittleEndian;

	instruments = NULL;
	instrumentCount = 0 ;
	miscellaneous = NULL;
	miscellaneousCount = 0;

	tracks = NULL;
	trackCount = 1;

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(magic, 4, fh) != 4)
	{
		_af_error(AF_BAD_READ, "Could not read BICSF file header");
		return AF_FAIL;
	}

	if (memcmp(magic, _af_ircam_vax_magic, 4) != 0 &&
		memcmp(magic, _af_ircam_sun_magic, 4) != 0 &&
		memcmp(magic, _af_ircam_mips_magic, 4) != 0 &&
		memcmp(magic, _af_ircam_next_magic, 4) != 0)
	{
		_af_error(AF_BAD_FILEFMT,
			"file is not a BICSF file (bad magic number)");
		return AF_FAIL;
	}

	/*
		If the file's magic number is that for VAX or MIPS,
		the file is little endian.
	*/
	isLittleEndian = (memcmp(magic, _af_ircam_vax_magic, 4) == 0 ||
		memcmp(magic, _af_ircam_mips_magic, 4) == 0);

	setFormatByteOrder(isLittleEndian ? AF_BYTEORDER_LITTLEENDIAN :
		AF_BYTEORDER_BIGENDIAN);

	float rate;
	readF32(&rate);
	uint32_t channels;
	readU32(&channels);
	uint32_t packMode;
	readU32(&packMode);

	Track *track = _af_track_new();
	if (!track)
		return AF_FAIL;

	tracks = track;

	track->f.sampleRate = rate;
	track->f.compressionType = AF_COMPRESSION_NONE;

	switch (packMode)
	{
		case SF_SHORT:
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.sampleWidth = 16;
			break;
		case SF_FLOAT:
			track->f.sampleFormat = AF_SAMPFMT_FLOAT;
			track->f.sampleWidth = 32;
			break;
		default:
			_af_error(AF_BAD_NOT_IMPLEMENTED,
				"BICSF data format %d not supported", packMode);
			return AF_FAIL;
	}

	track->f.channelCount = channels;
	if (channels != 1 && channels != 2 && channels != 4)
	{
		_af_error(AF_BAD_FILEFMT, "invalid channel count (%d) "
			"for BICSF format (1, 2, or 4 only)",
			channels);
		return AF_FAIL;
	}

	if (isLittleEndian)
		track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;
	else
		track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	if (_af_set_sample_format(&track->f, track->f.sampleFormat,
		track->f.sampleWidth) == AF_FAIL)
	{
		return AF_FAIL;
	}

	if (track->f.sampleFormat == AF_SAMPFMT_FLOAT)
		track->f.pcm.slope = maxAmp;

	track->data_size = af_flength(fh) - SIZEOF_BSD_HEADER;

	/*
		Only uncompressed data formats are supported for IRCAM
		files right now.  The following line would need to be
		changed if compressed data formats were supported.
	*/
	track->totalfframes = track->data_size /
		(int) _af_format_frame_size(&track->f, false);

	track->fpos_first_frame = SIZEOF_BSD_HEADER;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	return AF_SUCCEED;
}

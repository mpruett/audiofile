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
	ircam.c

	This file contains routines for parsing Berkeley/IRCAM/CARL
	format files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "afinternal.h"
#include "audiofile.h"
#include "util.h"
#include "byteorder.h"
#include "setup.h"
#include "track.h"
#include "marker.h"

#include "ircam.h"

/*
	These magic numbers are fucking stupid.

	Here ircam_mips_magic refers to little-endian MIPS, not SGI IRIX,
	which uses big-endian MIPS.
*/
const u_int8_t ircam_vax_magic[4] = {0x64, 0xa3, 0x01, 0x00},
	ircam_sun_magic[4] = {0x64, 0xa3, 0x02, 0x00},
	ircam_mips_magic[4] = {0x64, 0xa3, 0x03, 0x00},
	ircam_next_magic[4] = {0x64, 0xa3, 0x04, 0x00};

_AFfilesetup _af_ircam_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_IRCAM,		/* fileFormat */
	AF_TRUE,		/* trackSet */
	AF_TRUE,		/* instrumentSet */
	AF_TRUE,		/* miscellaneousSet */
	1,			/* trackCount */
	NULL,			/* tracks */
	0,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

bool _af_ircam_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[4];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 4, 1, fh) != 1)
		return AF_FALSE;

	/* Check to see if the file's magic number matches. */
	if (memcmp(buffer, ircam_vax_magic, 4) == 0 ||
		memcmp(buffer, ircam_sun_magic, 4) == 0 ||
		memcmp(buffer, ircam_mips_magic, 4) == 0 ||
		memcmp(buffer, ircam_next_magic, 4) == 0)
	{
		return AF_TRUE;
	}

	return AF_FALSE;
}

AFfilesetup _af_ircam_complete_setup (AFfilesetup setup)
{
	_TrackSetup	*track;

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

	return _af_filesetup_copy(setup, &_af_ircam_default_filesetup, AF_TRUE);
}

status _af_ircam_read_init (AFfilesetup setup, AFfilehandle handle)
{
	_Track		*track;
	u_int8_t	magic[4];
	float		rate;
	u_int32_t	channels;
	u_int32_t	packMode;

	float		maxAmp = 1.0;

	bool		isSwapped, isLittleEndian;

	handle->instruments = NULL;
	handle->instrumentCount = 0 ;
	handle->miscellaneous = NULL;
	handle->miscellaneousCount = 0;

	handle->tracks = NULL;
	handle->trackCount = 1;

	af_fseek(handle->fh, 0, SEEK_SET);

	if (af_fread(magic, 4, 1, handle->fh) != 1)
	{
		_af_error(AF_BAD_READ, "Could not read BICSF file header");
		return AF_FAIL;
	}

	if (memcmp(magic, ircam_vax_magic, 4) != 0 &&
		memcmp(magic, ircam_sun_magic, 4) != 0 &&
		memcmp(magic, ircam_mips_magic, 4) != 0 &&
		memcmp(magic, ircam_next_magic, 4) != 0)
	{
		_af_error(AF_BAD_FILEFMT,
			"file is not a BICSF file (bad magic number)");
		return AF_FAIL;
	}

	/*
		If the file's magic number is that for VAX or MIPS,
		the file is little endian.
	*/
	isLittleEndian = (memcmp(magic, ircam_vax_magic, 4) == 0 ||
		memcmp(magic, ircam_mips_magic, 4) == 0);

#ifdef WORDS_BIGENDIAN
	isSwapped = isLittleEndian;
#else
	isSwapped = !isLittleEndian;
#endif

	af_fread(&rate, 4, 1, handle->fh);
	af_fread(&channels, 4, 1, handle->fh);
	af_fread(&packMode, 4, 1, handle->fh);

	if (isSwapped)
	{
		rate = _af_byteswap_float32(rate);
		channels = _af_byteswap_int32(channels);
		packMode = _af_byteswap_int32(packMode);
	}

	if ((handle->tracks = _af_track_new()) == NULL)
		return AF_FAIL;

	track = &handle->tracks[0];

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

	track->data_size = af_flength(handle->fh) - SIZEOF_BSD_HEADER;

	/*
		Only uncompressed data formats are supported for IRCAM
		files right now.  The following line would need to be
		changed if compressed data formats were supported.
	*/
	track->totalfframes = track->data_size /
		_af_format_frame_size(&track->f, AF_FALSE);

	track->fpos_first_frame = SIZEOF_BSD_HEADER;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	handle->formatSpecific = NULL;

	return AF_SUCCEED;
}

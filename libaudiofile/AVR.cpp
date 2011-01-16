/*
	Audio File Library
	Copyright (C) 2004, Michael Pruett <michael@68k.org>

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
	AVR.cpp

	This file contains routines for parsing AVR (Audio Visual
	Research) sound files.
*/

#include "config.h"
#include "AVR.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"

_AFfilesetup _af_avr_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_AVR,		/* fileFormat */
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

AVRFile::AVRFile()
{
	setFormatByteOrder(AF_BYTEORDER_BIGENDIAN);
}

bool AVRFile::recognize(File *fh)
{
	uint32_t	magic;

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(&magic, 4, fh) != 4 || memcmp(&magic, "2BIT", 4) != 0)
		return false;

	return true;
}

status AVRFile::readInit(AFfilesetup setup)
{
	uint32_t	magic;
	char		name[8];
	uint16_t	mono, resolution, sign, loop, midi;
	uint32_t	rate, size, loopStart, loopEnd;
	char		reserved[26];
	char		user[64];

	Track		*track;

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(&magic, 4, fh) != 4)
	{
		_af_error(AF_BAD_READ, "could not read AVR file header");
		return AF_FAIL;
	}

	if (memcmp(&magic, "2BIT", 4) != 0)
	{
		_af_error(AF_BAD_FILEFMT, "file is not AVR format");
		return AF_FAIL;
	}

	/* Read name. */
	af_read(name, 8, fh);

	readU16(&mono);
	readU16(&resolution);
	readU16(&sign);
	readU16(&loop);
	readU16(&midi);

	readU32(&rate);
	readU32(&size);
	readU32(&loopStart);
	readU32(&loopEnd);

	af_read(reserved, 26, fh);
	af_read(user, 64, fh);

	if ((track = _af_track_new()) == NULL)
		return AF_FAIL;

	tracks = track;
	trackCount = 1;

	instruments = NULL;
	instrumentCount = 0;

	miscellaneous = NULL;
	miscellaneousCount = 0;

	/* Use only low-order three bytes of sample rate. */
	track->f.sampleRate = rate & 0xffffff;

	if (sign == 0)
		track->f.sampleFormat = AF_SAMPFMT_UNSIGNED;
	else if (sign == 0xffff)
		track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
	else
	{
		_af_error(AF_BAD_SAMPFMT, "bad sample format in AVR file");
		return AF_FAIL;
	}

	if (resolution != 8 && resolution != 16)
	{
		_af_error(AF_BAD_WIDTH, "bad sample width %d in AVR file",
			resolution);
		return AF_FAIL;
	}
	track->f.sampleWidth = resolution;

	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	if (mono == 0)
		track->f.channelCount = 1;
	else if (mono == 0xffff)
		track->f.channelCount = 2;
	else
	{
		_af_error(AF_BAD_CHANNELS,
			"invalid number of channels in AVR file");
		return AF_FAIL;
	}

	track->f.compressionType = AF_COMPRESSION_NONE;

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	track->fpos_first_frame = af_ftell(fh);
	track->totalfframes = size;
	track->data_size = track->totalfframes *
		(int) _af_format_frame_size(&track->f, false);
        track->nextfframe = 0;
        track->fpos_next_frame = track->fpos_first_frame;

	/* The file has been parsed successfully. */
	return AF_SUCCEED;
}

AFfilesetup AVRFile::completeSetup(AFfilesetup setup)
{
	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "AVR files must have exactly 1 track");
		return AF_NULL_FILESETUP;
	}

	TrackSetup *track = setup->getTrack();
	if (!track)
		return AF_NULL_FILESETUP;

	/* AVR allows only unsigned and two's complement integer data. */
	if (track->f.sampleFormat != AF_SAMPFMT_UNSIGNED &&
		track->f.sampleFormat != AF_SAMPFMT_TWOSCOMP)
	{
		_af_error(AF_BAD_FILEFMT, "AVR format does supports only unsigned and two's complement integer data");
		return AF_NULL_FILESETUP;
	}

	/* For now we support only 8- and 16-bit samples. */
	if (track->f.sampleWidth != 8 && track->f.sampleWidth != 16)
	{
		_af_error(AF_BAD_WIDTH, "invalid sample width %d for AVR file (only 8- and 16-bit sample widths are allowed)");
		return AF_NULL_FILESETUP;
	}

	/* AVR does not support compression. */
	if (track->f.compressionType != AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "compression not supported for AVR files");
		return AF_NULL_FILESETUP;
	}

	/* AVR audio data is big-endian. */
	if (track->f.byteOrder != AF_BYTEORDER_BIGENDIAN)
	{
		if (track->byteOrderSet)
		{
			_af_error(AF_BAD_BYTEORDER,
				"AVR format supports only big-endian data");
			return AF_NULL_FILESETUP;
		}
		else
			track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
	}

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "AVR files do not support AES data");
		return AF_NULL_FILESETUP;
	}

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_FILESETUP, "AVR format does not support markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_FILESETUP, "AVR format does not support instruments");
		return AF_NULL_FILESETUP;
	}

	if (setup->miscellaneousSet && setup->miscellaneousCount != 0)
	{
		_af_error(AF_BAD_FILESETUP, "AVR format does not support miscellaneous data");
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &_af_avr_default_filesetup, false);
}

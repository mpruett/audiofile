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
	avr.c

	This file contains routines for parsing AVR (Audio Visual
	Research) sound files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "audiofile.h"
#include "afinternal.h"
#include "track.h"
#include "util.h"
#include "setup.h"
#include "byteorder.h"

#include "avr.h"

_AFfilesetup _af_avr_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_AVR,		/* fileFormat */
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

bool _af_avr_recognize (AFvirtualfile *fh)
{
	u_int32_t	magic;

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(&magic, 4, 1, fh) != 1 || memcmp(&magic, "2BIT", 4) != 0)
		return AF_FALSE;

	return AF_TRUE;
}

status _af_avr_read_init (AFfilesetup setup, AFfilehandle file)
{
	u_int32_t	magic;
	char		name[8];
	u_int16_t	mono, resolution, sign, loop, midi;
	u_int32_t	rate, size, loopStart, loopEnd;
	char		reserved[26];
	char		user[64];

	_Track		*track;

	assert(file != NULL);
	assert(file->fh != NULL);

	af_fseek(file->fh, 0, SEEK_SET);

	if (af_fread(&magic, 4, 1, file->fh) != 1)
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
	af_fread(name, 8, 1, file->fh);

	af_read_uint16_be(&mono, file->fh);
	af_read_uint16_be(&resolution, file->fh);
	af_read_uint16_be(&sign, file->fh);
	af_read_uint16_be(&loop, file->fh);
	af_read_uint16_be(&midi, file->fh);

	af_read_uint32_be(&rate, file->fh);
	af_read_uint32_be(&size, file->fh);
	af_read_uint32_be(&loopStart, file->fh);
	af_read_uint32_be(&loopEnd, file->fh);

	af_fread(reserved, 26, 1, file->fh);
	af_fread(user, 64, 1, file->fh);

	if ((track = _af_track_new()) == NULL)
		return AF_FAIL;

	file->tracks = track;
	file->trackCount = 1;

	file->instruments = NULL;
	file->instrumentCount = 0;

	file->miscellaneous = NULL;
	file->miscellaneousCount = 0;

	file->formatSpecific = NULL;

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

	track->fpos_first_frame = af_ftell(file->fh);
	track->totalfframes = size;
	track->data_size = track->totalfframes *
		_af_format_frame_size(&track->f, AF_FALSE);
        track->nextfframe = 0;
        track->fpos_next_frame = track->fpos_first_frame;

	/* The file has been parsed successfully. */
	return AF_SUCCEED;
}

AFfilesetup _af_avr_complete_setup (AFfilesetup setup)
{
	_TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "AVR files must have exactly 1 track");
		return AF_NULL_FILESETUP;
	}

	track = _af_filesetup_get_tracksetup(setup, AF_DEFAULT_TRACK);

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

	return _af_filesetup_copy(setup, &_af_avr_default_filesetup, AF_FALSE);
}

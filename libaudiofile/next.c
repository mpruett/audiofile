/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>

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
	next.c

	This file contains routines for parsing NeXT/Sun .snd format sound
	files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "next.h"
#include "audiofile.h"
#include "afinternal.h"
#include "track.h"
#include "util.h"
#include "setup.h"
#include "byteorder.h"

int _af_next_compression_types[_AF_NEXT_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

_AFfilesetup _af_next_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_NEXTSND,	/* fileFormat */
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

status _af_next_read_init (AFfilesetup setup, AFfilehandle file)
{
	u_int32_t	id, offset, length, encoding, sampleRate, channelCount;
	_Track		*track;

	assert(file != NULL);
	assert(file->fh != NULL);

	file->formatSpecific = NULL;

	file->instruments = NULL;
	file->instrumentCount = 0;

	file->miscellaneous = NULL;
	file->miscellaneousCount = 0;

	file->tracks = NULL;	/* Allocate this later. */
	file->trackCount = 1;

	af_fseek(file->fh, 0, SEEK_SET);

	af_fread(&id, 4, 1, file->fh);
	assert(!memcmp(&id, ".snd", 4));

	af_fread(&offset, 4, 1, file->fh);
	af_fread(&length, 4, 1, file->fh);
	af_fread(&encoding, 4, 1, file->fh);
	af_fread(&sampleRate, 4, 1, file->fh);
	af_fread(&channelCount, 4, 1, file->fh);

	offset = BENDIAN_TO_HOST_INT32(offset);
	length = BENDIAN_TO_HOST_INT32(length);
	encoding = BENDIAN_TO_HOST_INT32(encoding);
	sampleRate = BENDIAN_TO_HOST_INT32(sampleRate);
	channelCount = BENDIAN_TO_HOST_INT32(channelCount);

#ifdef DEBUG
	printf("id, offset, length, encoding, sampleRate, channelCount:\n"
		" %d %d %d %d %d %d\n",
		id, offset, length, encoding, sampleRate, channelCount);
#endif

	if ((track = _af_track_new()) == NULL)
		return AF_FAIL;

	file->tracks = track;

	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	/* Override the compression type later if necessary. */
	track->f.compressionType = AF_COMPRESSION_NONE;

	track->fpos_first_frame = offset;
	track->data_size = af_flength(file->fh) - offset;

	switch (encoding)
	{
		case _AU_FORMAT_MULAW_8:
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.compressionType = AF_COMPRESSION_G711_ULAW;
			break;
		case _AU_FORMAT_ALAW_8:
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.compressionType = AF_COMPRESSION_G711_ALAW;
			break;
		case _AU_FORMAT_LINEAR_8:
			track->f.sampleWidth = 8;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_FORMAT_LINEAR_16:
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_FORMAT_LINEAR_24:
			track->f.sampleWidth = 24;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_FORMAT_LINEAR_32:
			track->f.sampleWidth = 32;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			break;
		case _AU_FORMAT_FLOAT:
			track->f.sampleWidth = 32;
			track->f.sampleFormat = AF_SAMPFMT_FLOAT;
			break;
		case _AU_FORMAT_DOUBLE:
			track->f.sampleWidth = 64;
			track->f.sampleFormat = AF_SAMPFMT_DOUBLE;
			break;

		default:
			/*
				This encoding method is not recognized.
			*/
			_af_error(AF_BAD_SAMPFMT, "bad sample format");
			return AF_FAIL;
	}

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	track->f.sampleRate = sampleRate;
	track->f.channelCount = channelCount;
	track->totalfframes = length / _af_format_frame_size(&track->f, AF_FALSE);

#ifdef DEBUG
	printf("_af_next_read_init\n");
	_af_print_filehandle(file);
#endif

	/* The file has been parsed successfully. */
	return AF_SUCCEED;
}

bool _af_next_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[4];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 1, 4, fh) != 4 || memcmp(buffer, ".snd", 4) != 0)
		return AF_FALSE;

	return AF_TRUE;
}

AFfilesetup _af_next_complete_setup (AFfilesetup setup)
{
	_TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "NeXT files must have exactly 1 track");
		return AF_NULL_FILESETUP;
	}

	track = _af_filesetup_get_tracksetup(setup, AF_DEFAULT_TRACK);
	if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
	{
		_af_error(AF_BAD_FILEFMT, "NeXT format does not support unsigned data");
		_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, track->f.sampleWidth);
	}

	if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP)
	{
		if (track->f.sampleWidth != 8 &&
			track->f.sampleWidth != 16 &&
			track->f.sampleWidth != 24 &&
			track->f.sampleWidth != 32)
		{
			_af_error(AF_BAD_WIDTH, "invalid sample width %d for NeXT file (only 8-, 16-, 24-, and 32-bit data are allowed)");
			return AF_NULL_FILESETUP;
		}
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		track->f.compressionType != AF_COMPRESSION_G711_ULAW &&
		track->f.compressionType != AF_COMPRESSION_G711_ALAW)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "compression format not implemented for NeXT files");
		return AF_NULL_FILESETUP;
	}

	if (track->f.byteOrder != AF_BYTEORDER_BIGENDIAN && track->byteOrderSet)
	{
		_af_error(AF_BAD_BYTEORDER, "NeXT format supports only big-endian data");
		track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
	}

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "NeXT files cannot have AES data");
		return AF_NULL_FILESETUP;
	}

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_FILESETUP, "NeXT format does not support markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_FILESETUP, "NeXT format does not support instruments");
		return AF_NULL_FILESETUP;
	}

	if (setup->miscellaneousSet && setup->miscellaneousCount != 0)
	{
		_af_error(AF_BAD_FILESETUP, "NeXT format does not support miscellaneous data");
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &_af_next_default_filesetup, AF_FALSE);
}

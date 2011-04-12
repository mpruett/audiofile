/*
	Audio File Library
	Copyright (C) 2001, Silicon Graphics, Inc.
	Copyright (C) 2011, Michael Pruett <michael@68k.org>

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
	IRCAMWrite.cpp

	This file contains routines for writing to Berkeley/IRCAM/CARL
	format files.
*/

#include "config.h"
#include "IRCAM.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "File.h"
#include "Marker.h"
#include "Setup.h"
#include "Track.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"

/* We write IRCAM files using the native byte order. */
status IRCAMFile::writeInit(AFfilesetup setup)
{
	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	uint32_t dataOffset = SIZEOF_BSD_HEADER;

	Track *track = &tracks[0];
	track->totalfframes = 0;
	track->fpos_first_frame = dataOffset;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	/* Choose the magic number appropriate for the byte order. */
	const uint8_t *magic;
#ifdef WORDS_BIGENDIAN
	magic = _af_ircam_sun_be_magic;
#else
	magic = _af_ircam_vax_le_magic;
#endif

	uint32_t channels = track->f.channelCount;
	float rate = track->f.sampleRate;

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		track->f.compressionType != AF_COMPRESSION_G711_ULAW &&
		track->f.compressionType != AF_COMPRESSION_G711_ALAW)
	{
		_af_error(AF_BAD_COMPTYPE,
			"unsupported compression type %d in IRCAM sound file",
			track->f.compressionType);
		return AF_FAIL;
	}

	uint32_t packMode = 0;
	if (track->f.compressionType == AF_COMPRESSION_G711_ULAW)
		packMode = SF_ULAW;
	else if (track->f.compressionType == AF_COMPRESSION_G711_ALAW)
		packMode = SF_ALAW;
	else if (track->f.isSigned())
	{
		switch (track->f.bytesPerFrame(false))
		{
			case 1: packMode = SF_CHAR; break;
			case 2: packMode = SF_SHORT; break;
			case 3: packMode = SF_24INT; break;
			case 4: packMode = SF_LONG; break;
			default:
				_af_error(AF_BAD_SAMPFMT,
					"unsupported sample width %d for two's complement BICSF file",
					track->f.sampleWidth);
				return AF_FAIL;
		}
	}
	else if (track->f.isFloat())
	{
		if (track->f.sampleWidth == 32)
			packMode = SF_FLOAT;
		else if (track->f.sampleWidth == 64)
			packMode = SF_DOUBLE;
		else
		{
			_af_error(AF_BAD_SAMPFMT,
				"unsupported sample width %d for BICSF file",
				track->f.sampleWidth);
			return AF_FAIL;
		}
	}
	else if (track->f.isUnsigned())
	{
		_af_error(AF_BAD_SAMPFMT, "BICSF format does not support unsigned integer audio data");
		return AF_FAIL;
	}

	fh->seek(0, File::SeekFromBeginning);
	fh->write(magic, 4);
	writeFloat(&rate);
	writeU32(&channels);
	writeU32(&packMode);

	/* Zero the entire description block. */
	uint8_t zeros[SIZEOF_BSD_HEADER];
	memset(zeros, 0, SIZEOF_BSD_HEADER);
	fh->write(zeros, SIZEOF_BSD_HEADER - 4*4);

	return AF_SUCCEED;
}

status IRCAMFile::update()
{
	return AF_SUCCEED;
}

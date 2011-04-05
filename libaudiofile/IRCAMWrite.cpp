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
	IRCAMWrite.cpp

	This file contains routines for writing to Berkeley/IRCAM/CARL
	format files.
*/

#include "config.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Marker.h"
#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"

#include "IRCAM.h"

/* We write IRCAM files using the native byte order. */
status IRCAMFile::writeInit(AFfilesetup setup)
{
	float rate;
	uint32_t channels;
	uint32_t packMode;
	uint32_t dataOffset;
	uint8_t zeros[SIZEOF_BSD_HEADER];

	assert(fileFormat == AF_FILE_IRCAM);

	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	dataOffset = SIZEOF_BSD_HEADER;

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

	channels = track->f.channelCount;
	rate = track->f.sampleRate;

	assert(track->f.compressionType == AF_COMPRESSION_NONE);

	if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP)
	{
		assert(track->f.sampleWidth == 16);
		packMode = SF_SHORT;
	}
	else if (track->f.sampleFormat == AF_SAMPFMT_FLOAT)
	{
		assert(track->f.sampleWidth == 32);
		packMode = SF_FLOAT;
	}

	af_fseek(fh, 0, SEEK_SET);
	af_write(magic, 4, fh);
	writeF32(&rate);
	writeU32(&channels);
	writeU32(&packMode);

	/* Zero the entire description block. */
	memset(zeros, 0, SIZEOF_BSD_HEADER);
	af_write(zeros, SIZEOF_BSD_HEADER - 4*4, fh);

	return AF_SUCCEED;
}

status IRCAMFile::update()
{
	return AF_SUCCEED;
}

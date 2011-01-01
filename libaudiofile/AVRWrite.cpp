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
	AVRWrite.cpp

	This file contains routines for writing AVR (Audio Visual
	Research) sound files.
*/

#include "config.h"
#include "AVR.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"

status AVRFile::update()
{
	Track		*track;
	uint32_t	size, loopStart, loopEnd;

	track = _af_filehandle_get_track(this, AF_DEFAULT_TRACK);

	/* Seek to the position of the size field. */
	af_fseek(fh, 26, SEEK_SET);

	size = track->totalfframes;

	/* For the case of no loops, loopStart = 0 and loopEnd = size. */
	loopStart = 0;
	loopEnd = size;

	af_write_uint32_be(&size, fh);
	af_write_uint32_be(&loopStart, fh);
	af_write_uint32_be(&loopEnd, fh);

	return AF_SUCCEED;
}

static char *af_basename (char *filename)
{
	char	*base;
	base = strrchr(filename, '/');
	if (base == NULL)
		return filename;
	else
		return base + 1;
}

status AVRFile::writeInit(AFfilesetup setup)
{
	Track		*track;
	char		name[8];
	uint16_t	mono, resolution, sign, loop, midi;
	uint32_t	rate, size, loopStart, loopEnd;
	char		reserved[26];
	char		user[64];

	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	track = _af_filehandle_get_track(this, AF_DEFAULT_TRACK);

	if (af_fseek(fh, 0, SEEK_SET) != 0)
	{
		_af_error(AF_BAD_LSEEK, "bad seek");
		return AF_FAIL;
	}

	af_write("2BIT", 4, fh);
	memset(name, 0, 8);
	if (fileName != NULL)
		strncpy(name, af_basename(fileName), 8);
	af_write(name, 8, fh);

	if (track->f.channelCount == 1)
		mono = 0x0;
	else
		mono = 0xffff;
	af_write_uint16_be(&mono, fh);

	resolution = track->f.sampleWidth;
	af_write_uint16_be(&resolution, fh);

	if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
		sign = 0x0;
	else
		sign = 0xffff;
	af_write_uint16_be(&sign, fh);

	/* We do not currently support loops. */
	loop = 0;
	af_write_uint16_be(&loop, fh);
	midi = 0xffff;
	af_write_uint16_be(&midi, fh);

	rate = track->f.sampleRate;
	/* Set the high-order byte of rate to 0xff. */
	rate |= 0xff000000;
	size = track->totalfframes;
	loopStart = 0;
	loopEnd = size;

	af_write_uint32_be(&rate, fh);
	af_write_uint32_be(&size, fh);
	af_write_uint32_be(&loopStart, fh);
	af_write_uint32_be(&loopEnd, fh);

	memset(reserved, 0, 26);
	af_write(reserved, 26, fh);

	memset(user, 0, 64);
	af_write(user, 64, fh);

	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = af_ftell(fh);

	return AF_SUCCEED;
}

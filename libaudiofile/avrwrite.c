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
	avrwrite.c

	This file contains routines for writing AVR (Audio Visual
	Research) sound files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "audiofile.h"
#include "afinternal.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"

#include "avr.h"

status _af_avr_update (AFfilehandle file)
{
	_Track		*track;
	u_int32_t	size, loopStart, loopEnd;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	/* Seek to the position of the size field. */
	af_fseek(file->fh, 26, SEEK_SET);

	size = track->totalfframes;

	/* For the case of no loops, loopStart = 0 and loopEnd = size. */
	loopStart = 0;
	loopEnd = size;

	af_write_uint32_be(&size, file->fh);
	af_write_uint32_be(&loopStart, file->fh);
	af_write_uint32_be(&loopEnd, file->fh);

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

status _af_avr_write_init (AFfilesetup setup, AFfilehandle filehandle)
{
	_Track		*track;
	char		name[8];
	u_int16_t	mono, resolution, sign, loop, midi;
	u_int32_t	rate, size, loopStart, loopEnd;
	char		reserved[26];
	char		user[64];

	if (_af_filesetup_make_handle(setup, filehandle) == AF_FAIL)
		return AF_FAIL;

	filehandle->formatSpecific = NULL;

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	if (af_fseek(filehandle->fh, 0, SEEK_SET) != 0)
	{
		_af_error(AF_BAD_LSEEK, "bad seek");
		return AF_FAIL;
	}

	af_fwrite("2BIT", 4, 1, filehandle->fh);
	memset(name, 0, 8);
	if (filehandle->fileName != NULL)
		strncpy(name, af_basename(filehandle->fileName), 8);
	af_fwrite(name, 8, 1, filehandle->fh);

	if (track->f.channelCount == 1)
		mono = 0x0;
	else
		mono = 0xffff;
	af_write_uint16_be(&mono, filehandle->fh);

	resolution = track->f.sampleWidth;
	af_write_uint16_be(&resolution, filehandle->fh);

	if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
		sign = 0x0;
	else
		sign = 0xffff;
	af_write_uint16_be(&sign, filehandle->fh);

	/* We do not currently support loops. */
	loop = 0;
	af_write_uint16_be(&loop, filehandle->fh);
	midi = 0xffff;
	af_write_uint16_be(&midi, filehandle->fh);

	rate = track->f.sampleRate;
	/* Set the high-order byte of rate to 0xff. */
	rate |= 0xff000000;
	size = track->totalfframes;
	loopStart = 0;
	loopEnd = size;

	af_write_uint32_be(&rate, filehandle->fh);
	af_write_uint32_be(&size, filehandle->fh);
	af_write_uint32_be(&loopStart, filehandle->fh);
	af_write_uint32_be(&loopEnd, filehandle->fh);

	memset(reserved, 0, 26);
	af_fwrite(reserved, 26, 1, filehandle->fh);

	memset(user, 0, 64);
	af_fwrite(user, 64, 1, filehandle->fh);

	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = af_ftell(filehandle->fh);

	return AF_SUCCEED;
}

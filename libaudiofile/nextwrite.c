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
	nextwrite.c

	This file contains routines for writing NeXT/Sun format sound files.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"
#include "next.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"

status _af_next_update (AFfilehandle file);

static u_int32_t nextencodingtype (_AudioFormat *format);
static status next_write_header (AFfilehandle file);

/* A return value of zero indicates successful synchronisation. */
status _af_next_update (AFfilehandle file)
{
	next_write_header(file);
	return AF_SUCCEED;
}

static status next_write_header (AFfilehandle file)
{
	_Track		*track;
	int		frameSize;
	u_int32_t	offset, length, encoding, sampleRate, channelCount;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	frameSize = _af_format_frame_size(&track->f, AF_FALSE);

	offset = HOST_TO_BENDIAN_INT32(track->fpos_first_frame);
	length = HOST_TO_BENDIAN_INT32(track->totalfframes * frameSize);
	encoding = HOST_TO_BENDIAN_INT32(nextencodingtype(&track->f));
	sampleRate = HOST_TO_BENDIAN_INT32(track->f.sampleRate);
	channelCount = HOST_TO_BENDIAN_INT32(track->f.channelCount);

	if (af_fseek(file->fh, 0, SEEK_SET) != 0)
		_af_error(AF_BAD_LSEEK, "bad seek");

	af_fwrite(".snd", 4, 1, file->fh);
	af_fwrite(&offset, 4, 1, file->fh);
	af_fwrite(&length, 4, 1, file->fh);
	af_fwrite(&encoding, 4, 1, file->fh);
	af_fwrite(&sampleRate, 4, 1, file->fh);
	af_fwrite(&channelCount, 4, 1, file->fh);

	return AF_SUCCEED;
}

static u_int32_t nextencodingtype (_AudioFormat *format)
{
	u_int32_t	encoding = 0;

	if (format->compressionType != AF_COMPRESSION_NONE)
	{
		if (format->compressionType == AF_COMPRESSION_G711_ULAW)
			encoding = _AU_FORMAT_MULAW_8;
		else if (format->compressionType == AF_COMPRESSION_G711_ALAW)
			encoding = _AU_FORMAT_ALAW_8;
	}
	else if (format->sampleFormat == AF_SAMPFMT_TWOSCOMP)
	{
		if (format->sampleWidth == 8)
			encoding = _AU_FORMAT_LINEAR_8;
		else if (format->sampleWidth == 16)
			encoding = _AU_FORMAT_LINEAR_16;
		else if (format->sampleWidth == 24)
			encoding = _AU_FORMAT_LINEAR_24;
		else if (format->sampleWidth == 32)
			encoding = _AU_FORMAT_LINEAR_32;
	}
	else if (format->sampleFormat == AF_SAMPFMT_FLOAT)
		encoding = _AU_FORMAT_FLOAT;
	else if (format->sampleFormat == AF_SAMPFMT_DOUBLE)
		encoding = _AU_FORMAT_DOUBLE;

	return encoding;
}

status _af_next_write_init (AFfilesetup setup, AFfilehandle filehandle)
{
	_Track	*track;

	if (_af_filesetup_make_handle(setup, filehandle) == AF_FAIL)
		return AF_FAIL;

	filehandle->formatSpecific = NULL;

	if (filehandle->miscellaneousCount > 0)
	{
		_af_error(AF_BAD_NUMMISC, "NeXT format supports no miscellaneous chunks");
		return AF_FAIL;
	}

	next_write_header(filehandle);

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);
	track->fpos_first_frame = 28;

	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	return AF_SUCCEED;
}

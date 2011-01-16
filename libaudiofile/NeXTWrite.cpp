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
	NeXTWrite.cpp

	This file contains routines for writing NeXT/Sun format sound files.
*/

#include "config.h"
#include "NeXT.h"

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

status _af_next_update (AFfilehandle file);

static uint32_t nextencodingtype (AudioFormat *format);

/* A return value of zero indicates successful synchronisation. */
status NeXTFile::update()
{
	writeHeader();
	return AF_SUCCEED;
}

status NeXTFile::writeHeader()
{
	Track *track = getTrack();

	if (af_fseek(fh, 0, SEEK_SET) != 0)
		_af_error(AF_BAD_LSEEK, "bad seek");

	uint32_t offset = track->fpos_first_frame;
	int frameSize = _af_format_frame_size(&track->f, false);
	uint32_t length = track->totalfframes * frameSize;
	uint32_t encoding = nextencodingtype(&track->f);
	uint32_t sampleRate = track->f.sampleRate;
	uint32_t channelCount = track->f.channelCount;

	af_write(".snd", 4, fh);
	writeU32(&offset);
	writeU32(&length);
	writeU32(&encoding);
	writeU32(&sampleRate);
	writeU32(&channelCount);

	return AF_SUCCEED;
}

static uint32_t nextencodingtype (AudioFormat *format)
{
	uint32_t encoding = 0;

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

status NeXTFile::writeInit(AFfilesetup setup)
{
	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	if (miscellaneousCount > 0)
	{
		_af_error(AF_BAD_NUMMISC, "NeXT format supports no miscellaneous chunks");
		return AF_FAIL;
	}

	writeHeader();

	Track *track = getTrack();
	track->fpos_first_frame = 28;

	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	return AF_SUCCEED;
}

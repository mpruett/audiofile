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
	NISTWrite.cpp

	This file contains routines for writing to NIST SPHERE
	format files.
*/

#include "config.h"
#include "NIST.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"

static const char *sample_byte_format (AudioFormat *fmt)
{
	int nbytes = _af_format_sample_size(fmt, false);

	assert(nbytes == 1 || nbytes == 2);

	if (nbytes == 1)
		return "0";
	else if (nbytes == 2)
	{
		if (fmt->byteOrder == AF_BYTEORDER_BIGENDIAN)
			return "10";
		else
			return "01";
	}

	/* NOTREACHED */
	return NULL;
}

static const char *sample_coding (AudioFormat *fmt)
{
	if (fmt->compressionType == AF_COMPRESSION_NONE)
		return "pcm";
	else if (fmt->compressionType == AF_COMPRESSION_G711_ULAW)
		return "ulaw";
	else if (fmt->compressionType == AF_COMPRESSION_G711_ALAW)
		return "alaw";

	/* NOTREACHED */
	return NULL;
}

status NISTFile::writeHeader()
{
	Track *track = getTrack();

	char header[NIST_SPHERE_HEADER_LENGTH];
	int printed = snprintf(header, NIST_SPHERE_HEADER_LENGTH,
		"NIST_1A\n   1024\n"
		"channel_count -i %d\n"
		"sample_count -i %d\n"
		"sample_rate -i %d\n"
		"sample_n_bytes -i %d\n"
		"sample_byte_format -s%d %s\n"
		"sample_sig_bits -i %d\n"
		"sample_coding -s%d %s\n"
		"end_head\n",
		track->f.channelCount,
		(int) (track->totalfframes * track->f.channelCount),
		(int) track->f.sampleRate,
		(int) _af_format_sample_size(&track->f, false),
		(int) _af_format_sample_size(&track->f, false), sample_byte_format(&track->f),
		track->f.sampleWidth,
		(int) strlen(sample_coding(&track->f)), sample_coding(&track->f));

	/* Fill the remaining space in the buffer with space characters. */
	if (printed < NIST_SPHERE_HEADER_LENGTH)
		memset(header + printed, ' ', NIST_SPHERE_HEADER_LENGTH - printed);

	return af_write(header, NIST_SPHERE_HEADER_LENGTH, fh) == NIST_SPHERE_HEADER_LENGTH ? AF_SUCCEED : AF_FAIL;
}

status NISTFile::writeInit(AFfilesetup setup)
{
	Track		*track;

	assert(fileFormat == AF_FILE_NIST_SPHERE);

	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	track = &tracks[0];
	track->totalfframes = 0;
	track->fpos_first_frame = NIST_SPHERE_HEADER_LENGTH;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	af_fseek(fh, 0, SEEK_SET);
	writeHeader();

	return AF_SUCCEED;
}

status NISTFile::update()
{
	af_fseek(fh, 0, SEEK_SET);
	writeHeader();

	return AF_SUCCEED;
}

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
	nistwrite.c

	This file contains routines for writing to NIST SPHERE
	format files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "afinternal.h"
#include "audiofile.h"
#include "util.h"
#include "byteorder.h"
#include "setup.h"
#include "track.h"

#include "nist.h"

static char *sample_byte_format (_AudioFormat *fmt)
{
	int	nbytes = _af_format_sample_size(fmt, AF_FALSE);

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

static char *sample_coding (_AudioFormat *fmt)
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

status WriteNISTHeader (AFfilehandle file)
{
	AFvirtualfile	*fp = file->fh;
	_Track		*track;
	char		header[NIST_SPHERE_HEADER_LENGTH];
	int		printed;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	printed = snprintf(header, NIST_SPHERE_HEADER_LENGTH,
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
		(int) _af_format_sample_size(&track->f, AF_FALSE),
		(int) _af_format_sample_size(&track->f, AF_FALSE), sample_byte_format(&track->f),
		track->f.sampleWidth,
		(int) strlen(sample_coding(&track->f)), sample_coding(&track->f));

	/* Fill the remaining space in the buffer with space characters. */
	if (printed < NIST_SPHERE_HEADER_LENGTH)
		memset(header + printed, ' ', NIST_SPHERE_HEADER_LENGTH - printed);

	return af_fwrite(header, NIST_SPHERE_HEADER_LENGTH, 1, fp);
}

status _af_nist_write_init (AFfilesetup setup, AFfilehandle handle)
{
	_Track		*track;

	assert(handle->fileFormat == AF_FILE_NIST_SPHERE);

	if (_af_filesetup_make_handle(setup, handle) == AF_FAIL)
		return AF_FAIL;

	track = &handle->tracks[0];
	track->totalfframes = 0;
	track->fpos_first_frame = NIST_SPHERE_HEADER_LENGTH;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	handle->formatSpecific = NULL;

	af_fseek(handle->fh, 0, SEEK_SET);
	WriteNISTHeader(handle);

	return AF_SUCCEED;
}

status _af_nist_update (AFfilehandle file)
{
	af_fseek(file->fh, 0, SEEK_SET);
	WriteNISTHeader(file);

	return AF_SUCCEED;
}

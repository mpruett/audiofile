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
	ircamwrite.c

	This file contains routines for writing to Berkeley/IRCAM/CARL
	format files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "afinternal.h"
#include "audiofile.h"
#include "util.h"
#include "byteorder.h"
#include "setup.h"
#include "track.h"
#include "marker.h"

#include "ircam.h"

/*
	These magic numbers are fucking stupid.

	Here ircam_mips_magic refers to little-endian MIPS, not SGI IRIX,
	which uses big-endian MIPS.
*/
extern const u_int8_t ircam_vax_magic[4], ircam_sun_magic[4],
	ircam_mips_magic[4], ircam_next_magic[4];

/* We write IRCAM files using the native byte order. */
status _af_ircam_write_init (AFfilesetup setup, AFfilehandle handle)
{
	_Track		*track;
	u_int8_t	*magic;
	float		rate;
	u_int32_t	channels;
	u_int32_t	packMode;
	u_int32_t	dataOffset;
	u_int8_t	zeros[SIZEOF_BSD_HEADER];

	float		maxAmp = 1.0;

	bool		isSwapped, isLittleEndian;

	assert(handle->fileFormat == AF_FILE_IRCAM);

	if (_af_filesetup_make_handle(setup, handle) == AF_FAIL)
		return AF_FAIL;

	dataOffset = SIZEOF_BSD_HEADER;

	track = &handle->tracks[0];
	track->totalfframes = 0;
	track->fpos_first_frame = dataOffset;
	track->nextfframe = 0;
	track->fpos_next_frame = track->fpos_first_frame;

	handle->formatSpecific = NULL;

	/* Choose the magic number appropriate for the byte order. */
#ifdef WORDS_BIGENDIAN
	magic = ircam_sun_magic;
#else
	magic = ircam_vax_magic;
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

	af_fseek(handle->fh, 0, SEEK_SET);
	af_fwrite(magic, 4, 1, handle->fh);
	af_fwrite(&rate, 4, 1, handle->fh);
	af_fwrite(&channels, 4, 1, handle->fh);
	af_fwrite(&packMode, 4, 1, handle->fh);

	/* Zero the entire description block. */
	memset(zeros, 0, SIZEOF_BSD_HEADER);
	af_fwrite(zeros, SIZEOF_BSD_HEADER - 4*4, 1, handle->fh);

	return AF_SUCCEED;
}

status _af_ircam_update (AFfilehandle file)
{
	return AF_SUCCEED;
}

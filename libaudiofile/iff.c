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
	iff.c

	This file contains routines for parsing IFF/8SVX sound
	files.
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

#include "audiofile.h"
#include "afinternal.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"
#include "track.h"
#include "marker.h"

#include "iff.h"

static status ParseMiscellaneous (AFfilehandle file, AFvirtualfile *fh,
	u_int32_t type, size_t size);
static status ParseVHDR (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);
static status ParseBODY (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);

_AFfilesetup _af_iff_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_IFF_8SVX,	/* fileFormat */
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

bool _af_iff_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 1, 8, fh) != 8 || memcmp(buffer, "FORM", 4) != 0)
		return AF_FALSE;
	if (af_fread(buffer, 1, 4, fh) != 4 || memcmp(buffer, "8SVX", 4) != 0)
		return AF_FALSE;

	return AF_TRUE;
}

/*
	Parse miscellaneous data chunks such as name, author, copyright,
	and annotation chunks.
*/
static status ParseMiscellaneous (AFfilehandle file, AFvirtualfile *fh,
	u_int32_t type, size_t size)
{
	int	misctype = AF_MISC_UNRECOGNIZED;

	assert(!memcmp(&type, "NAME", 4) || !memcmp(&type, "AUTH", 4) ||
		!memcmp(&type, "(c) ", 4) || !memcmp(&type, "ANNO", 4));

	/* Skip zero-length miscellaneous chunks. */
	if (size == 0)
		return AF_FAIL;

	file->miscellaneousCount++;
	file->miscellaneous = _af_realloc(file->miscellaneous,
		file->miscellaneousCount * sizeof (_Miscellaneous));

	if (!memcmp(&type, "NAME", 4))
		misctype = AF_MISC_NAME;
	else if (!memcmp(&type, "AUTH", 4))
		misctype = AF_MISC_AUTH;
	else if (!memcmp(&type, "(c) ", 4))
		misctype = AF_MISC_COPY;
	else if (!memcmp(&type, "ANNO", 4))
		misctype = AF_MISC_ANNO;

	file->miscellaneous[file->miscellaneousCount - 1].id = file->miscellaneousCount;
	file->miscellaneous[file->miscellaneousCount - 1].type = misctype;
	file->miscellaneous[file->miscellaneousCount - 1].size = size;
	file->miscellaneous[file->miscellaneousCount - 1].position = 0;
	file->miscellaneous[file->miscellaneousCount - 1].buffer = _af_malloc(size);
	af_fread(file->miscellaneous[file->miscellaneousCount - 1].buffer,
		size, 1, file->fh);

	return AF_SUCCEED;
}

/*
	Parse voice header chunk.
*/
static status ParseVHDR (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size)
{
	_Track		*track;
	u_int32_t	oneShotSamples, repeatSamples, samplesPerRepeat;
	u_int16_t	sampleRate;
	u_int8_t	octaves, compression;
	u_int32_t	volume;

	assert(!memcmp(&type, "VHDR", 4));

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_read_uint32_be(&oneShotSamples, fh);
	af_read_uint32_be(&repeatSamples, fh);
	af_read_uint32_be(&samplesPerRepeat, fh);
	af_read_uint16_be(&sampleRate, fh);
	af_fread(&octaves, 1, 1, fh);
	af_fread(&compression, 1, 1, fh);
	af_read_uint32_be(&volume, fh);

	track->f.sampleWidth = 8; 
	track->f.sampleRate = sampleRate;
	track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
	track->f.compressionType = AF_COMPRESSION_NONE;
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
	track->f.channelCount = 1;

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	return AF_SUCCEED;
}

static status ParseBODY (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size)
{
	_Track		*track;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	/*
		IFF/8SVX files have only one audio channel with one
		byte per sample, so the number of frames is equal to
		the number of bytes.
	*/
	track->totalfframes = size;
	track->data_size = size;

	/* Sound data follows. */
	track->fpos_first_frame = af_ftell(fh);

	return AF_SUCCEED;
}

status _af_iff_read_init (AFfilesetup setup, AFfilehandle file)
{
	u_int32_t	type, size, formtype;
	size_t		index;
	_Track		*track;

	assert(file != NULL);
	assert(file->fh != NULL);

	af_fseek(file->fh, 0, SEEK_SET);

	af_fread(&type, 4, 1, file->fh);
	af_read_uint32_be(&size, file->fh);
	af_fread(&formtype, 4, 1, file->fh);

	if (memcmp(&type, "FORM", 4) != 0 || memcmp(&formtype, "8SVX", 4) != 0)
		return AF_FAIL;

	file->instrumentCount = 0;
	file->instruments = NULL;
	file->miscellaneousCount = 0;
	file->miscellaneous = NULL;

	/* IFF/8SVX files have only one track. */
	track = _af_track_new();
	file->trackCount = 1;
	file->tracks = track;

	/* Set the index to include the form type ('8SVX' in this case). */
	index = 4;

	while (index < size)
	{
		u_int32_t	chunkid = 0, chunksize = 0;
		status		result = AF_SUCCEED;

		af_fread(&chunkid, 4, 1, file->fh);
		af_read_uint32_be(&chunksize, file->fh);

		if (!memcmp("VHDR", &chunkid, 4))
		{
			result = ParseVHDR(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("BODY", &chunkid, 4))
		{
			result = ParseBODY(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("NAME", &chunkid, 4) ||
			!memcmp("AUTH", &chunkid, 4) ||
			!memcmp("(c) ", &chunkid, 4) ||
			!memcmp("ANNO", &chunkid, 4))
		{
			ParseMiscellaneous(file, file->fh, chunkid, chunksize);
		}

		if (result == AF_FAIL)
			return AF_FAIL;

		/*
			Increment the index by the size of the chunk
			plus the size of the chunk header.
		*/
		index += chunksize + 8;

		/* All chunks must be aligned on an even number of bytes. */
		if ((index % 2) != 0)
			index++;

		/* Set the seek position to the beginning of the next chunk. */
		af_fseek(file->fh, index + 8, SEEK_SET);
	}

	/* The file has been successfully parsed. */
	return AF_SUCCEED;
}

AFfilesetup _af_iff_complete_setup (AFfilesetup setup)
{
	_TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "IFF/8SVX file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	track = &setup->tracks[0];

	if (track->sampleFormatSet &&
		track->f.sampleFormat != AF_SAMPFMT_TWOSCOMP)
	{
		_af_error(AF_BAD_SAMPFMT,
			"IFF/8SVX format supports only two's complement integer data");
		return AF_NULL_FILESETUP;
	}

	if (track->sampleFormatSet && track->f.sampleWidth != 8)
	{
		_af_error(AF_BAD_WIDTH,
			"IFF/8SVX file allows only 8 bits per sample "
			"(%d bits requested)", track->f.sampleWidth);
		return AF_NULL_FILESETUP;
	}

	if (track->channelCountSet && track->f.channelCount != 1)
	{
		_af_error(AF_BAD_CHANNELS,
			"invalid channel count (%d) for IFF/8SVX format "
			"(only 1 channel supported)",
			track->f.channelCount);
		return AF_NULL_FILESETUP;
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IFF/8SVX does not support compression");
		return AF_NULL_FILESETUP;
	}

	/* Ignore requested byte order since samples are only one byte. */
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
	/* Either one channel was requested or no request was made. */
	track->f.channelCount = 1;
	_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, 8);

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_NUMMARKS,
			"IFF/8SVX format does not support markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_NUMINSTS,
			"IFF/8SVX format does not support instruments");
		return AF_NULL_FILESETUP;
	}

	if (setup->miscellaneousSet && setup->miscellaneousCount != 0)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "IFF/8SVX format does not "
			"currently support miscellaneous chunks");
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &_af_iff_default_filesetup, AF_TRUE);
}

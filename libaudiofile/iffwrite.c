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
	iffwrite.c

	This file contains routines for writing IFF/8SVX format sound
	files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"

#include "iff.h"

status _af_iff_update (AFfilehandle file);

static status WriteVHDR (AFfilehandle file);
static status WriteMiscellaneous (AFfilehandle file);
static status WriteBODY (AFfilehandle file);

static _IFFinfo *iff_info_new (void)
{
	_IFFinfo	*iff = _af_calloc(1, sizeof (_IFFinfo));

	iff->miscellaneousPosition = 0;
	iff->VHDR_offset = 0;
	iff->BODY_offset = 0;

	return iff;
}

status _af_iff_write_init (AFfilesetup setup, AFfilehandle file)
{
	u_int32_t	fileSize = 0;

	if (_af_filesetup_make_handle(setup, file) == AF_FAIL)
		return AF_FAIL;

	file->formatSpecific = iff_info_new();

	af_fwrite("FORM", 4, 1, file->fh);
	af_write_uint32_be(&fileSize, file->fh);

	af_fwrite("8SVX", 4, 1, file->fh);

	WriteVHDR(file);
	WriteMiscellaneous(file);
	WriteBODY(file);

	return AF_SUCCEED;
}

status _af_iff_update (AFfilehandle file)
{
	u_int32_t	length;

	WriteVHDR(file);
	WriteMiscellaneous(file);
	WriteBODY(file);

	/* Get the length of the file. */
	length = af_flength(file->fh);
	length -= 8;

	/* Set the length of the FORM chunk. */
	af_fseek(file->fh, 4, SEEK_SET);
	af_write_uint32_be(&length, file->fh);

	return AF_SUCCEED;
}

static status WriteVHDR (const AFfilehandle file)
{
	_Track		*track;
	_IFFinfo	*iff;
	u_int32_t	chunkSize;
	u_int32_t	oneShotSamples, repeatSamples, samplesPerRepeat;
	u_int16_t	sampleRate;
	u_int8_t	octaves, compression;
	u_int32_t	volume;

	iff = (_IFFinfo *) file->formatSpecific;

	/*
		If VHDR_offset hasn't been set yet, set it to the
		current offset.
	*/
	if (iff->VHDR_offset == 0)
		iff->VHDR_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, iff->VHDR_offset, SEEK_SET);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fwrite("VHDR", 4, 1, file->fh);

	chunkSize = 20;
	af_write_uint32_be(&chunkSize, file->fh);

	/*
		IFF/8SVX files have only one audio channel, so the
		number of samples is equal to the number of frames.
	*/
	oneShotSamples = track->totalfframes;
	af_write_uint32_be(&oneShotSamples, file->fh);
	repeatSamples = 0;
	af_write_uint32_be(&repeatSamples, file->fh);
	samplesPerRepeat = 0;
	af_write_uint32_be(&samplesPerRepeat, file->fh);

	sampleRate = track->f.sampleRate;
	af_write_uint16_be(&sampleRate, file->fh);

	octaves = 0;
	compression = 0;
	af_fwrite(&octaves, 1, 1, file->fh);
	af_fwrite(&compression, 1, 1, file->fh);

	/* Volume is in fixed-point notation; 65536 means gain of 1.0. */
	volume = 65536;
	af_write_uint32_be(&volume, file->fh);

	return AF_SUCCEED;
}

static status WriteBODY (AFfilehandle file)
{
	_Track		*track;
	u_int32_t	chunkSize;
	_IFFinfo	*iff = (_IFFinfo *) file->formatSpecific;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	if (iff->BODY_offset == 0)
		iff->BODY_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, iff->BODY_offset, SEEK_SET);

	af_fwrite("BODY", 4, 1, file->fh);

	/*
		IFF/8SVX supports only one channel, so the number of
		frames is equal to the number of samples, and each
		sample is one byte.
	*/
	chunkSize = track->totalfframes;
	af_write_uint32_be(&chunkSize, file->fh);

	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = af_ftell(file->fh);

	/* Add a pad byte to the end of the chunk if the chunk size is odd. */
	if ((chunkSize % 2) == 1)
	{
		u_int8_t	zero = 0;
		af_fseek(file->fh, iff->BODY_offset + 8 + chunkSize, SEEK_SET);
		af_fwrite(&zero, 1, 1, file->fh);
	}

	return AF_SUCCEED;
}

/*
	WriteMiscellaneous writes all the miscellaneous data chunks in a
	file handle structure to an IFF/8SVX file.
*/
static status WriteMiscellaneous (AFfilehandle file)
{
	_IFFinfo	*iff;
	int		i;

	iff = (_IFFinfo *) file->formatSpecific;

	if (iff->miscellaneousPosition == 0)
		iff->miscellaneousPosition = af_ftell(file->fh);
	else
		af_fseek(file->fh, iff->miscellaneousPosition, SEEK_SET);

	for (i=0; i<file->miscellaneousCount; i++)
	{
		_Miscellaneous	*misc = &file->miscellaneous[i];
		u_int32_t	chunkType, chunkSize;
		u_int8_t	padByte = 0;

		switch (misc->type)
		{
			case AF_MISC_NAME:
				memcpy(&chunkType, "NAME", 4); break;
			case AF_MISC_AUTH:
				memcpy(&chunkType, "AUTH", 4); break;
			case AF_MISC_COPY:
				memcpy(&chunkType, "(c) ", 4); break;
			case AF_MISC_ANNO:
				memcpy(&chunkType, "ANNO", 4); break;
		}

		af_fwrite(&chunkType, 4, 1, file->fh);

		chunkSize = misc->size;
		af_write_uint32_be(&chunkSize, file->fh);

		/*
			Write the miscellaneous buffer and then a pad byte
			if necessary.  If the buffer is null, skip the space
			for now.
		*/
		if (misc->buffer != NULL)
			af_fwrite(misc->buffer, misc->size, 1, file->fh);
		else
			af_fseek(file->fh, misc->size, SEEK_CUR);

		if (misc->size % 2 != 0)
			af_fwrite(&padByte, 1, 1, file->fh);
	}

	return AF_SUCCEED;
}

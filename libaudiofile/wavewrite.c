/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>
	Copyright (C) 2000, Silicon Graphics, Inc.

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
	wavewrite.c

	This file contains routines which facilitate writing to WAVE files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "audiofile.h"
#include "afinternal.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"
#include "wave.h"

status _af_wave_update (AFfilehandle file);

static status WriteFormat (AFfilehandle file);
static status WriteMiscellaneous (AFfilehandle file);
static status WriteData (AFfilehandle file);

_WAVEInfo *waveinfo_new (void)
{
	_WAVEInfo	*waveinfo = _af_malloc(sizeof (_WAVEInfo));

	waveinfo->fileSizeOffset = 0;
	waveinfo->sampleRateOffset = 0;
	waveinfo->miscellaneousStartOffset = 0;
	waveinfo->totalMiscellaneousSize = 0;
	waveinfo->dataSizeOffset = 0;

	return waveinfo;
}

static status WriteFormat (AFfilehandle file)
{
	_Track		*track = NULL;
	u_int16_t	formatTag, channelCount;
	u_int32_t	sampleRate, averageBytesPerSecond;
	u_int16_t	blockAlign;
	u_int32_t	chunkSize;
	u_int16_t	bitsPerSample;
	_WAVEInfo	*waveinfo = NULL;

	assert(file != NULL);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);
	waveinfo = (_WAVEInfo *) file->formatSpecific;

	af_fwrite("fmt ", 4, 1, file->fh);

	switch (track->f.compressionType)
	{
		case AF_COMPRESSION_NONE:
			chunkSize = 16;
			formatTag = WAVE_FORMAT_PCM;
			blockAlign = _af_format_frame_size(&track->f, AF_FALSE);
			bitsPerSample = 8 * _af_format_sample_size(&track->f, AF_FALSE);
			break;
		case AF_COMPRESSION_G711_ULAW:
			chunkSize = 18;
			formatTag = IBM_FORMAT_MULAW;
			blockAlign = _af_format_frame_size(&track->f, AF_FALSE);
			bitsPerSample = 8 * _af_format_sample_size(&track->f, AF_FALSE);
			break;
		case AF_COMPRESSION_G711_ALAW:
			chunkSize = 18;
			formatTag = IBM_FORMAT_ALAW;
			blockAlign = _af_format_frame_size(&track->f, AF_FALSE);
			bitsPerSample = 8 * _af_format_sample_size(&track->f, AF_FALSE);
			break;
		default:
			_af_error(AF_BAD_COMPTYPE, "bad compression type");
			return AF_FAIL;
	}

	chunkSize = HOST_TO_LENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);

	formatTag = HOST_TO_LENDIAN_INT16(formatTag);
	af_fwrite(&formatTag, 2, 1, file->fh);
	formatTag = LENDIAN_TO_HOST_INT16(formatTag);

	channelCount = track->f.channelCount;
	channelCount = HOST_TO_LENDIAN_INT16(channelCount);
	af_fwrite(&channelCount, 2, 1, file->fh);

	sampleRate = track->f.sampleRate;
	sampleRate = HOST_TO_LENDIAN_INT32(sampleRate);
	af_fwrite(&sampleRate, 4, 1, file->fh);

	averageBytesPerSecond =
		track->f.sampleRate * _af_format_frame_size(&track->f, AF_FALSE);
	averageBytesPerSecond = HOST_TO_LENDIAN_INT32(averageBytesPerSecond);
	af_fwrite(&averageBytesPerSecond, 4, 1, file->fh);

	blockAlign = _af_format_frame_size(&track->f, AF_FALSE);
	blockAlign = HOST_TO_LENDIAN_INT16(blockAlign);
	af_fwrite(&blockAlign, 2, 1, file->fh);

	bitsPerSample = HOST_TO_LENDIAN_INT16(bitsPerSample);
	af_fwrite(&bitsPerSample, 2, 1, file->fh);

	/*
		If the data is compressed we have additional
		format-specific information to write as well as the
		'fact' (frame count) chunk.
	*/
	if (track->f.compressionType != AF_COMPRESSION_NONE)
	{
		u_int32_t	factSize = 4;
		u_int32_t	totalFrameCount = 0;

		if (track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
			track->f.compressionType == AF_COMPRESSION_G711_ALAW)
		{
			u_int16_t	zero = 0;
			af_fwrite(&zero, 2, 1, file->fh);
		}

		af_fwrite("fact", 4, 1, file->fh);
		factSize = HOST_TO_LENDIAN_INT32(factSize);
		af_fwrite(&factSize, 4, 1, file->fh);

		waveinfo->fileSizeOffset = af_ftell(file->fh);

		totalFrameCount = HOST_TO_LENDIAN_INT32(totalFrameCount);
		af_fwrite(&totalFrameCount, 4, 1, file->fh);
	}

	return AF_SUCCEED;
}

static status WriteData (AFfilehandle file)
{
	_Track		*track;
	u_int32_t	frameSize, chunkSize;
	_WAVEInfo	*waveinfo;

	assert(file);

	waveinfo = file->formatSpecific;
	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fwrite("data", 4, 1, file->fh);
	waveinfo->dataSizeOffset = af_ftell(file->fh);

	frameSize = _af_format_frame_size(&track->f, AF_FALSE);
	chunkSize = frameSize * track->totalfframes;
	chunkSize = HOST_TO_LENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);
	track->fpos_first_frame = af_ftell(file->fh);

	return AF_SUCCEED;
}

status _af_wave_update (AFfilehandle file)
{
	_Track	*track;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	if (track->fpos_first_frame != 0)
	{
		size_t	dataLength, fileLength;

		/*
			We call _af_format_frame_size to calculate the
			frame size of normal PCM data or compressed data.
		*/
		dataLength = track->totalfframes * _af_format_frame_size(&track->f, AF_FALSE);

		dataLength = HOST_TO_LENDIAN_INT32(dataLength);
		af_fseek(file->fh, track->fpos_first_frame - 4, SEEK_SET);
		af_fwrite(&dataLength, 4, 1, file->fh);

		fileLength = af_flength(file->fh);
		fileLength -= 8;
		fileLength = HOST_TO_LENDIAN_INT32(fileLength);

		af_fseek(file->fh, 4, SEEK_SET);
		af_fwrite(&fileLength, 4, 1, file->fh);
	}

	return AF_SUCCEED;
}

status WriteMiscellaneous (AFfilehandle filehandle)
{
	_WAVEInfo	*wave = (_WAVEInfo *) filehandle->formatSpecific;

	if (filehandle->miscellaneousCount != 0)
	{
		int		i;
		u_int32_t	miscellaneousBytes;

		/* Start at 4 to account for 'INFO' chunk id. */
		miscellaneousBytes = 4;

		for (i=0; i<filehandle->miscellaneousCount; i++)
		{
			/* Account for miscellaneous type and size. */
			miscellaneousBytes += 8;
			miscellaneousBytes += filehandle->miscellaneous[i].size;

			/* Add a pad byte if necessary. */
			if (filehandle->miscellaneous[i].size % 2 != 0)
				miscellaneousBytes++;

			assert(miscellaneousBytes % 2 == 0);
		}

		wave->miscellaneousStartOffset = af_ftell(filehandle->fh);
		wave->totalMiscellaneousSize = miscellaneousBytes;

		/* Add 8 to account for length of 'LIST' chunk id and size. */
		af_fseek(filehandle->fh, miscellaneousBytes + 8, SEEK_CUR);
	}

	return AF_SUCCEED;
}

status _af_wave_write_init (AFfilesetup setup, AFfilehandle filehandle)
{
	u_int32_t	zero = 0;

	if (_af_filesetup_make_handle(setup, filehandle) == AF_FAIL)
		return AF_FAIL;

	filehandle->formatSpecific = waveinfo_new();

	af_fseek(filehandle->fh, 0, SEEK_SET);
	af_fwrite("RIFF", 4, 1, filehandle->fh);
	af_fwrite(&zero, 4, 1, filehandle->fh);
	af_fwrite("WAVE", 4, 1, filehandle->fh);

	WriteMiscellaneous(filehandle);
	WriteFormat(filehandle);
	WriteData(filehandle);

	return AF_SUCCEED;
}

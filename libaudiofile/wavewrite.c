/*
	Audio File Library
	Copyright (C) 1998-2000, Michael Pruett <michael@68k.org>
	Copyright (C) 2000-2001, Silicon Graphics, Inc.

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

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#include "audiofile.h"
#include "afinternal.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"
#include "wave.h"

status _af_wave_update (AFfilehandle file);

static status WriteFormat (AFfilehandle file);
static status WriteFrameCount (AFfilehandle file);
static status WriteMiscellaneous (AFfilehandle file);
static status WriteCues (AFfilehandle file);
static status WriteData (AFfilehandle file);

_WAVEInfo *waveinfo_new (void)
{
	_WAVEInfo	*waveinfo = _af_malloc(sizeof (_WAVEInfo));

	waveinfo->factOffset = 0;
	waveinfo->miscellaneousStartOffset = 0;
	waveinfo->totalMiscellaneousSize = 0;
	waveinfo->markOffset = 0;
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

	assert(file != NULL);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fwrite("fmt ", 4, 1, file->fh);

	switch (track->f.compressionType)
	{
		case AF_COMPRESSION_NONE:
			chunkSize = 16;
			if (track->f.sampleFormat == AF_SAMPFMT_FLOAT)
			{
				formatTag = WAVE_FORMAT_IEEE_FLOAT;
			}
			else if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP ||
				track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
			{
				formatTag = WAVE_FORMAT_PCM;
			}
			else
			{
				_af_error(AF_BAD_COMPTYPE, "bad sample format");
				return AF_FAIL;
			}

			blockAlign = _af_format_frame_size(&track->f, AF_FALSE);
			bitsPerSample = 8 * _af_format_sample_size(&track->f, AF_FALSE);
			break;

		/*
			G.711 compression uses eight bits per sample.
		*/
		case AF_COMPRESSION_G711_ULAW:
			chunkSize = 18;
			formatTag = IBM_FORMAT_MULAW;
			blockAlign = track->f.channelCount;
			bitsPerSample = 8;
			break;

		case AF_COMPRESSION_G711_ALAW:
			chunkSize = 18;
			formatTag = IBM_FORMAT_ALAW;
			blockAlign = track->f.channelCount;
			bitsPerSample = 8;
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

	if (track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		track->f.compressionType == AF_COMPRESSION_G711_ALAW)
	{
		u_int16_t	zero = 0;
		af_fwrite(&zero, 2, 1, file->fh);
	}

	return AF_SUCCEED;
}

static status WriteFrameCount (AFfilehandle file)
{
	_Track		*track = NULL;
	_WAVEInfo	*waveinfo = NULL;
	u_int32_t	factSize = 4;
	u_int32_t	totalFrameCount;

	assert(file != NULL);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);
	waveinfo = (_WAVEInfo *) file->formatSpecific;

	/* We only write the fact chunk for compressed audio. */
	if (track->f.compressionType == AF_COMPRESSION_NONE)
		return AF_SUCCEED;

	/*
		If the offset for the fact chunk hasn't been set yet,
		set it to the file's current position.
	*/
	if (waveinfo->factOffset == 0)
		waveinfo->factOffset = af_ftell(file->fh);
	else
		af_fseek(file->fh, waveinfo->factOffset, SEEK_SET);

	af_fwrite("fact", 4, 1, file->fh);
	factSize = HOST_TO_LENDIAN_INT32(factSize);
	af_fwrite(&factSize, 4, 1, file->fh);

	totalFrameCount = HOST_TO_LENDIAN_INT32(track->totalfframes);
	af_fwrite(&totalFrameCount, 4, 1, file->fh);

	return AF_SUCCEED;
}

static status WriteData (AFfilehandle file)
{
	_Track		*track;
	u_int32_t	chunkSize;
	_WAVEInfo	*waveinfo;

	assert(file);

	waveinfo = file->formatSpecific;
	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fwrite("data", 4, 1, file->fh);
	waveinfo->dataSizeOffset = af_ftell(file->fh);

	chunkSize = _af_format_frame_size(&track->f, AF_FALSE) *
		track->totalfframes;

	chunkSize = HOST_TO_LENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);
	track->fpos_first_frame = af_ftell(file->fh);

	return AF_SUCCEED;
}

status _af_wave_update (AFfilehandle file)
{
	_Track		*track;
	_WAVEInfo	*wave = (_WAVEInfo *) file->formatSpecific;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	if (track->fpos_first_frame != 0)
	{
		u_int32_t	dataLength, fileLength;

		/* Update the frame count chunk if present. */
		WriteFrameCount(file);

		/* Update the length of the data chunk. */
		af_fseek(file->fh, wave->dataSizeOffset, SEEK_SET);

		/*
			We call _af_format_frame_size to calculate the
			frame size of normal PCM data or compressed data.
		*/
		dataLength = (u_int32_t) track->totalfframes *
			_af_format_frame_size(&track->f, AF_FALSE);
		dataLength = HOST_TO_LENDIAN_INT32(dataLength);
		af_fwrite(&dataLength, 4, 1, file->fh);

		/* Update the length of the RIFF chunk. */
		fileLength = (u_int32_t) af_flength(file->fh);
		fileLength -= 8;
		fileLength = HOST_TO_LENDIAN_INT32(fileLength);

		af_fseek(file->fh, 4, SEEK_SET);
		af_fwrite(&fileLength, 4, 1, file->fh);
	}

	/*
		Write the actual data that was set after initializing
		the miscellaneous IDs.	The size of the data will be
		unchanged.
	*/
	WriteMiscellaneous(file);

	/* Write the new positions; the size of the data will be unchanged. */
	WriteCues(file);

	return AF_SUCCEED;
}

/* Convert an Audio File Library miscellaneous type to a WAVE type. */
static status misc_type_to_wave (int misctype, u_int32_t *miscid)
{
	if (misctype == AF_MISC_AUTH)
		memcpy(miscid, "IART", 4);
	else if (misctype == AF_MISC_NAME)
		memcpy(miscid, "INAM", 4);
	else if (misctype == AF_MISC_COPY)
		memcpy(miscid, "ICOP", 4);
	else if (misctype == AF_MISC_ICMT)
		memcpy(miscid, "ICMT", 4);
	else if (misctype == AF_MISC_ICRD)
		memcpy(miscid, "ICRD", 4);
	else if (misctype == AF_MISC_ISFT)
		memcpy(miscid, "ISFT", 4);
	else
		return AF_FAIL;

	return AF_SUCCEED;
}

status WriteMiscellaneous (AFfilehandle filehandle)
{
	_WAVEInfo	*wave = (_WAVEInfo *) filehandle->formatSpecific;

	if (filehandle->miscellaneousCount != 0)
	{
		int		i;
		u_int32_t	miscellaneousBytes;
		u_int32_t 	chunkSize;

		/* Start at 12 to account for 'LIST', size, and 'INFO'. */
		miscellaneousBytes = 12;

		/* Then calculate the size of the whole INFO chunk. */
		for (i=0; i<filehandle->miscellaneousCount; i++)
		{
			u_int32_t	miscid;

			/* Skip miscellaneous data of an unsupported type. */
			if (misc_type_to_wave(filehandle->miscellaneous[i].type,
				&miscid) == AF_FAIL)
				continue;

			/* Account for miscellaneous type and size. */
			miscellaneousBytes += 8;
			miscellaneousBytes += filehandle->miscellaneous[i].size;

			/* Add a pad byte if necessary. */
			if (filehandle->miscellaneous[i].size % 2 != 0)
				miscellaneousBytes++;

			assert(miscellaneousBytes % 2 == 0);
		}

		if (wave->miscellaneousStartOffset == 0)
			wave->miscellaneousStartOffset = af_ftell(filehandle->fh);
		else
			af_fseek(filehandle->fh, wave->miscellaneousStartOffset, SEEK_SET);

		wave->totalMiscellaneousSize = miscellaneousBytes;

		/*
			Write the data.  On the first call to this
			function (from _af_wave_write_init), the
			data won't be available, af_fseek is used to
			reserve space until the data has been provided.
			On subseuent calls to this function (from
			_af_wave_update), the data will really be written.
		*/

		/* Write 'LIST'. */
		af_fwrite("LIST", 4, 1, filehandle->fh);

		/* Write the size of the following chunk. */
		chunkSize = miscellaneousBytes-8;
		chunkSize = HOST_TO_LENDIAN_INT32(chunkSize);
		af_fwrite(&chunkSize, sizeof (u_int32_t), 1, filehandle->fh);

		/* Write 'INFO'. */
		af_fwrite("INFO", 4, 1, filehandle->fh);

		/* Write each miscellaneous chunk. */
		for (i=0; i<filehandle->miscellaneousCount; i++)
		{
			u_int32_t	miscsize = HOST_TO_LENDIAN_INT32(filehandle->miscellaneous[i].size);
			u_int32_t 	miscid = 0;

			/* Skip miscellaneous data of an unsupported type. */
			if (misc_type_to_wave(filehandle->miscellaneous[i].type,
				&miscid) == AF_FAIL)
				continue;

			af_fwrite(&miscid, 4, 1, filehandle->fh);
			af_fwrite(&miscsize, 4, 1, filehandle->fh);
			if (filehandle->miscellaneous[i].buffer != NULL)
			{
				u_int8_t	zero = 0;

				af_fwrite(filehandle->miscellaneous[i].buffer, filehandle->miscellaneous[i].size, 1, filehandle->fh);

				/* Pad if necessary. */
				if ((filehandle->miscellaneous[i].size%2) != 0)
					af_fwrite(&zero, 1, 1, filehandle->fh);
			}
			else
			{
				int	size;
				size = filehandle->miscellaneous[i].size;

				/* Pad if necessary. */
				if ((size % 2) != 0)
					size++;
				af_fseek(filehandle->fh, size, SEEK_CUR);
			}
		}
	}

	return AF_SUCCEED;
}

static status WriteCues (AFfilehandle file)
{
	int		i, *markids, markCount;
	u_int32_t	numCues, cueChunkSize, listChunkSize;
	_Track		*track = &file->tracks[0];
	_WAVEInfo	*wave;

	assert(file);

	markCount = afGetMarkIDs(file, AF_DEFAULT_TRACK, NULL);
	if (markCount == 0)
		return AF_SUCCEED;

	wave = file->formatSpecific;

	if (wave->markOffset == 0)
		wave->markOffset = af_ftell(file->fh);
	else
		af_fseek(file->fh, wave->markOffset, SEEK_SET);

	af_fwrite("cue ", 4, 1, file->fh);

	/*
		The cue chunk consists of 4 bytes for the number of cue points
		followed by 24 bytes for each cue point record.
	*/
	cueChunkSize = 4 + markCount * 24;
	cueChunkSize = HOST_TO_LENDIAN_INT32(cueChunkSize);
	af_fwrite(&cueChunkSize, sizeof (u_int32_t), 1, file->fh);
	numCues = HOST_TO_LENDIAN_INT32(markCount);
	af_fwrite(&numCues, sizeof (u_int32_t), 1, file->fh);

	markids = _af_calloc(markCount, sizeof (int));
	assert(markids != NULL);
	afGetMarkIDs(file, AF_DEFAULT_TRACK, markids);

	/* Write each marker to the file. */
	for (i=0; i < markCount; i++)
	{
		u_int32_t	identifier, position, chunkStart, blockStart;
		u_int32_t	sampleOffset;
		AFframecount	markposition;

		identifier = HOST_TO_LENDIAN_INT32(markids[i]);
		af_fwrite(&identifier, sizeof (u_int32_t), 1, file->fh);

		position = HOST_TO_LENDIAN_INT32(i);
		af_fwrite(&position, sizeof (u_int32_t), 1, file->fh);

		/* For now the RIFF id is always the first data chunk. */
		af_fwrite("data", 4, 1, file->fh);

		/*
			For an uncompressed WAVE file which contains
			only one data chunk, chunkStart and blockStart
			are zero.
		*/
		chunkStart = 0;
		af_fwrite(&chunkStart, sizeof (u_int32_t), 1, file->fh);

		blockStart = 0;
		af_fwrite(&blockStart, sizeof (u_int32_t), 1, file->fh);

		markposition = afGetMarkPosition(file, AF_DEFAULT_TRACK, markids[i]);

		/* Sample offsets are stored in the WAVE file as frames. */
		sampleOffset = HOST_TO_LENDIAN_INT32(markposition);
		af_fwrite(&sampleOffset, sizeof (u_int32_t), 1, file->fh);
	}

	/*
		Now write the cue names which is in a master list chunk
		with a subchunk for each cue's name.
	*/

	listChunkSize = 4;
	for (i=0; i<markCount; i++)
	{
		const char *name;

		name = afGetMarkName(file, AF_DEFAULT_TRACK, markids[i]);

		/*
			Each label chunk consists of 4 bytes for the
			"labl" chunk ID, 4 bytes for the chunk data
			size, 4 bytes for the cue point ID, and then
			the length of the label as a Pascal-style string.

			In all, this is 12 bytes plus the length of the
			string, its size byte, and a trailing pad byte
			if the length of the chunk is otherwise odd.
		*/
		listChunkSize += 12 + (strlen(name) + 1) +
			((strlen(name) + 1) % 2);
	}

	af_fwrite("LIST", 4, 1, file->fh);
	listChunkSize = HOST_TO_LENDIAN_INT32(listChunkSize);
	af_fwrite(&listChunkSize, sizeof (u_int32_t), 1, file->fh);
	af_fwrite("adtl", 4, 1, file->fh);

	for (i=0; i<markCount; i++)
	{
		const char	*name;
		u_int32_t	labelSize, cuePointID;

		name = afGetMarkName(file, AF_DEFAULT_TRACK, markids[i]);

		/* Make labelSize even if it is not already. */
		labelSize = 4+(strlen(name)+1) + ((strlen(name) + 1) % 2);
		labelSize = HOST_TO_LENDIAN_INT32(labelSize);
		cuePointID = HOST_TO_LENDIAN_INT32(markids[i]);

		af_fwrite("labl", 4, 1, file->fh);
		af_fwrite(&labelSize, 4, 1, file->fh);
		af_fwrite(&cuePointID, 4, 1, file->fh);
		af_fwrite(name, strlen(name) + 1, 1, file->fh);
		/*
			If the name plus the size byte comprises an odd
			length, add another byte to make the string an
			even length.
		*/
		if (((strlen(name) + 1) % 2) != 0)
		{
			u_int8_t	c=0;
			af_fwrite(&c, 1, 1, file->fh);
		}
	}

	free(markids);

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
	WriteCues(filehandle);
	WriteFormat(filehandle);
	WriteFrameCount(filehandle);
	WriteData(filehandle);

	return AF_SUCCEED;
}

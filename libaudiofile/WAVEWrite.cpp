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
	WAVEWrite.cpp

	This file contains routines which facilitate writing to WAVE files.
*/

#include "config.h"
#include "WAVE.h"

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

status WAVEFile::writeFormat()
{
	Track		*track = NULL;

	uint16_t	formatTag, channelCount;
	uint32_t	sampleRate, averageBytesPerSecond;
	uint16_t	blockAlign;
	uint32_t	chunkSize;
	uint16_t	bitsPerSample;

	track = _af_filehandle_get_track(this, AF_DEFAULT_TRACK);

	af_write("fmt ", 4, fh);

	switch (track->f.compressionType)
	{
		case AF_COMPRESSION_NONE:
			chunkSize = 16;
			if (track->f.sampleFormat == AF_SAMPFMT_FLOAT ||
				track->f.sampleFormat == AF_SAMPFMT_DOUBLE)
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

			blockAlign = _af_format_frame_size(&track->f, false);
			bitsPerSample = 8 * _af_format_sample_size(&track->f, false);
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

	af_write_uint32_le(&chunkSize, fh);
	af_write_uint16_le(&formatTag, fh);

	channelCount = track->f.channelCount;
	af_write_uint16_le(&channelCount, fh);

	sampleRate = track->f.sampleRate;
	af_write_uint32_le(&sampleRate, fh);

	averageBytesPerSecond =
		track->f.sampleRate * _af_format_frame_size(&track->f, false);
	af_write_uint32_le(&averageBytesPerSecond, fh);

	blockAlign = _af_format_frame_size(&track->f, false);
	af_write_uint16_le(&blockAlign, fh);

	af_write_uint16_le(&bitsPerSample, fh);

	if (track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		track->f.compressionType == AF_COMPRESSION_G711_ALAW)
	{
		uint16_t	zero = 0;
		af_write_uint16_le(&zero, fh);
	}

	return AF_SUCCEED;
}

status WAVEFile::writeFrameCount()
{
	uint32_t	factSize = 4;
	uint32_t	totalFrameCount;

	Track *track = _af_filehandle_get_track(this, AF_DEFAULT_TRACK);

	/* Omit the fact chunk only for uncompressed integer audio formats. */
	if (track->f.compressionType == AF_COMPRESSION_NONE &&
		(track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP ||
		track->f.sampleFormat == AF_SAMPFMT_UNSIGNED))
		return AF_SUCCEED;

	/*
		If the offset for the fact chunk hasn't been set yet,
		set it to the file's current position.
	*/
	if (factOffset == 0)
		factOffset = af_ftell(fh);
	else
		af_fseek(fh, factOffset, SEEK_SET);

	af_write("fact", 4, fh);
	af_write_uint32_le(&factSize, fh);

	totalFrameCount = track->totalfframes;
	af_write_uint32_le(&totalFrameCount, fh);

	return AF_SUCCEED;
}

status WAVEFile::writeData()
{
	Track		*track;
	uint32_t	chunkSize;

	track = _af_filehandle_get_track(this, AF_DEFAULT_TRACK);

	af_write("data", 4, fh);
	dataSizeOffset = af_ftell(fh);

	chunkSize = (int) _af_format_frame_size(&track->f, false) *
		track->totalfframes;

	af_write_uint32_le(&chunkSize, fh);
	track->fpos_first_frame = af_ftell(fh);

	return AF_SUCCEED;
}

status WAVEFile::update()
{
	Track *track = _af_filehandle_get_track(this, AF_DEFAULT_TRACK);

	if (track->fpos_first_frame != 0)
	{
		uint32_t	dataLength, fileLength;

		/* Update the frame count chunk if present. */
		writeFrameCount();

		/* Update the length of the data chunk. */
		af_fseek(fh, dataSizeOffset, SEEK_SET);

		/*
			We call _af_format_frame_size to calculate the
			frame size of normal PCM data or compressed data.
		*/
		dataLength = (uint32_t) track->totalfframes *
			(int) _af_format_frame_size(&track->f, false);
		af_write_uint32_le(&dataLength, fh);

		/* Update the length of the RIFF chunk. */
		fileLength = (uint32_t) af_flength(fh);
		fileLength -= 8;

		af_fseek(fh, 4, SEEK_SET);
		af_write_uint32_le(&fileLength, fh);
	}

	/*
		Write the actual data that was set after initializing
		the miscellaneous IDs.	The size of the data will be
		unchanged.
	*/
	writeMiscellaneous();

	/* Write the new positions; the size of the data will be unchanged. */
	writeCues();

	return AF_SUCCEED;
}

/* Convert an Audio File Library miscellaneous type to a WAVE type. */
static status misc_type_to_wave (int misctype, uint32_t *miscid)
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

status WAVEFile::writeMiscellaneous()
{
	if (miscellaneousCount != 0)
	{
		int		i;
		uint32_t	miscellaneousBytes;
		uint32_t 	chunkSize;

		/* Start at 12 to account for 'LIST', size, and 'INFO'. */
		miscellaneousBytes = 12;

		/* Then calculate the size of the whole INFO chunk. */
		for (i=0; i<miscellaneousCount; i++)
		{
			uint32_t	miscid;

			/* Skip miscellaneous data of an unsupported type. */
			if (misc_type_to_wave(miscellaneous[i].type,
				&miscid) == AF_FAIL)
				continue;

			/* Account for miscellaneous type and size. */
			miscellaneousBytes += 8;
			miscellaneousBytes += miscellaneous[i].size;

			/* Add a pad byte if necessary. */
			if (miscellaneous[i].size % 2 != 0)
				miscellaneousBytes++;

			assert(miscellaneousBytes % 2 == 0);
		}

		if (miscellaneousStartOffset == 0)
			miscellaneousStartOffset = af_ftell(fh);
		else
			af_fseek(fh, miscellaneousStartOffset, SEEK_SET);

		totalMiscellaneousSize = miscellaneousBytes;

		/*
			Write the data.  On the first call to this
			function (from _af_wave_write_init), the
			data won't be available, af_fseek is used to
			reserve space until the data has been provided.
			On subseuent calls to this function (from
			_af_wave_update), the data will really be written.
		*/

		/* Write 'LIST'. */
		af_write("LIST", 4, fh);

		/* Write the size of the following chunk. */
		chunkSize = miscellaneousBytes-8;
		af_write_uint32_le(&chunkSize, fh);

		/* Write 'INFO'. */
		af_write("INFO", 4, fh);

		/* Write each miscellaneous chunk. */
		for (i=0; i<miscellaneousCount; i++)
		{
			uint32_t	miscsize = miscellaneous[i].size;
			uint32_t 	miscid = 0;

			/* Skip miscellaneous data of an unsupported type. */
			if (misc_type_to_wave(miscellaneous[i].type,
				&miscid) == AF_FAIL)
				continue;

			af_write(&miscid, 4, fh);
			af_write_uint32_le(&miscsize, fh);
			if (miscellaneous[i].buffer != NULL)
			{
				uint8_t	zero = 0;

				af_write(miscellaneous[i].buffer, miscellaneous[i].size, fh);

				/* Pad if necessary. */
				if ((miscellaneous[i].size%2) != 0)
					af_write_uint8(&zero, fh);
			}
			else
			{
				int	size;
				size = miscellaneous[i].size;

				/* Pad if necessary. */
				if ((size % 2) != 0)
					size++;
				af_fseek(fh, size, SEEK_CUR);
			}
		}
	}

	return AF_SUCCEED;
}

status WAVEFile::writeCues()
{
	int i, *markids, markCount;
	uint32_t numCues, cueChunkSize, listChunkSize;

	markCount = afGetMarkIDs(this, AF_DEFAULT_TRACK, NULL);
	if (markCount == 0)
		return AF_SUCCEED;

	if (markOffset == 0)
		markOffset = af_ftell(fh);
	else
		af_fseek(fh, markOffset, SEEK_SET);

	af_write("cue ", 4, fh);

	/*
		The cue chunk consists of 4 bytes for the number of cue points
		followed by 24 bytes for each cue point record.
	*/
	cueChunkSize = 4 + markCount * 24;
	af_write_uint32_le(&cueChunkSize, fh);
	numCues = markCount;
	af_write_uint32_le(&numCues, fh);

	markids = (int *) _af_calloc(markCount, sizeof (int));
	assert(markids != NULL);
	afGetMarkIDs(this, AF_DEFAULT_TRACK, markids);

	/* Write each marker to the file. */
	for (i=0; i < markCount; i++)
	{
		uint32_t	identifier, position, chunkStart, blockStart;
		uint32_t	sampleOffset;
		AFframecount	markposition;

		identifier = markids[i];
		af_write_uint32_le(&identifier, fh);

		position = i;
		af_write_uint32_le(&position, fh);

		/* For now the RIFF id is always the first data chunk. */
		af_write("data", 4, fh);

		/*
			For an uncompressed WAVE file which contains
			only one data chunk, chunkStart and blockStart
			are zero.
		*/
		chunkStart = 0;
		af_write(&chunkStart, sizeof (uint32_t), fh);

		blockStart = 0;
		af_write(&blockStart, sizeof (uint32_t), fh);

		markposition = afGetMarkPosition(this, AF_DEFAULT_TRACK, markids[i]);

		/* Sample offsets are stored in the WAVE file as frames. */
		sampleOffset = markposition;
		af_write_uint32_le(&sampleOffset, fh);
	}

	/*
		Now write the cue names which is in a master list chunk
		with a subchunk for each cue's name.
	*/

	listChunkSize = 4;
	for (i=0; i<markCount; i++)
	{
		const char *name;

		name = afGetMarkName(this, AF_DEFAULT_TRACK, markids[i]);

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

	af_write("LIST", 4, fh);
	af_write_uint32_le(&listChunkSize, fh);
	af_write("adtl", 4, fh);

	for (i=0; i<markCount; i++)
	{
		const char	*name;
		uint32_t	labelSize, cuePointID;

		name = afGetMarkName(this, AF_DEFAULT_TRACK, markids[i]);

		/* Make labelSize even if it is not already. */
		labelSize = 4+(strlen(name)+1) + ((strlen(name) + 1) % 2);
		cuePointID = markids[i];

		af_write("labl", 4, fh);
		af_write_uint32_le(&labelSize, fh);
		af_write_uint32_le(&cuePointID, fh);
		af_write(name, strlen(name) + 1, fh);
		/*
			If the name plus the size byte comprises an odd
			length, add another byte to make the string an
			even length.
		*/
		if (((strlen(name) + 1) % 2) != 0)
		{
			uint8_t	zero=0;
			af_write_uint8(&zero, fh);
		}
	}

	free(markids);

	return AF_SUCCEED;
}

status WAVEFile::writeInit(AFfilesetup setup)
{
	uint32_t	zero = 0;

	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	af_fseek(fh, 0, SEEK_SET);
	af_write("RIFF", 4, fh);
	af_write(&zero, 4, fh);
	af_write("WAVE", 4, fh);

	writeMiscellaneous();
	writeCues();
	writeFormat();
	writeFrameCount();
	writeData();

	return AF_SUCCEED;
}

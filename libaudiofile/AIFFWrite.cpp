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
	AIFFWrite.cpp

	This file contains routines for writing AIFF and AIFF-C format
	sound files.
*/

#include "config.h"
#include "AIFF.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "byteorder.h"
#include "extended.h"
#include "util.h"

status AIFFFile::writeInit(AFfilesetup setup)
{
	uint32_t fileSize = 0;

	assert(fileFormat == AF_FILE_AIFF ||
		fileFormat == AF_FILE_AIFFC);

	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	af_write("FORM", 4, fh);
	writeU32(&fileSize);

	if (fileFormat == AF_FILE_AIFF)
		af_write("AIFF", 4, fh);
	else if (fileFormat == AF_FILE_AIFFC)
		af_write("AIFC", 4, fh);

	if (fileFormat == AF_FILE_AIFFC)
		writeFVER();

	writeCOMM();
	writeMARK();
	writeINST();
	writeAESD();
	writeMiscellaneous();
	writeSSND();

	return AF_SUCCEED;
}

status AIFFFile::update()
{
#ifdef DEBUG
	printf("_af_aiff_update called.\n");
#endif

	/* Get the length of the file. */
	uint32_t length = af_flength(fh);
	length -= 8;

	/* Set the length of the FORM chunk. */
	af_fseek(fh, 4, SEEK_SET);
	writeU32(&length);

	if (fileFormat == AF_FILE_AIFFC)
		writeFVER();

	writeCOMM();
	writeMARK();
	writeINST();
	writeAESD();
	writeMiscellaneous();
	writeSSND();

	return AF_SUCCEED;
}

status AIFFFile::writeCOMM()
{
	bool isAIFFC = fileFormat == AF_FILE_AIFFC;

	/*
		If COMM_offset hasn't been set yet, set it to the
		current offset.
	*/
	if (COMM_offset == 0)
		COMM_offset = af_ftell(fh);
	else
		af_fseek(fh, COMM_offset, SEEK_SET);

	Track *track = getTrack();

	uint8_t compressionTag[4];
	/* Pascal strings can occupy only 255 bytes (+ a size byte). */
	char compressionName[256];

	if (isAIFFC)
	{
		if (track->f.compressionType == AF_COMPRESSION_NONE)
		{
			if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP)
			{
				memcpy(compressionTag, "NONE", 4);
				strcpy(compressionName, "not compressed");
			}
			else if (track->f.sampleFormat == AF_SAMPFMT_FLOAT)
			{
				memcpy(compressionTag, "fl32", 4);
				strcpy(compressionName, "32-bit Floating Point");
			}
			else if (track->f.sampleFormat == AF_SAMPFMT_DOUBLE)
			{
				memcpy(compressionTag, "fl64", 4);
				strcpy(compressionName, "64-bit Floating Point");
			}
			/*
				We disallow unsigned sample data for
				AIFF files in _af_aiff_complete_setup,
				so the next condition should never be
				satisfied.
			*/
			else if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
			{
				_af_error(AF_BAD_SAMPFMT,
					"AIFF/AIFF-C format does not support unsigned data");
				assert(0);
				return AF_FAIL;
			}
		}
		else if (track->f.compressionType == AF_COMPRESSION_G711_ULAW)
		{
			memcpy(compressionTag, "ulaw", 4);
			strcpy(compressionName, "CCITT G.711 u-law");
		}
		else if (track->f.compressionType == AF_COMPRESSION_G711_ALAW)
		{
			memcpy(compressionTag, "alaw", 4);
			strcpy(compressionName, "CCITT G.711 A-law");
		}
	}

	af_write("COMM", 4, fh);

	/*
		For AIFF-C files, the length of the COMM chunk is 22
		plus the length of the compression name plus the size
		byte.  If the length of the data is an odd number of
		bytes, add a zero pad byte at the end, but don't
		include the pad byte in the chunk's size.
	*/
	uint32_t chunkSize;
	if (isAIFFC)
		chunkSize = 22 + strlen(compressionName) + 1;
	else
		chunkSize = 18;
	writeU32(&chunkSize);

	/* number of channels, 2 bytes */
	uint16_t channelCount = track->f.channelCount;
	writeU16(&channelCount);

	/* number of sample frames, 4 bytes */
	uint32_t frameCount = track->totalfframes;
	writeU32(&frameCount);

	/* sample size, 2 bytes */
	uint16_t sampleSize = track->f.sampleWidth;
	writeU16(&sampleSize);

	/* sample rate, 10 bytes */
	uint8_t sampleRate[10];
	_af_convert_to_ieee_extended(track->f.sampleRate, sampleRate);
	af_write(sampleRate, 10, fh);

	if (isAIFFC)
	{
		af_write(compressionTag, 4, fh);
		af_write_pstring(compressionName, fh);
	}

	return AF_SUCCEED;
}

/*
	The AESD chunk contains information pertinent to audio recording
	devices.
*/
status AIFFFile::writeAESD()
{
	Track *track = getTrack();

	if (!track->hasAESData)
		return AF_SUCCEED;

	if (AESD_offset == 0)
		AESD_offset = af_ftell(fh);
	else
		af_fseek(fh, AESD_offset, SEEK_SET);

	if (af_write("AESD", 4, fh) < 4)
		return AF_FAIL;

	uint32_t size = 24;
	if (!writeU32(&size))
		return AF_FAIL;

	if (af_write(track->aesData, 24, fh) < 24)
		return AF_FAIL;

	return AF_SUCCEED;
}

status AIFFFile::writeSSND()
{
	assert(fh);

	Track *track = getTrack();

	if (SSND_offset == 0)
		SSND_offset = af_ftell(fh);
	else
		af_fseek(fh, SSND_offset, SEEK_SET);

	uint32_t chunkSize = (int) _af_format_frame_size(&track->f, false) *
		track->totalfframes + 8;

	af_write("SSND", 4, fh);
	writeU32(&chunkSize);

	uint32_t zero = 0;
	/* data offset */
	writeU32(&zero);
	/* block size */
	writeU32(&zero);

	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = af_ftell(fh);

	return AF_SUCCEED;
}

status AIFFFile::writeINST()
{
	uint32_t length = 20;

	struct _INST instrumentdata;

	instrumentdata.sustainLoopPlayMode =
		afGetLoopMode(this, AF_DEFAULT_INST, 1);
	instrumentdata.sustainLoopBegin =
		afGetLoopStart(this, AF_DEFAULT_INST, 1);
	instrumentdata.sustainLoopEnd =
		afGetLoopEnd(this, AF_DEFAULT_INST, 1);

	instrumentdata.releaseLoopPlayMode =
		afGetLoopMode(this, AF_DEFAULT_INST, 2);
	instrumentdata.releaseLoopBegin =
		afGetLoopStart(this, AF_DEFAULT_INST, 2);
	instrumentdata.releaseLoopEnd =
		afGetLoopEnd(this, AF_DEFAULT_INST, 2);

	af_write("INST", 4, fh);
	writeU32(&length);

	instrumentdata.baseNote =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_MIDI_BASENOTE);
	writeU8(&instrumentdata.baseNote);
	instrumentdata.detune =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_NUMCENTS_DETUNE);
	writeS8(&instrumentdata.detune);
	instrumentdata.lowNote =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_MIDI_LONOTE);
	writeU8(&instrumentdata.lowNote);
	instrumentdata.highNote =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_MIDI_HINOTE);
	writeU8(&instrumentdata.highNote);
	instrumentdata.lowVelocity =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_MIDI_LOVELOCITY);
	writeU8(&instrumentdata.lowVelocity);
	instrumentdata.highVelocity =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_MIDI_HIVELOCITY);
	writeU8(&instrumentdata.highVelocity);

	instrumentdata.gain =
		afGetInstParamLong(this, AF_DEFAULT_INST, AF_INST_NUMDBS_GAIN);
	writeS16(&instrumentdata.gain);

	writeU16(&instrumentdata.sustainLoopPlayMode);
	writeU16(&instrumentdata.sustainLoopBegin);
	writeU16(&instrumentdata.sustainLoopEnd);

	writeU16(&instrumentdata.releaseLoopPlayMode);
	writeU16(&instrumentdata.releaseLoopBegin);
	writeU16(&instrumentdata.releaseLoopEnd);

	return AF_SUCCEED;
}

status AIFFFile::writeMARK()
{
	uint16_t numMarkers = afGetMarkIDs(this, AF_DEFAULT_TRACK, NULL);
	if (numMarkers == 0)
		return AF_SUCCEED;

	if (MARK_offset == 0)
		MARK_offset = af_ftell(fh);
	else
		af_fseek(fh, MARK_offset, SEEK_SET);

	AFfileoffset chunkStartPosition, chunkEndPosition;
	uint32_t length = 0;

	af_write("MARK", 4, fh);
	writeU32(&length);

	chunkStartPosition = af_ftell(fh);

	int *markids = (int *) _af_calloc(numMarkers, sizeof (int));
	assert(markids);
	afGetMarkIDs(this, AF_DEFAULT_TRACK, markids);

	writeU16(&numMarkers);

	for (unsigned i=0; i<numMarkers; i++)
	{
		uint16_t id = markids[i];
		writeU16(&id);

		uint32_t position = afGetMarkPosition(this, AF_DEFAULT_TRACK, markids[i]);
		writeU32(&position);

		const char *name = afGetMarkName(this, AF_DEFAULT_TRACK, markids[i]);
		assert(name);

		/* Write the name as a Pascal-style string. */
		af_write_pstring(name, fh);
	}

	free(markids);

	chunkEndPosition = af_ftell(fh);
	length = chunkEndPosition - chunkStartPosition;

#ifdef DEBUG
	printf(" end: %d\n", chunkEndPosition);
	printf(" length: %d\n", length);
#endif

	af_fseek(fh, chunkStartPosition - 4, SEEK_SET);

	writeU32(&length);
	af_fseek(fh, chunkEndPosition, SEEK_SET);

	return AF_SUCCEED;
}

/*
	The FVER chunk, if present, is always the first chunk in the file.
*/
status AIFFFile::writeFVER()
{
	uint32_t chunkSize, timeStamp;

	assert(fileFormat == AF_FILE_AIFFC);

	if (FVER_offset == 0)
		FVER_offset = af_ftell(fh);
	else
		af_fseek(fh, FVER_offset, SEEK_SET);

	af_write("FVER", 4, fh);

	chunkSize = 4;
	writeU32(&chunkSize);

	timeStamp = AIFC_VERSION_1;
	writeU32(&timeStamp);

	return AF_SUCCEED;
}

/*
	WriteMiscellaneous writes all the miscellaneous data chunks in a
	file handle structure to an AIFF or AIFF-C file.
*/
status AIFFFile::writeMiscellaneous()
{
	if (miscellaneousPosition == 0)
		miscellaneousPosition = af_ftell(fh);
	else
		af_fseek(fh, miscellaneousPosition, SEEK_SET);

	for (int i=0; i<miscellaneousCount; i++)
	{
		Miscellaneous *misc = &miscellaneous[i];
		uint32_t chunkType, chunkSize;
		uint8_t padByte = 0;

#ifdef DEBUG
		printf("WriteMiscellaneous: %d, type %d\n", i, misc->type);
#endif

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
			case AF_MISC_MIDI:
				memcpy(&chunkType, "MIDI", 4); break;
			case AF_MISC_APPL:
				memcpy(&chunkType, "APPL", 4); break;
		}


		af_write(&chunkType, 4, fh);

		chunkSize = misc->size;
		writeU32(&chunkSize);
		/*
			Write the miscellaneous buffer and then a pad byte
			if necessary.  If the buffer is null, skip the space
			for now.
		*/
		if (misc->buffer != NULL)
			af_write(misc->buffer, misc->size, fh);
		else
			af_fseek(fh, misc->size, SEEK_CUR);

		if (misc->size % 2 != 0)
			writeU8(&padByte);
	}

	return AF_SUCCEED;
}

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
	aiffwrite.c

	This file contains routines for writing AIFF and AIFF-C format
	sound files.
*/

#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "extended.h"
#include "afinternal.h"
#include "audiofile.h"
#include "aiff.h"
#include "byteorder.h"
#include "util.h"
#include "setup.h"

status _af_aiff_update (AFfilehandle file);

static status WriteCOMM (AFfilehandle file);
static status WriteSSND (AFfilehandle file);
static status WriteMARK (AFfilehandle file);
static status WriteINST (AFfilehandle file);
static status WriteFVER (AFfilehandle file);
static status WriteAESD (AFfilehandle file);
static status WriteMiscellaneous (AFfilehandle file);

static _AIFFInfo *aiffinfo_new (void)
{
	_AIFFInfo	*aiff = _af_malloc(sizeof (_AIFFInfo));

	aiff->miscellaneousPosition = 0;
	aiff->FVER_offset = 0;
	aiff->COMM_offset = 0;
	aiff->MARK_offset = 0;
	aiff->INST_offset = 0;
	aiff->AESD_offset = 0;
	aiff->SSND_offset = 0;

	return aiff;
}

status _af_aiff_write_init (AFfilesetup setup, AFfilehandle file)
{
	u_int32_t	fileSize = HOST_TO_BENDIAN_INT32(0);

	assert(file);
	assert(file->fileFormat == AF_FILE_AIFF ||
		file->fileFormat == AF_FILE_AIFFC);

	if (_af_filesetup_make_handle(setup, file) == AF_FAIL)
		return AF_FAIL;

	file->formatSpecific = aiffinfo_new();

	af_fwrite("FORM", 4, 1, file->fh);
	af_fwrite(&fileSize, 4, 1, file->fh);

	if (file->fileFormat == AF_FILE_AIFF)
		af_fwrite("AIFF", 4, 1, file->fh);
	else if (file->fileFormat == AF_FILE_AIFFC)
		af_fwrite("AIFC", 4, 1, file->fh);

	if (file->fileFormat == AF_FILE_AIFFC)
		WriteFVER(file);

	WriteCOMM(file);
	WriteMARK(file);
	WriteINST(file);
	WriteAESD(file);
	WriteMiscellaneous(file);
	WriteSSND(file);

	return AF_SUCCEED;
}

status _af_aiff_update (AFfilehandle file)
{
	_Track		*track;
	u_int32_t	length;

	assert(file);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

#ifdef DEBUG
	printf("_af_aiff_update called.\n");
#endif

	/* Get the length of the file. */
	length = af_flength(file->fh);
	length -= 8;
	length = HOST_TO_BENDIAN_INT32(length);

	/* Set the length of the FORM chunk. */
	af_fseek(file->fh, 4, SEEK_SET);
	af_fwrite(&length, 4, 1, file->fh);

	if (file->fileFormat == AF_FILE_AIFFC)
		WriteFVER(file);

	WriteCOMM(file);
	WriteMARK(file);
	WriteINST(file);
	WriteAESD(file);
	WriteMiscellaneous(file);
	WriteSSND(file);

	return AF_SUCCEED;
}

static status WriteCOMM (const AFfilehandle file)
{
	_Track		*track;
	u_int32_t	chunkSize;
	_AIFFInfo	*aiff;
	bool		isAIFFC;

	u_int16_t	sb;
	u_int32_t	lb;
	unsigned char	eb[10];

	u_int8_t	compressionTag[4];
	/* Pascal strings can occupy only 255 bytes (+ a size byte). */
	char		compressionName[256];

	isAIFFC = file->fileFormat == AF_FILE_AIFFC;

	aiff = file->formatSpecific;

	/*
		If COMM_offset hasn't been set yet, set it to the
		current offset.
	*/
	if (aiff->COMM_offset == 0)
		aiff->COMM_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, aiff->COMM_offset, SEEK_SET);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

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

	af_fwrite("COMM", 4, 1, file->fh);

	/*
		For AIFF-C files, the length of the COMM chunk is 22
		plus the length of the compression name plus the size
		byte.  If the length of the data is an odd number of
		bytes, add a zero pad byte at the end, but don't
		include the pad byte in the chunk's size.
	*/
	if (isAIFFC)
		chunkSize = 22 + strlen(compressionName) + 1;
	else
		chunkSize = 18;
	chunkSize = HOST_TO_BENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);

	/* number of channels, 2 bytes */
	sb = HOST_TO_BENDIAN_INT16(track->f.channelCount);
	af_fwrite(&sb, 2, 1, file->fh);

	/* number of sample frames, 4 bytes */
	lb = HOST_TO_BENDIAN_INT32(track->totalfframes);
	af_fwrite(&lb, 4, 1, file->fh);

	/* sample size, 2 bytes */
	sb = HOST_TO_BENDIAN_INT16(track->f.sampleWidth);
	af_fwrite(&sb, 2, 1, file->fh);

	/* sample rate, 10 bytes */
	ConvertToIeeeExtended(track->f.sampleRate, eb);
	af_fwrite(eb, 10, 1, file->fh);

	if (file->fileFormat == AF_FILE_AIFFC)
	{
		u_int8_t	sizeByte, zero = 0;

		af_fwrite(compressionTag, 4, 1, file->fh);

		sizeByte = strlen(compressionName);

		af_fwrite(&sizeByte, 1, 1, file->fh);
		af_fwrite(compressionName, sizeByte, 1, file->fh);

		/*
			If sizeByte is even, then 1+sizeByte
			(the length of the string) is odd.  Add an
			extra byte to make the chunk's extent even
			(even though the chunk's size may be odd).
		*/
		if ((sizeByte % 2) == 0)
			af_fwrite(&zero, 1, 1, file->fh);
	}

	return AF_SUCCEED;
}

/*
	The AESD chunk contains information pertinent to audio recording
	devices.
*/
static status WriteAESD (const AFfilehandle file)
{
	_Track		*track;
	u_int32_t	size = 24;
	_AIFFInfo	*aiff;

	assert(file);

	aiff = file->formatSpecific;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	if (track->hasAESData == AF_FALSE)
		return AF_SUCCEED;

	if (aiff->AESD_offset == 0)
		aiff->AESD_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, aiff->AESD_offset, SEEK_SET);

	if (af_fwrite("AESD", 4, 1, file->fh) < 1)
		return AF_FAIL;

	size = HOST_TO_BENDIAN_INT32(size);

	if (af_fwrite(&size, 4, 1, file->fh) < 1)
		return AF_FAIL;

	if (af_fwrite(track->aesData, 24, 1, file->fh) < 1)
		return AF_FAIL;

	return AF_SUCCEED;
}

static status WriteSSND (AFfilehandle file)
{
	_Track		*track;
	u_int32_t	chunkSize, zero = 0;
	_AIFFInfo	*aiff;

	assert(file);
	assert(file->fh);

	aiff = file->formatSpecific;

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	if (aiff->SSND_offset == 0)
		aiff->SSND_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, aiff->SSND_offset, SEEK_SET);

	chunkSize = _af_format_frame_size(&track->f, AF_FALSE) *
		track->totalfframes + 8;

	af_fwrite("SSND", 4, 1, file->fh);
	chunkSize = HOST_TO_BENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);

	/* data offset */
	af_fwrite(&zero, 4, 1, file->fh);
	/* block size */
	af_fwrite(&zero, 4, 1, file->fh);

	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = af_ftell(file->fh);

	return AF_SUCCEED;
}

static status WriteINST (AFfilehandle file)
{
	u_int32_t	length;
	struct _INST	instrumentdata;

	length = 20;
	length = HOST_TO_BENDIAN_INT32(length);

	instrumentdata.sustainLoopPlayMode =
		HOST_TO_BENDIAN_INT16(afGetLoopMode(file, AF_DEFAULT_INST, 1));
	instrumentdata.sustainLoopBegin =
		HOST_TO_BENDIAN_INT16(afGetLoopStart(file, AF_DEFAULT_INST, 1));
	instrumentdata.sustainLoopEnd =
		HOST_TO_BENDIAN_INT16(afGetLoopEnd(file, AF_DEFAULT_INST, 1));

	instrumentdata.releaseLoopPlayMode =
		HOST_TO_BENDIAN_INT16(afGetLoopMode(file, AF_DEFAULT_INST, 2));
	instrumentdata.releaseLoopBegin =
		HOST_TO_BENDIAN_INT16(afGetLoopStart(file, AF_DEFAULT_INST, 2));
	instrumentdata.releaseLoopEnd =
		HOST_TO_BENDIAN_INT16(afGetLoopEnd(file, AF_DEFAULT_INST, 2));

	af_fwrite("INST", 4, 1, file->fh);
	af_fwrite(&length, 4, 1, file->fh);

	instrumentdata.baseNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_BASENOTE);
	af_fwrite(&instrumentdata.baseNote, 1, 1, file->fh);
	instrumentdata.detune =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_NUMCENTS_DETUNE);
	af_fwrite(&instrumentdata.detune, 1, 1, file->fh);
	instrumentdata.lowNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_LONOTE);
	af_fwrite(&instrumentdata.lowNote, 1, 1, file->fh);
	instrumentdata.highNote =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_HINOTE);
	af_fwrite(&instrumentdata.highNote, 1, 1, file->fh);
	instrumentdata.lowVelocity =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_LOVELOCITY);
	af_fwrite(&instrumentdata.lowVelocity, 1, 1, file->fh);
	instrumentdata.highVelocity =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_MIDI_HIVELOCITY);
	af_fwrite(&instrumentdata.highVelocity, 1, 1, file->fh);

	instrumentdata.gain =
		afGetInstParamLong(file, AF_DEFAULT_INST, AF_INST_NUMDBS_GAIN);
	instrumentdata.gain = HOST_TO_BENDIAN_INT16(instrumentdata.gain);
	af_fwrite(&instrumentdata.gain, 2, 1, file->fh);

	af_fwrite(&instrumentdata.sustainLoopPlayMode, 2, 1, file->fh);
	af_fwrite(&instrumentdata.sustainLoopBegin, 2, 1, file->fh);
	af_fwrite(&instrumentdata.sustainLoopEnd, 2, 1, file->fh);

	af_fwrite(&instrumentdata.releaseLoopPlayMode, 2, 1, file->fh);
	af_fwrite(&instrumentdata.releaseLoopBegin, 2, 1, file->fh);
	af_fwrite(&instrumentdata.releaseLoopEnd, 2, 1, file->fh);

	return AF_SUCCEED;
}

static status WriteMARK (AFfilehandle file)
{
	off_t		chunkStartPosition, chunkEndPosition;
	u_int32_t	length = 0;
	u_int16_t	numMarkers, sb;
	int		i, *markids;
	_AIFFInfo	*aiff;

	assert(file);

	numMarkers = afGetMarkIDs(file, AF_DEFAULT_TRACK, NULL);
	if (numMarkers == 0)
		return AF_SUCCEED;

	aiff = file->formatSpecific;

	if (aiff->MARK_offset == 0)
		aiff->MARK_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, aiff->MARK_offset, SEEK_SET);

	af_fwrite("MARK", 4, 1, file->fh);
	af_fwrite(&length, 4, 1, file->fh);

	chunkStartPosition = af_ftell(file->fh);

	markids = _af_calloc(numMarkers, sizeof (int));
	assert(markids);
	afGetMarkIDs(file, AF_DEFAULT_TRACK, markids);

	sb = HOST_TO_BENDIAN_INT16(numMarkers);
	af_fwrite(&sb, 2, 1, file->fh);

	for (i=0; i<numMarkers; i++)
	{
		u_int8_t	namelength, zero = 0;
		u_int16_t	id;
		u_int32_t	position;
		char		*name;

		id = markids[i];
		position = afGetMarkPosition(file, AF_DEFAULT_TRACK, markids[i]);

		id = HOST_TO_BENDIAN_INT16(id);
		position = HOST_TO_BENDIAN_INT32(position);

		af_fwrite(&id, 2, 1, file->fh);
		af_fwrite(&position, 4, 1, file->fh);

		name = afGetMarkName(file, AF_DEFAULT_TRACK, markids[i]);
		assert(name);
		namelength = strlen(name);

		/* Write the name as a Pascal-style string. */
		af_fwrite(&namelength, 1, 1, file->fh);
		af_fwrite(name, 1, namelength, file->fh);

		/*
			We need a pad byte if the length of the
			Pascal-style string (including the size byte)
			is odd, i.e. if namelength + 1 % 2 == 1.
		*/
		if ((namelength % 2) == 0)
			af_fwrite(&zero, 1, 1, file->fh);
	}

	free(markids);

	chunkEndPosition = af_ftell(file->fh);
	length = chunkEndPosition - chunkStartPosition;

#ifdef DEBUG
	printf(" end: %d\n", chunkEndPosition);
	printf(" length: %d\n", length);
#endif

	af_fseek(file->fh, chunkStartPosition - 4, SEEK_SET);

	length = HOST_TO_BENDIAN_INT32(length);
	af_fwrite(&length, 4, 1, file->fh);
	af_fseek(file->fh, chunkEndPosition, SEEK_SET);

	return AF_SUCCEED;
}

/*
	The FVER chunk, if present, is always the first chunk in the file.
*/
static status WriteFVER (AFfilehandle file)
{
	u_int32_t	chunkSize, timeStamp;
	_AIFFInfo	*aiff;

	assert(file->fileFormat == AF_FILE_AIFFC);

	aiff = file->formatSpecific;

	if (aiff->FVER_offset == 0)
		aiff->FVER_offset = af_ftell(file->fh);
	else
		af_fseek(file->fh, aiff->FVER_offset, SEEK_SET);

	af_fwrite("FVER", 4, 1, file->fh);

	chunkSize = 4;
	chunkSize = HOST_TO_BENDIAN_INT32(chunkSize);
	af_fwrite(&chunkSize, 4, 1, file->fh);

	timeStamp = AIFCVersion1;
	timeStamp = HOST_TO_BENDIAN_INT32(timeStamp);
	af_fwrite(&timeStamp, 4, 1, file->fh);

	return AF_SUCCEED;
}

/*
	WriteMiscellaneous writes all the miscellaneous data chunks in a
	file handle structure to an AIFF or AIFF-C file.
*/
static status WriteMiscellaneous (AFfilehandle file)
{
	_AIFFInfo	*aiff;
	int		i;

	aiff = (_AIFFInfo *) file->formatSpecific;

	if (aiff->miscellaneousPosition == 0)
		aiff->miscellaneousPosition = af_ftell(file->fh);
	else
		af_fseek(file->fh, aiff->miscellaneousPosition, SEEK_SET);

	for (i=0; i<file->miscellaneousCount; i++)
	{
		_Miscellaneous	*misc = &file->miscellaneous[i];
		u_int32_t	chunkType, chunkSize;
		u_int8_t	padByte = 0;

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

		chunkSize = HOST_TO_BENDIAN_INT32(misc->size);

		af_fwrite(&chunkType, 4, 1, file->fh);
		af_fwrite(&chunkSize, 4, 1, file->fh);
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

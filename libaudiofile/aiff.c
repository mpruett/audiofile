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
	aiff.c

	This file contains routines for parsing AIFF and AIFF-C sound
	files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#include "extended.h"
#include "audiofile.h"
#include "util.h"
#include "afinternal.h"
#include "byteorder.h"
#include "aiff.h"
#include "setup.h"
#include "track.h"
#include "marker.h"

static status ParseFVER (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);
static status ParseAESD (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);
static status ParseMiscellaneous (AFfilehandle file, AFvirtualfile *fh,
	u_int32_t type, size_t size);
static status ParseINST (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);
static status ParseMARK (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);
static status ParseCOMM (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);
static status ParseSSND (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size);

_InstParamInfo _af_aiff_inst_params[_AF_AIFF_NUM_INSTPARAMS] =
{
	{ AF_INST_MIDI_BASENOTE, AU_PVTYPE_LONG, "MIDI base note", {60} },
	{ AF_INST_NUMCENTS_DETUNE, AU_PVTYPE_LONG, "Detune in cents", {0} },
	{ AF_INST_MIDI_LOVELOCITY, AU_PVTYPE_LONG, "Low velocity", {1} },
	{ AF_INST_MIDI_HIVELOCITY, AU_PVTYPE_LONG, "High velocity", {127} },
	{ AF_INST_MIDI_LONOTE, AU_PVTYPE_LONG, "Low note", {0} },
	{ AF_INST_MIDI_HINOTE, AU_PVTYPE_LONG, "High note", {127} },
	{ AF_INST_NUMDBS_GAIN, AU_PVTYPE_LONG, "Gain in dB", {0} },
	{ AF_INST_SUSLOOPID, AU_PVTYPE_LONG, "Sustain loop id", {0} },
	{ AF_INST_RELLOOPID, AU_PVTYPE_LONG, "Release loop id", {0} }
};

int _af_aiffc_compression_types[_AF_AIFF_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

_AFfilesetup _af_aiff_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_AIFF,		/* fileFormat */
	AF_TRUE,		/* trackSet */
	AF_TRUE,		/* instrumentSet */
	AF_TRUE,		/* miscellaneousSet */
	1,			/* trackCount */
	NULL,			/* tracks */
	1,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

/*
	FVER chunks are only present in AIFF-C files.
*/
static status ParseFVER (AFfilehandle file, AFvirtualfile *fh, u_int32_t type, size_t size)
{
	u_int32_t	timestamp;

	assert(!memcmp(&type, "FVER", 4));

	af_fread(&timestamp, sizeof (u_int32_t), 1, fh);
	timestamp = BENDIAN_TO_HOST_INT32(timestamp);
	/* timestamp holds the number of seconds since January 1, 1904. */

	return AF_SUCCEED;
}

/*
	Parse AES recording data.
*/
static status ParseAESD (AFfilehandle file, AFvirtualfile *fh, u_int32_t type, size_t size)
{
	_Track		*track;
	unsigned char	aesChannelStatusData[24];

	assert(!memcmp(&type, "AESD", 4));
	assert(size == 24);

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	track->hasAESData = AF_TRUE;

	/*
		Try to read 24 bytes of AES nonaudio data from the file.
		Fail if the file disappoints.
	*/
	if (af_fread(aesChannelStatusData, 1, 24, fh) != 24)
		return AF_FAIL;

	memcpy(track->aesData, aesChannelStatusData, 24);

	return AF_SUCCEED;
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
		!memcmp(&type, "(c) ", 4) || !memcmp(&type, "ANNO", 4) ||
		!memcmp(&type, "APPL", 4) || !memcmp(&type, "MIDI", 4));
	assert(size >= 0);

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
	else if (!memcmp(&type, "APPL", 4))
		misctype = AF_MISC_APPL;
	else if (!memcmp(&type, "MIDI", 4))
		misctype = AF_MISC_MIDI;

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
	Parse instrument chunks, which contain information about using
	sound data as a sampled instrument.
*/
static status ParseINST (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size)
{
	_Instrument	*instrument;
	u_int8_t	baseNote;
	int8_t		detune;
	u_int8_t	lowNote, highNote, lowVelocity, highVelocity;
	int16_t		gain;

	u_int16_t	sustainLoopPlayMode, sustainLoopBegin, sustainLoopEnd;
	u_int16_t	releaseLoopPlayMode, releaseLoopBegin, releaseLoopEnd;

	assert(!memcmp(&type, "INST", 4));

	instrument = _af_calloc(1, sizeof (_Instrument));
	instrument->id = AF_DEFAULT_INST;
	instrument->values = _af_calloc(_AF_AIFF_NUM_INSTPARAMS, sizeof (AFPVu));
	instrument->loopCount = 2;
	instrument->loops = _af_calloc(2, sizeof (_Loop));

	file->instrumentCount = 1;
	file->instruments = instrument;

	af_fread(&baseNote, 1, 1, fh);
	af_fread(&detune, 1, 1, fh);
	af_fread(&lowNote, 1, 1, fh);
	af_fread(&highNote, 1, 1, fh);
	af_fread(&lowVelocity, 1, 1, fh);
	af_fread(&highVelocity, 1, 1, fh);
	af_fread(&gain, 2, 1, fh);
	gain = BENDIAN_TO_HOST_INT16(gain);

#ifdef DEBUG
	printf("baseNote/detune/lowNote/highNote/lowVelocity/highVelocity/gain:"
		" %d %d %d %d %d %d %d\n",
		baseNote, detune, lowNote, highNote, lowVelocity, highVelocity,
		gain);
#endif

	instrument->values[0].l = baseNote;
	instrument->values[1].l = detune;
	instrument->values[2].l = lowVelocity;
	instrument->values[3].l = highVelocity;
	instrument->values[4].l = lowNote;
	instrument->values[5].l = highNote;
	instrument->values[6].l = gain;

	instrument->values[7].l = 1;	/* sustain loop id */
	instrument->values[8].l = 2;	/* release loop id */

	af_fread(&sustainLoopPlayMode, sizeof (u_int16_t), 1, fh);
	sustainLoopPlayMode = BENDIAN_TO_HOST_INT16(sustainLoopPlayMode);
	af_fread(&sustainLoopBegin, sizeof (u_int16_t), 1, fh);
	sustainLoopBegin = BENDIAN_TO_HOST_INT16(sustainLoopBegin);
	af_fread(&sustainLoopEnd, sizeof (u_int16_t), 1, fh);
	sustainLoopEnd = BENDIAN_TO_HOST_INT16(sustainLoopEnd);

	af_fread(&releaseLoopPlayMode, sizeof (u_int16_t), 1, fh);
	releaseLoopPlayMode = BENDIAN_TO_HOST_INT16(releaseLoopPlayMode);
	af_fread(&releaseLoopBegin, sizeof (u_int16_t), 1, fh);
	releaseLoopBegin = BENDIAN_TO_HOST_INT16(releaseLoopBegin);
	af_fread(&releaseLoopEnd, sizeof (u_int16_t), 1, fh);
	releaseLoopEnd = BENDIAN_TO_HOST_INT16(releaseLoopEnd);

#ifdef DEBUG
	printf("sustain loop: mode %d, begin %d, end %d\n",
		sustainLoopPlayMode, sustainLoopBegin, sustainLoopEnd);

	printf("release loop: mode %d, begin %d, end %d\n",
		releaseLoopPlayMode, releaseLoopBegin, releaseLoopEnd);
#endif

	instrument->loops[0].id = 1;
	instrument->loops[0].mode = sustainLoopPlayMode;
	instrument->loops[0].beginMarker = sustainLoopBegin;
	instrument->loops[0].endMarker = sustainLoopEnd;

	instrument->loops[1].id = 2;
	instrument->loops[1].mode = releaseLoopPlayMode;
	instrument->loops[1].beginMarker = releaseLoopBegin;
	instrument->loops[1].endMarker = releaseLoopEnd;

	return AF_SUCCEED;
}

/*
	Parse marker chunks, which contain the positions and names of loop markers.
*/
static status ParseMARK (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size)
{
	_Track		*track;
	int		i;
	u_int16_t	numMarkers;

	assert(!memcmp(&type, "MARK", 4));

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fread(&numMarkers, sizeof (u_int16_t), 1, fh);
	numMarkers = BENDIAN_TO_HOST_INT16(numMarkers);

	track->markerCount = numMarkers;
	if (numMarkers)
		track->markers = _af_marker_new(numMarkers);

	for (i=0; i<numMarkers; i++)
	{
		u_int16_t	markerID = 0;
		u_int32_t	markerPosition = 0;
		u_int8_t	sizeByte = 0;
		char		*markerName = NULL;

		af_fread(&markerID, sizeof (u_int16_t), 1, fh);
		markerID = BENDIAN_TO_HOST_INT16(markerID);
		af_fread(&markerPosition, sizeof (u_int32_t), 1, fh);
		markerPosition = BENDIAN_TO_HOST_INT32(markerPosition);
		af_fread(&sizeByte, sizeof (unsigned char), 1, fh);
		markerName = _af_malloc(sizeByte + 1);
		af_fread(markerName, sizeof (unsigned char), sizeByte, fh);

		markerName[sizeByte] = '\0';

#ifdef DEBUG
		printf("marker id: %d, position: %d, name: %s\n",
			markerID, markerPosition, markerName);

		printf("size byte: %d\n", sizeByte);
#endif

		/*
			If sizeByte is even, then 1+sizeByte (the length
			of the string) is odd.	Skip an extra byte to
			make it even.
		*/

		if ((sizeByte % 2) == 0)
			af_fseek(fh, 1, SEEK_CUR);

		track->markers[i].id = markerID;
		track->markers[i].position = markerPosition;
		track->markers[i].name = markerName;
		track->markers[i].comment = _af_strdup("");
	}

	return AF_SUCCEED;
}

/*
	Parse common data chunks, which contain information regarding the
	sampling rate, the number of sample frames, and the number of
	sound channels.
*/
static status ParseCOMM (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size)
{
	_Track		*track;
	u_int16_t	numChannels;
	u_int32_t	numSampleFrames;
	u_int16_t	sampleSize;
	unsigned char	sampleRate[10];

	assert(!memcmp(&type, "COMM", 4));

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fread(&numChannels, sizeof (u_int16_t), 1, fh);
	track->f.channelCount = BENDIAN_TO_HOST_INT16(numChannels);

	af_fread(&numSampleFrames, sizeof (u_int32_t), 1, fh);
	track->totalfframes = BENDIAN_TO_HOST_INT32(numSampleFrames);

	af_fread(&sampleSize, sizeof (u_int16_t), 1, fh);
	track->f.sampleWidth = BENDIAN_TO_HOST_INT16(sampleSize);

	af_fread(sampleRate, 10, 1, fh);
	track->f.sampleRate = ConvertFromIeeeExtended(sampleRate);

	track->f.compressionType = AF_COMPRESSION_NONE;
	track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	if (file->fileFormat == AF_FILE_AIFFC)
	{
		u_int8_t	compressionID[4];
		/* Pascal strings are at most 255 bytes long. */
		unsigned char	compressionName[256];
		unsigned char	compressionNameLength;

		af_fread(compressionID, 4, 1, fh);

		/* Read the Pascal-style string containing the name. */
		af_fread(&compressionNameLength, 1, 1, fh);
		af_fread(compressionName, compressionNameLength, 1, fh);
		compressionName[compressionNameLength] = '\0';

		if (!memcmp(compressionID, "NONE", 4))
			track->f.compressionType = AF_COMPRESSION_NONE;
		else if (!memcmp(compressionID, "ACE2", 4) ||
			!memcmp(compressionID, "ACE8", 4) ||
			!memcmp(compressionID, "MAC3", 4) ||
			!memcmp(compressionID, "MAC6", 4))
		{
			_af_error(AF_BAD_NOT_IMPLEMENTED, "AIFF-C format does not support Apple's proprietary %s compression format", compressionName);
			return AF_FAIL;
		}
		else if (!memcmp(compressionID, "ulaw", 4) ||
			!memcmp(compressionID, "ULAW", 4))
		{
			track->f.compressionType = AF_COMPRESSION_G711_ULAW;
		}
		else if (!memcmp(compressionID, "alaw", 4) ||
			!memcmp(compressionID, "ALAW", 4))
		{
			track->f.compressionType = AF_COMPRESSION_G711_ALAW;
		}
		else if (!memcmp(compressionID, "fl32", 4) ||
			!memcmp(compressionID, "FL32", 4))
		{
			track->f.sampleFormat = AF_SAMPFMT_FLOAT;
			track->f.sampleWidth = 32;
			track->f.compressionType = AF_COMPRESSION_NONE;
		}
		else if (!memcmp(compressionID, "fl64", 4) ||
			!memcmp(compressionID, "FL64", 4))
		{
			track->f.sampleFormat = AF_SAMPFMT_DOUBLE;
			track->f.sampleWidth = 64;
			track->f.compressionType = AF_COMPRESSION_NONE;
		}
	}

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	return AF_SUCCEED;
}

/*
	Parse the stored sound chunk, which usually contains little more
	than the sound data.
*/
static status ParseSSND (AFfilehandle file, AFvirtualfile *fh, u_int32_t type,
	size_t size)
{
	_Track		*track;
	u_int32_t	offset, blockSize;

	assert(!memcmp(&type, "SSND", 4));

	track = _af_filehandle_get_track(file, AF_DEFAULT_TRACK);

	af_fread(&offset, sizeof (u_int32_t), 1, fh);
	offset = BENDIAN_TO_HOST_INT32(offset);
	af_fread(&blockSize, sizeof (u_int32_t), 1, fh);
	blockSize = BENDIAN_TO_HOST_INT32(blockSize);

	/*
		This seems like a reasonable way to calculate the number of
		bytes in an SSND chunk.
	*/
	track->data_size = size - 8 - offset;

#ifdef DEBUG
	printf("offset: %d\n", offset);
	printf("block size: %d\n", blockSize);
#endif

	track->fpos_first_frame = af_ftell(fh) + offset;

#ifdef DEBUG
	printf("data start: %d\n", track->fpos_first_frame);
#endif

	/* Sound data follows. */

	return AF_SUCCEED;
}

status _af_aiff_read_init (AFfilesetup setup, AFfilehandle file)
{
	u_int32_t	type, size, formtype;
	size_t		index = 0;
	bool		hasCOMM, hasFVER, hasSSND, hasMARK, hasINST;
	bool		hasAESD, hasNAME, hasAUTH, hasCOPY;
	_Track		*track;

	hasCOMM = AF_FALSE;
	hasFVER = AF_FALSE;
	hasSSND = AF_FALSE;
	hasMARK = AF_FALSE;
	hasINST = AF_FALSE;
	hasAESD = AF_FALSE;
	hasNAME = AF_FALSE;
	hasAUTH = AF_FALSE;
	hasCOPY = AF_FALSE;

	assert(file != NULL);
	assert(file->fh != NULL);

	af_fseek(file->fh, 0, SEEK_SET);

	af_fread(&type, 4, 1, file->fh);
	af_fread(&size, 4, 1, file->fh);
	size = BENDIAN_TO_HOST_INT32(size);
	af_fread(&formtype, 4, 1, file->fh);

	if (memcmp(&type, "FORM", 4) != 0 ||
		(memcmp(&formtype, "AIFF", 4) && memcmp(&formtype, "AIFC", 4)))
		return AF_FAIL;

#ifdef DEBUG
	printf("size: %d\n", size);
#endif

	file->instrumentCount = 0;
	file->instruments = NULL;
	file->miscellaneousCount = 0;
	file->miscellaneous = NULL;

	/* AIFF files have only one track. */
	track = _af_track_new();
	file->trackCount = 1;
	file->tracks = track;

	/* Include the offset of the form type. */
	index += 4;

	while (index < size)
	{
		u_int32_t	chunkid = 0, chunksize = 0;

#ifdef DEBUG
		printf("index: %d\n", index);
#endif
		af_fread(&chunkid, 4, 1, file->fh);
		af_fread(&chunksize, 4, 1, file->fh);
		chunksize = BENDIAN_TO_HOST_INT32(chunksize);

#ifdef DEBUG
		_af_printid(chunkid);
		printf(" size: %d\n", chunksize);
#endif

		if (!memcmp("COMM", &chunkid, 4))
		{
			hasCOMM = AF_TRUE;
			ParseCOMM(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("FVER", &chunkid, 4))
		{
			hasFVER = AF_TRUE;
			ParseFVER(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("INST", &chunkid, 4))
		{
			hasINST = AF_TRUE;
			ParseINST(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("MARK", &chunkid, 4))
		{
			hasMARK = AF_TRUE;
			ParseMARK(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("AESD", &chunkid, 4))
		{
			hasAESD = AF_TRUE;
			ParseAESD(file, file->fh, chunkid, chunksize);
		}
		else if (!memcmp("NAME", &chunkid, 4) ||
			!memcmp("AUTH", &chunkid, 4) ||
			!memcmp("(c) ", &chunkid, 4) ||
			!memcmp("ANNO", &chunkid, 4) ||
			!memcmp("APPL", &chunkid, 4) ||
			!memcmp("MIDI", &chunkid, 4))
		{
			ParseMiscellaneous(file, file->fh, chunkid, chunksize);
		}
		/*
			The sound data chunk is required if there are more than
			zero sample frames.
		*/
		else if (!memcmp("SSND", &chunkid, 4))
		{
			if (hasSSND)
			{
				_af_error(AF_BAD_AIFF_SSND, "AIFF file has more than one SSND chunk");
				return AF_FAIL;
			}
			hasSSND = AF_TRUE;
			ParseSSND(file, file->fh, chunkid, chunksize);
		}

		index += chunksize + 8;

		/* all chunks must be aligned on an even number of bytes */
		if ((index % 2) != 0)
			index++;

		af_fseek(file->fh, index + 8, SEEK_SET);
	}

	if (!hasCOMM)
	{
		_af_error(AF_BAD_AIFF_COMM, "bad AIFF COMM chunk");
	}

	/* The file has been successfully parsed. */
	return AF_SUCCEED;
}

bool _af_aiff_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 1, 8, fh) != 8 || memcmp(buffer, "FORM", 4) != 0)
		return AF_FALSE;
	if (af_fread(buffer, 1, 4, fh) != 4 || memcmp(buffer, "AIFF", 4) != 0)
		return AF_FALSE;

	return AF_TRUE;
}

bool _af_aifc_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 1, 8, fh) != 8 || memcmp(buffer, "FORM", 4) != 0)
		return AF_FALSE;
	if (af_fread(buffer, 1, 4, fh) != 4 || memcmp(buffer, "AIFC", 4) != 0)
		return AF_FALSE;

	return AF_TRUE;
}

AFfilesetup _af_aiff_complete_setup (AFfilesetup setup)
{
	_TrackSetup	*track;

	bool	isAIFF = setup->fileFormat == AF_FILE_AIFF;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "AIFF/AIFF-C file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	track = &setup->tracks[0];

	if (track->sampleFormatSet)
	{
		if (track->f.sampleFormat == AF_SAMPFMT_UNSIGNED)
		{
			_af_error(AF_BAD_FILEFMT, "AIFF/AIFF-C format does not support unsigned data");
			return AF_NULL_FILESETUP;
		}
		else if (isAIFF && track->f.sampleFormat != AF_SAMPFMT_TWOSCOMP)
		{
			_af_error(AF_BAD_FILEFMT, "AIFF format supports only two's complement integer data");
			return AF_NULL_FILESETUP;
		}
	}
	else
		_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP,
			track->f.sampleWidth);

	/* Check sample width if writing two's complement. Otherwise ignore. */
	if (track->f.sampleFormat == AF_SAMPFMT_TWOSCOMP &&
		(track->f.sampleWidth < 1 || track->f.sampleWidth > 32))
	{
		_af_error(AF_BAD_WIDTH,
			"invalid sample width %d for AIFF/AIFF-C file "
			"(must be 1-32)", track->f.sampleWidth);
		return AF_NULL_FILESETUP;
	}

	if (isAIFF && track->f.compressionType != AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_FILESETUP,
			"AIFF does not support compression; use AIFF-C");
		return AF_NULL_FILESETUP;
	}

	/* XXXmpruett handle compression here */

	if (track->byteOrderSet &&
		track->f.byteOrder != AF_BYTEORDER_BIGENDIAN &&
		track->f.sampleWidth > 8)
	{
		_af_error(AF_BAD_BYTEORDER,
			"AIFF/AIFF-C format supports only big-endian data");
	}
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	if (setup->instrumentSet)
	{
		if (setup->instrumentCount != 0 && setup->instrumentCount != 1)
		{
			_af_error(AF_BAD_NUMINSTS, "AIFF/AIFF-C file must have 0 or 1 instrument chunk");
			return AF_NULL_FILESETUP;
		}
		if (setup->instruments != 0 &&
			setup->instruments[0].loopCount != 2)
		{
			_af_error(AF_BAD_NUMLOOPS, "AIFF/AIFF-C file with instrument must also have 2 loops");
			return AF_NULL_FILESETUP;
		}
	}

	if (setup->miscellaneousSet)
	{
		int	i;
		for (i=0; i<setup->miscellaneousCount; i++)
		{
			switch (setup->miscellaneous[i].type)
			{
				case AF_MISC_COPY:
				case AF_MISC_AUTH:
				case AF_MISC_NAME:
				case AF_MISC_ANNO:
				case AF_MISC_APPL:
				case AF_MISC_MIDI:
					break;

				default:
					_af_error(AF_BAD_MISCTYPE, "invalid miscellaneous type %d for AIFF/AIFF-C file", setup->miscellaneous[i].type);
					return AF_NULL_FILESETUP;
			}
		}
	}

	return _af_filesetup_copy(setup, &_af_aiff_default_filesetup, AF_TRUE);
}

bool _af_aiff_instparam_valid (AFfilehandle filehandle, AUpvlist list, int i)
{
	int	param, type, lval;

	AUpvgetparam(list, i, &param);
	AUpvgetvaltype(list, i, &type);
	if (type != AU_PVTYPE_LONG)
		return AF_FALSE;

	AUpvgetval(list, i, &lval);

	switch (param)
	{
		case AF_INST_MIDI_BASENOTE:
			return ((lval >= 0) && (lval <= 127));

		case AF_INST_NUMCENTS_DETUNE:
			return ((lval >= -50) && (lval <= 50));

		case AF_INST_MIDI_LOVELOCITY:
			return ((lval >= 1) && (lval <= 127));

		case AF_INST_MIDI_HIVELOCITY:
			return ((lval >= 1) && (lval <= 127));

		case AF_INST_MIDI_LONOTE:
			return ((lval >= 0) && (lval <= 127));

		case AF_INST_MIDI_HINOTE:
			return ((lval >= 0) && (lval <= 127));

		case AF_INST_NUMDBS_GAIN:
		case AF_INST_SUSLOOPID:
		case AF_INST_RELLOOPID:
			return AF_TRUE;

		default:
			return AF_FALSE;
			break;
	}

	return AF_TRUE;
}

int _af_aifc_get_version (AFfilehandle file)
{
	return AIFC_VERSION_1;
}

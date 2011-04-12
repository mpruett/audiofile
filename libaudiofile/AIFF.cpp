/*
	Audio File Library
	Copyright (C) 1998-2000, 2003, Michael Pruett <michael@68k.org>
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
	AIFF.cpp

	This file contains routines for reading and writing AIFF and
	AIFF-C sound files.
*/

#include "config.h"
#include "AIFF.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Instrument.h"
#include "Marker.h"
#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "byteorder.h"
#include "extended.h"
#include "util.h"

const InstParamInfo _af_aiff_inst_params[_AF_AIFF_NUM_INSTPARAMS] =
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

const int _af_aiffc_compression_types[_AF_AIFFC_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

static _AFfilesetup aiff_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_AIFF,		/* fileFormat */
	true,			/* trackSet */
	true,			/* instrumentSet */
	true,			/* miscellaneousSet */
	1,			/* trackCount */
	NULL,			/* tracks */
	1,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

#define AIFC_VERSION_1 0xa2805140

struct _INST
{
	uint8_t		baseNote;
	int8_t		detune;
	uint8_t		lowNote, highNote;
	uint8_t		lowVelocity, highVelocity;
	int16_t		gain;

	uint16_t	sustainLoopPlayMode;
	uint16_t	sustainLoopBegin;
	uint16_t	sustainLoopEnd;

	uint16_t	releaseLoopPlayMode;
	uint16_t	releaseLoopBegin;
	uint16_t	releaseLoopEnd;
};

AIFFFile::AIFFFile()
{
	setFormatByteOrder(AF_BYTEORDER_BIGENDIAN);

	miscellaneousPosition = 0;
	FVER_offset = 0;
	COMM_offset = 0;
	MARK_offset = 0;
	INST_offset = 0;
	AESD_offset = 0;
	SSND_offset = 0;
}

/*
	FVER chunks are only present in AIFF-C files.
*/
status AIFFFile::parseFVER(uint32_t type, size_t size)
{
	assert(!memcmp(&type, "FVER", 4));

	uint32_t timestamp;
	readU32(&timestamp);
	/* timestamp holds the number of seconds since January 1, 1904. */

	return AF_SUCCEED;
}

/*
	Parse AES recording data.
*/
status AIFFFile::parseAESD(uint32_t type, size_t size)
{
	unsigned char aesChannelStatusData[24];

	assert(!memcmp(&type, "AESD", 4));
	assert(size == 24);

	Track *track = getTrack();

	track->hasAESData = true;

	/*
		Try to read 24 bytes of AES nonaudio data from the file.
		Fail if the file disappoints.
	*/
	if (af_read(aesChannelStatusData, 24, fh) != 24)
		return AF_FAIL;

	memcpy(track->aesData, aesChannelStatusData, 24);

	return AF_SUCCEED;
}

/*
	Parse miscellaneous data chunks such as name, author, copyright,
	and annotation chunks.
*/
status AIFFFile::parseMiscellaneous(uint32_t type, size_t size)
{
	int misctype = AF_MISC_UNRECOGNIZED;

	assert(!memcmp(&type, "NAME", 4) || !memcmp(&type, "AUTH", 4) ||
		!memcmp(&type, "(c) ", 4) || !memcmp(&type, "ANNO", 4) ||
		!memcmp(&type, "APPL", 4) || !memcmp(&type, "MIDI", 4));

	/* Skip zero-length miscellaneous chunks. */
	if (size == 0)
		return AF_FAIL;

	miscellaneousCount++;
	miscellaneous = (Miscellaneous *) _af_realloc(miscellaneous,
		miscellaneousCount * sizeof (Miscellaneous));

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

	miscellaneous[miscellaneousCount - 1].id = miscellaneousCount;
	miscellaneous[miscellaneousCount - 1].type = misctype;
	miscellaneous[miscellaneousCount - 1].size = size;
	miscellaneous[miscellaneousCount - 1].position = 0;
	miscellaneous[miscellaneousCount - 1].buffer = _af_malloc(size);
	af_read(miscellaneous[miscellaneousCount - 1].buffer,
		size, fh);

	return AF_SUCCEED;
}

/*
	Parse instrument chunks, which contain information about using
	sound data as a sampled instrument.
*/
status AIFFFile::parseINST(uint32_t type, size_t size)
{
	uint8_t baseNote;
	int8_t detune;
	uint8_t lowNote, highNote, lowVelocity, highVelocity;
	int16_t gain;

	uint16_t sustainLoopPlayMode, sustainLoopBegin, sustainLoopEnd;
	uint16_t releaseLoopPlayMode, releaseLoopBegin, releaseLoopEnd;

	assert(!memcmp(&type, "INST", 4));

	Instrument *instrument = (Instrument *) _af_calloc(1, sizeof (Instrument));
	instrument->id = AF_DEFAULT_INST;
	instrument->values = (AFPVu *) _af_calloc(_AF_AIFF_NUM_INSTPARAMS, sizeof (AFPVu));
	instrument->loopCount = 2;
	instrument->loops = (Loop *) _af_calloc(2, sizeof (Loop));

	instrumentCount = 1;
	instruments = instrument;

	readU8(&baseNote);
	readS8(&detune);
	readU8(&lowNote);
	readU8(&highNote);
	readU8(&lowVelocity);
	readU8(&highVelocity);
	readS16(&gain);

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

	readU16(&sustainLoopPlayMode);
	readU16(&sustainLoopBegin);
	readU16(&sustainLoopEnd);

	readU16(&releaseLoopPlayMode);
	readU16(&releaseLoopBegin);
	readU16(&releaseLoopEnd);

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
status AIFFFile::parseMARK(uint32_t type, size_t size)
{
	assert(!memcmp(&type, "MARK", 4));

	Track *track = getTrack();

	uint16_t numMarkers;
	readU16(&numMarkers);

	track->markerCount = numMarkers;
	if (numMarkers)
		track->markers = _af_marker_new(numMarkers);

	for (unsigned i=0; i<numMarkers; i++)
	{
		uint16_t markerID = 0;
		uint32_t markerPosition = 0;
		uint8_t sizeByte = 0;
		char *markerName = NULL;

		readU16(&markerID);
		readU32(&markerPosition);
		af_read(&sizeByte, 1, fh);
		markerName = (char *) _af_malloc(sizeByte + 1);
		af_read(markerName, sizeByte, fh);

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
status AIFFFile::parseCOMM(uint32_t type, size_t size)
{
	assert(!memcmp(&type, "COMM", 4));

	Track *track = getTrack();

	uint16_t numChannels;
	uint32_t numSampleFrames;
	uint16_t sampleSize;
	unsigned char sampleRate[10];

	readU16(&numChannels);
	track->f.channelCount = numChannels;

	readU32(&numSampleFrames);
	track->totalfframes = numSampleFrames;

	readU16(&sampleSize);
	track->f.sampleWidth = sampleSize;

	af_read(sampleRate, 10, fh);
	track->f.sampleRate = _af_convert_from_ieee_extended(sampleRate);

	track->f.compressionType = AF_COMPRESSION_NONE;
	track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	if (fileFormat == AF_FILE_AIFFC)
	{
		uint8_t compressionID[4];
		/* Pascal strings are at most 255 bytes long. */
		char compressionName[256];

		af_read(compressionID, 4, fh);

		/* Read the Pascal-style string containing the name. */
		af_read_pstring(compressionName, fh);

		if (!memcmp(compressionID, "NONE", 4) ||
			!memcmp(compressionID, "twos", 4))
		{
			track->f.compressionType = AF_COMPRESSION_NONE;
		}
		else if (!memcmp(compressionID, "in24", 4))
		{
			track->f.compressionType = AF_COMPRESSION_NONE;
			track->f.sampleWidth = 24;
		}
		else if (!memcmp(compressionID, "in32", 4))
		{
			track->f.compressionType = AF_COMPRESSION_NONE;
			track->f.sampleWidth = 32;
		}
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
		else if (!memcmp(compressionID, "sowt", 4))
		{
			track->f.compressionType = AF_COMPRESSION_NONE;
			track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;
		}
		else
		{
			_af_error(AF_BAD_NOT_IMPLEMENTED, "AIFF-C compression type '%c%c%c%c' not currently supported",
				compressionID[0],
				compressionID[1],
				compressionID[2],
				compressionID[3]);
			return AF_FAIL;
		}
	}

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	return AF_SUCCEED;
}

/*
	Parse the stored sound chunk, which usually contains little more
	than the sound data.
*/
status AIFFFile::parseSSND(uint32_t type, size_t size)
{
	assert(!memcmp(&type, "SSND", 4));

	Track *track = getTrack();

	uint32_t offset, blockSize;
	readU32(&offset);
	readU32(&blockSize);

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

status AIFFFile::readInit(AFfilesetup setup)
{
	uint32_t type, size, formtype;

	bool hasCOMM = false;
	bool hasFVER = false;
	bool hasSSND = false;
	bool hasMARK = false;
	bool hasINST = false;
	bool hasAESD = false;
	bool hasNAME = false;
	bool hasAUTH = false;
	bool hasCOPY = false;

	assert(fh != NULL);

	af_fseek(fh, 0, SEEK_SET);

	af_read(&type, 4, fh);
	readU32(&size);
	af_read(&formtype, 4, fh);

	if (memcmp(&type, "FORM", 4) != 0 ||
		(memcmp(&formtype, "AIFF", 4) && memcmp(&formtype, "AIFC", 4)))
		return AF_FAIL;

#ifdef DEBUG
	printf("size: %d\n", size);
#endif

	instrumentCount = 0;
	instruments = NULL;
	miscellaneousCount = 0;
	miscellaneous = NULL;

	/* AIFF files have only one track. */
	Track *track = _af_track_new();
	trackCount = 1;
	tracks = track;

	/* Include the offset of the form type. */
	size_t index = 4;
	while (index < size)
	{
		uint32_t chunkid = 0, chunksize = 0;
		status result = AF_SUCCEED;

#ifdef DEBUG
		printf("index: %d\n", index);
#endif
		af_read(&chunkid, 4, fh);
		readU32(&chunksize);

#ifdef DEBUG
		_af_printid(chunkid);
		printf(" size: %d\n", chunksize);
#endif

		if (!memcmp("COMM", &chunkid, 4))
		{
			hasCOMM = true;
			result = parseCOMM(chunkid, chunksize);
		}
		else if (!memcmp("FVER", &chunkid, 4))
		{
			hasFVER = true;
			parseFVER(chunkid, chunksize);
		}
		else if (!memcmp("INST", &chunkid, 4))
		{
			hasINST = true;
			parseINST(chunkid, chunksize);
		}
		else if (!memcmp("MARK", &chunkid, 4))
		{
			hasMARK = true;
			parseMARK(chunkid, chunksize);
		}
		else if (!memcmp("AESD", &chunkid, 4))
		{
			hasAESD = true;
			parseAESD(chunkid, chunksize);
		}
		else if (!memcmp("NAME", &chunkid, 4) ||
			!memcmp("AUTH", &chunkid, 4) ||
			!memcmp("(c) ", &chunkid, 4) ||
			!memcmp("ANNO", &chunkid, 4) ||
			!memcmp("APPL", &chunkid, 4) ||
			!memcmp("MIDI", &chunkid, 4))
		{
			parseMiscellaneous(chunkid, chunksize);
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
			hasSSND = true;
			result = parseSSND(chunkid, chunksize);
		}

		if (result == AF_FAIL)
			return AF_FAIL;

		index += chunksize + 8;

		/* all chunks must be aligned on an even number of bytes */
		if ((index % 2) != 0)
			index++;

		af_fseek(fh, index + 8, SEEK_SET);
	}

	if (!hasCOMM)
	{
		_af_error(AF_BAD_AIFF_COMM, "bad AIFF COMM chunk");
	}

	/* The file has been successfully parsed. */
	return AF_SUCCEED;
}

bool AIFFFile::recognizeAIFF(File *fh)
{
	uint8_t buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(buffer, 8, fh) != 8 || memcmp(buffer, "FORM", 4) != 0)
		return false;
	if (af_read(buffer, 4, fh) != 4 || memcmp(buffer, "AIFF", 4) != 0)
		return false;

	return true;
}

bool AIFFFile::recognizeAIFFC(File *fh)
{
	uint8_t buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(buffer, 8, fh) != 8 || memcmp(buffer, "FORM", 4) != 0)
		return false;
	if (af_read(buffer, 4, fh) != 4 || memcmp(buffer, "AIFC", 4) != 0)
		return false;

	return true;
}

AFfilesetup AIFFFile::completeSetup(AFfilesetup setup)
{
	TrackSetup	*track;

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
		for (int i=0; i<setup->miscellaneousCount; i++)
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

	return _af_filesetup_copy(setup, &aiff_default_filesetup, true);
}

bool AIFFFile::isInstrumentParameterValid(AUpvlist list, int i)
{
	int param, type, lval;

	AUpvgetparam(list, i, &param);
	AUpvgetvaltype(list, i, &type);
	if (type != AU_PVTYPE_LONG)
		return false;

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
			return true;

		default:
			return false;
			break;
	}

	return true;
}

int AIFFFile::getVersion()
{
	if (fileFormat == AF_FILE_AIFFC)
		return AIFC_VERSION_1;
	return 0;
}

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

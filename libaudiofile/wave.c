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
	wave.c

	This file contains code for parsing RIFF WAVE format sound files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audiofile.h"
#include "util.h"
#include "afinternal.h"
#include "byteorder.h"
#include "wave.h"
#include "track.h"
#include "setup.h"
#include "marker.h"

int _af_wave_compression_types[_AF_WAVE_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

_InstParamInfo _af_wave_inst_params[_AF_WAVE_NUM_INSTPARAMS] =
{
	{ AF_INST_MIDI_BASENOTE, AU_PVTYPE_LONG, "MIDI base note", {60} },
	{ AF_INST_NUMCENTS_DETUNE, AU_PVTYPE_LONG, "Detune in cents", {0} },
	{ AF_INST_MIDI_LOVELOCITY, AU_PVTYPE_LONG, "Low velocity", {1} },
	{ AF_INST_MIDI_HIVELOCITY, AU_PVTYPE_LONG, "High velocity", {127} },
	{ AF_INST_MIDI_LONOTE, AU_PVTYPE_LONG, "Low note", {0} },
	{ AF_INST_MIDI_HINOTE, AU_PVTYPE_LONG, "High note", {127} },
	{ AF_INST_NUMDBS_GAIN, AU_PVTYPE_LONG, "Gain in dB", {0} }
};

_AFfilesetup _af_wave_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_WAVE,		/* fileFormat */
	AF_TRUE,		/* trackSet */
	AF_TRUE,		/* instrumentSet */
	AF_TRUE,		/* miscellaneousSet  */
	1,			/* trackCount */
	NULL,			/* tracks */
	0,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

static status ParseFact (AFfilehandle filehandle, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	u_int32_t	totalFrames;
	_Track		*track;

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	af_fread(&totalFrames, 1, 4, fp);
	track->totalfframes = totalFrames;

	return AF_SUCCEED;
}

static status ParseFormat (AFfilehandle filehandle, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	_Track		*track;
	u_int16_t	formatTag, channelCount;
	u_int32_t	sampleRate, averageBytesPerSecond;
	u_int16_t	blockAlign;

	assert(filehandle != NULL);
	assert(fp != NULL);
	assert(!memcmp(&id, "fmt ", 4));

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	af_fread(&formatTag, 1, 2, fp);
	formatTag = LENDIAN_TO_HOST_INT16(formatTag);

	af_fread(&channelCount, 1, 2, fp);
	channelCount = LENDIAN_TO_HOST_INT16(channelCount);
	track->f.channelCount = channelCount;

	af_fread(&sampleRate, 1, 4, fp);
	sampleRate = LENDIAN_TO_HOST_INT32(sampleRate);
	track->f.sampleRate = sampleRate;

	af_fread(&averageBytesPerSecond, 1, 4, fp);
	averageBytesPerSecond = LENDIAN_TO_HOST_INT32(averageBytesPerSecond);

	af_fread(&blockAlign, 1, 2, fp);
	blockAlign = LENDIAN_TO_HOST_INT16(blockAlign);

	track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;

	switch (formatTag)
	{
		case WAVE_FORMAT_PCM:
		{
			u_int16_t	bitsPerSample;

			af_fread(&bitsPerSample, 1, 2, fp);
			bitsPerSample = LENDIAN_TO_HOST_INT16(bitsPerSample);

			track->f.sampleWidth = bitsPerSample;
			track->f.compressionType = AF_COMPRESSION_NONE;

			if (bitsPerSample == 0 || bitsPerSample > 32)
			{
				_af_error(AF_BAD_WIDTH,
					"bad sample width of %d bits",
					bitsPerSample);
				return AF_FAIL;
			}

			if (bitsPerSample <= 8)
				track->f.sampleFormat = AF_SAMPFMT_UNSIGNED;
			else
				track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
		}
		break;

		case WAVE_FORMAT_MULAW:
		case IBM_FORMAT_MULAW:
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.compressionType = AF_COMPRESSION_G711_ULAW;
			break;

		case WAVE_FORMAT_ALAW:
		case IBM_FORMAT_ALAW:
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.compressionType = AF_COMPRESSION_G711_ALAW;
			break;

		default:
			_af_error(AF_BAD_FILEFMT, "bad WAVE file format");
			return AF_FAIL;
			break;
	}

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	return AF_SUCCEED;
}

static status ParseData (AFfilehandle filehandle, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	_Track	*track;

	assert(filehandle != NULL);
	assert(fp != NULL);
	assert(!memcmp(&id, "data", 4));

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	track->fpos_first_frame = af_ftell(fp);
	track->data_size = size;

	return AF_SUCCEED;
}

static status ParseInfo (AFfilehandle file, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	int	location = 0;

	while (location < size)
	{
		int		misctype = AF_MISC_UNRECOGNIZED;
		u_int32_t	miscid, miscsize;

		af_fread(&miscid, 4, 1, fp);
		location += 4;
		af_fread(&miscsize, 4, 1, fp);
		miscsize = LENDIAN_TO_HOST_INT32(miscsize);
		location += 4;

		if (memcmp(&miscid, "IART", 4) == 0)
			misctype = AF_MISC_AUTH;
		else if (memcmp(&miscid, "INAM", 4) == 0)
			misctype = AF_MISC_NAME;
		else if (memcmp(&miscid, "ICOP", 4) == 0)
			misctype = AF_MISC_COPY;
		else if (memcmp(&miscid, "ICMT", 4) == 0)
			misctype = AF_MISC_ICMT;
		else if (memcmp(&miscid, "ICRD", 4) == 0)
			misctype = AF_MISC_ICRD;
		else if (memcmp(&miscid, "ISFT", 4) == 0)
			misctype = AF_MISC_ISFT;

		if (misctype != AF_MISC_UNRECOGNIZED)
		{
			char	*string = _af_malloc(miscsize);

			af_fread(string, miscsize, 1, fp);

			file->miscellaneousCount++;
			file->miscellaneous = _af_realloc(file->miscellaneous, sizeof (_Miscellaneous) * file->miscellaneousCount);

			file->miscellaneous[file->miscellaneousCount-1].id = file->miscellaneousCount;
			file->miscellaneous[file->miscellaneousCount-1].type = misctype;
			file->miscellaneous[file->miscellaneousCount-1].size = miscsize;
			file->miscellaneous[file->miscellaneousCount-1].position = 0;
			file->miscellaneous[file->miscellaneousCount-1].buffer = string;

			location += miscsize;
		}

		/*
			Make the current position an even number of bytes.
		*/
		if (miscsize % 2 != 0)
			af_fseek(fp, 1, SEEK_CUR);
	}
	return AF_SUCCEED;
}

static status ParsePlayList (AFfilehandle filehandle, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	_Instrument	*instrument;
	u_int32_t	segmentCount;
	int		segment;

	af_fread(&segmentCount, 4, 1, fp);
	segmentCount = LENDIAN_TO_HOST_INT32(segmentCount);

	if (segmentCount == 0)
	{
		filehandle->instrumentCount = 0;
		filehandle->instruments = NULL;
		return AF_SUCCEED;
	}

	for (segment=0; segment<segmentCount; segment++)
	{
		u_int32_t	startMarkID, loopLength, loopCount;

		af_fread(&startMarkID, 4, 1, fp);
		startMarkID = LENDIAN_TO_HOST_INT32(startMarkID);
		af_fread(&loopLength, 4, 1, fp);
		loopLength = LENDIAN_TO_HOST_INT32(loopLength);
		af_fread(&loopCount, 4, 1, fp);
		loopCount = LENDIAN_TO_HOST_INT32(loopCount);
	}

	return AF_SUCCEED;
}

static status ParseCue (AFfilehandle filehandle, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	_Track		*track;
	u_int32_t	markerCount;
	int		i;

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	af_fread(&markerCount, 4, 1, fp);
	markerCount = LENDIAN_TO_HOST_INT32(markerCount);
	track->markerCount = markerCount;

	if (markerCount == 0)
	{
		track->markers = NULL;
		return AF_SUCCEED;
	}

	if ((track->markers = _af_marker_new(markerCount)) == NULL)
		return AF_FAIL;

	for (i=0; i<markerCount; i++)
	{
		u_int32_t	id, position, chunkid;
		u_int32_t	chunkByteOffset, blockByteOffset;
		u_int32_t	sampleByteOffset;
		_Marker		*marker = &track->markers[i];

		af_fread(&id, 4, 1, fp);
		id = LENDIAN_TO_HOST_INT32(id);

		af_fread(&position, 4, 1, fp);
		position = LENDIAN_TO_HOST_INT32(position);

		af_fread(&chunkid, 4, 1, fp);
		chunkid = LENDIAN_TO_HOST_INT32(chunkid);

		af_fread(&chunkByteOffset, 4, 1, fp);
		chunkByteOffset = LENDIAN_TO_HOST_INT32(chunkByteOffset);

		af_fread(&blockByteOffset, 4, 1, fp);
		blockByteOffset = LENDIAN_TO_HOST_INT32(blockByteOffset);

		af_fread(&sampleByteOffset, 4, 1, fp);
		sampleByteOffset = LENDIAN_TO_HOST_INT32(sampleByteOffset);

		marker->id = id;
		marker->position = sampleByteOffset;
		marker->name = _af_strdup("");
		marker->comment = _af_strdup("");
	}

	return AF_SUCCEED;
}

static status ParseInstrument (AFfilehandle filehandle, AFvirtualfile *fp,
	u_int32_t id, size_t size)
{
	u_int8_t	baseNote;
	int8_t		detune, gain;
	u_int8_t	lowNote, highNote, lowVelocity, highVelocity;
	u_int8_t	padByte;

	af_fread(&baseNote, 1, 1, fp);
	af_fread(&detune, 1, 1, fp);
	af_fread(&gain, 1, 1, fp);
	af_fread(&lowNote, 1, 1, fp);
	af_fread(&highNote, 1, 1, fp);
	af_fread(&lowVelocity, 1, 1, fp);
	af_fread(&highVelocity, 1, 1, fp);
	af_fread(&padByte, 1, 1, fp);

	return AF_SUCCEED;
}

bool _af_wave_recognize (AFvirtualfile *fh)
{
	u_int8_t	buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 1, 8, fh) != 8 || memcmp(buffer, "RIFF", 4) != 0)
		return AF_FALSE;
	if (af_fread(buffer, 1, 4, fh) != 4 || memcmp(buffer, "WAVE", 4) != 0)
		return AF_FALSE;

	return AF_TRUE;
}

status _af_wave_read_init (AFfilesetup setup, AFfilehandle filehandle)
{
	_Track		*track;
	u_int32_t	type, size, formtype;
	u_int32_t	index = 0;
	bool		hasFormat, hasData, hasCue, hasPlayList, hasFact;
	bool		hasInstrument;
	int		frameSize;

	assert(filehandle != NULL);
	assert(filehandle->fh != NULL);

	hasFormat = AF_FALSE;
	hasData = AF_FALSE;
	hasCue = AF_FALSE;
	hasPlayList = AF_FALSE;
	hasInstrument = AF_FALSE;
	hasFact = AF_FALSE;

	filehandle->instruments = NULL;
	filehandle->instrumentCount = 0;
	filehandle->miscellaneous = NULL;
	filehandle->miscellaneousCount = 0;

	track = _af_track_new();
	filehandle->tracks = track;
	filehandle->trackCount = 1;

	af_fseek(filehandle->fh, 0, SEEK_SET);

	af_fread(&type, 4, 1, filehandle->fh);
	af_fread(&size, 4, 1, filehandle->fh);
	size = LENDIAN_TO_HOST_INT32(size);
	af_fread(&formtype, 4, 1, filehandle->fh);

	assert(!memcmp(&type, "RIFF", 4));
	assert(!memcmp(&formtype, "WAVE", 4));

#ifdef DEBUG
	printf("size: %d\n", size);
#endif

	/* Include the offset of the form type. */
	index += 4;

	while (index < size)
	{
		u_int32_t	chunkid = 0, chunksize = 0;
		status		result;

#ifdef DEBUG
		printf("index: %d\n", index);
#endif
		af_fread(&chunkid, 4, 1, filehandle->fh);

		af_fread(&chunksize, 4, 1, filehandle->fh);
		chunksize = LENDIAN_TO_HOST_INT32(chunksize);

#ifdef DEBUG
		_af_printid(BENDIAN_TO_HOST_INT32(chunkid));
		printf(" size: %d\n", chunksize);
#endif

		if (memcmp(&chunkid, "fmt ", 4) == 0)
		{
			result = ParseFormat(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;

			hasFormat = AF_TRUE;
		}
		else if (memcmp(&chunkid, "data", 4) == 0)
		{
			result = ParseData(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;

			hasData = AF_TRUE;
		}
		else if (memcmp(&chunkid, "inst", 4) == 0)
		{
			result = ParseInstrument(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "fact", 4) == 0)
		{
			hasFact = AF_TRUE;
			result = ParseFact(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "cue ", 4) == 0)
		{
			hasFact = AF_TRUE;
			result = ParseCue(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "INFO", 4) == 0)
		{
			hasFact = AF_TRUE;
			result = ParseInfo(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "INST", 4) == 0)
		{
			hasInstrument = AF_TRUE;
			result = ParseInstrument(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "plst", 4) == 0)
		{
			hasPlayList = AF_TRUE;
			result = ParsePlayList(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}

		index += chunksize + 8;

		/* All chunks must be aligned on an even number of bytes */
		if ((index % 2) != 0)
			index++;

		af_fseek(filehandle->fh, index + 8, SEEK_SET);
	}

	/* The format chunk and the data chunk are required. */
	if (!hasFormat || !hasData)
		return AF_FAIL;

	/*
		At this point we know that the file has a format chunk and
		a data chunk, so we can assume that channelCount,
		sampleWidth, and trackBytes have been initialized.
	*/

	/*
		We always round up to the nearest byte for the frame size.
	*/
	frameSize = _af_format_frame_size(&track->f, AF_FALSE);
	track->totalfframes = track->data_size / frameSize;

	/*
		The format and data chunks must be present.

		The WAVE format requires only that the format chunk must
		precede the data chunk.  All other chunks can occur in
		any order.

		Because we allow chunks to come in any order, we must
		post-process the markers' position now that we know the
		track's audio format has been initialized.

		Perhaps it's a hack, but no more so than the WAVE format.
	*/
	if (hasCue)
	{
		int	i;
		for (i=0; i<track->markerCount; i++)
			track->markers[i].position /= _af_format_frame_size(&track->f, AF_FALSE);
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		(track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		track->f.compressionType == AF_COMPRESSION_G711_ALAW))
	{
		track->totalfframes = size / track->f.channelCount;
	}

	/*
		A return value of AF_SUCCEED indicates successful parsing.
	*/
	return AF_SUCCEED;
}

AFfilesetup _af_wave_complete_setup (AFfilesetup setup)
{
	AFfilesetup	newsetup;
	_TrackSetup	*track;

	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "WAVE file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	track = _af_filesetup_get_tracksetup(setup, AF_DEFAULT_TRACK);

	if (track->sampleFormatSet)
	{
		switch (track->f.sampleFormat)
		{
			case AF_SAMPFMT_FLOAT:
			case AF_SAMPFMT_DOUBLE:
				_af_error(AF_BAD_SAMPFMT, "WAVE format does not support floating-point data");
				return AF_NULL_FILESETUP;
				break;
			case AF_SAMPFMT_UNSIGNED:
				if (track->sampleWidthSet)
				{
					if (track->f.sampleWidth < 1 || track->f.sampleWidth > 32)
					{
						_af_error(AF_BAD_WIDTH, "invalid sample width for WAVE file: %d (must be 1-32 bits)\n", track->f.sampleWidth);
						return AF_NULL_FILESETUP;
					}
					if (track->f.sampleWidth > 8)
					{
						_af_error(AF_BAD_SAMPFMT, "WAVE integer data of more than 8 bits must be two's complement signed");
						_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, track->f.sampleWidth);
					}
				}
				else
				/*
					If the sample width is not set but the user requests
					unsigned data, set the width to 8 bits.
				*/
					_af_set_sample_format(&track->f, track->f.sampleFormat, 8);
				break;

			case AF_SAMPFMT_TWOSCOMP:
				if (track->sampleWidthSet)
				{
					if (track->f.sampleWidth < 1 || track->f.sampleWidth > 32)
					{
						_af_error(AF_BAD_WIDTH, "invalid sample width %d for WAVE file (must be 1-32)", track->f.sampleWidth);
						return AF_NULL_FILESETUP;
					}
					else if (track->f.sampleWidth <= 8)
					{
						_af_error(AF_BAD_SAMPFMT, "Warning: WAVE format integer data of 1-8 bits must be unsigned; setting sample format to unsigned");
						_af_set_sample_format(&track->f, AF_SAMPFMT_UNSIGNED, track->f.sampleWidth);
					}
				}
				else
				/*
					If no sample width was specified, we default to 16 bits
					for signed integer data.
				*/
					_af_set_sample_format(&track->f, track->f.sampleFormat, 16);
				break;
		}
	}
	/*
		Otherwise set the sample format depending on the sample
		width or set completely to default.
	*/
	else
	{
		if (track->sampleWidthSet == AF_FALSE)
		{
			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
		}
		else
		{
			if (track->f.sampleWidth < 1 || track->f.sampleWidth > 32)
			{
				_af_error(AF_BAD_WIDTH, "invalid sample width %d for WAVE file (must be 1-32)", track->f.sampleWidth);
				return AF_NULL_FILESETUP;
			}
			else if (track->f.sampleWidth > 8)
				/* Here track->f.sampleWidth is in {1..32}. */
				track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			else
				/* Here track->f.sampleWidth is in {1..8}. */
				track->f.sampleFormat = AF_SAMPFMT_UNSIGNED;
		}
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		track->f.compressionType != AF_COMPRESSION_G711_ULAW &&
		track->f.compressionType != AF_COMPRESSION_G711_ALAW)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED, "compression format not supported in WAVE format");
		return AF_NULL_FILESETUP;
	}

	if (track->byteOrderSet &&
		track->f.byteOrder != AF_BYTEORDER_LITTLEENDIAN &&
		track->f.compressionType == AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_BYTEORDER, "WAVE format only supports little-endian data");
		return AF_NULL_FILESETUP;
	}

	if (track->f.compressionType == AF_COMPRESSION_NONE)
		track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;
	else
		track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;

	if (track->aesDataSet)
	{
		_af_error(AF_BAD_FILESETUP, "WAVE files cannot have AES data");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet)
	{
		if (setup->instrumentCount > 1)
		{
			_af_error(AF_BAD_NUMINSTS, "WAVE files can have 0 or 1 instrument");
			return AF_NULL_FILESETUP;
		}
		else if (setup->instrumentCount == 1)
		{
			if (setup->instruments[0].loopSet &&
				setup->instruments[0].loopCount > 0 &&
				(track->markersSet == AF_FALSE || track->markerCount == 0))
			{
				_af_error(AF_BAD_NUMMARKS, "WAVE files with loops must contain at least 1 marker");
				return AF_NULL_FILESETUP;
			}
		}
	}

	/* Make sure the miscellaneous data is of an acceptable type. */
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
				case AF_MISC_ICRD:
				case AF_MISC_ISFT:
				case AF_MISC_ICMT:
					break;
				default:
					_af_error(AF_BAD_MISCTYPE, "illegal miscellaneous type [%d] for WAVE file", setup->miscellaneous[i].type);
					return AF_NULL_FILESETUP;
			}
		}
	}

	/*
		Allocate an AFfilesetup and make all the unset fields correct.
	*/
	newsetup = _af_filesetup_copy(setup, &_af_wave_default_filesetup, AF_FALSE);

	/* Make sure we do not copy loops if they are not specified in setup. */
	if (setup->instrumentSet && setup->instrumentCount > 0 &&
		setup->instruments[0].loopSet)
	{
		free(newsetup->instruments[0].loops);
		newsetup->instruments[0].loopCount = 0;
	}

	return newsetup;
}

bool _af_wave_instparam_valid (AFfilehandle filehandle, AUpvlist list, int i)
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
			return AF_TRUE;

		default:
			return AF_FALSE;
			break;
	}

	return AF_TRUE;
}

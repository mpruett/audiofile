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
	wave.c

	This file contains code for parsing RIFF WAVE format sound files.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <math.h>
#include <stdint.h>
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

const int _af_wave_compression_types[_AF_WAVE_NUM_COMPTYPES] =
{
	AF_COMPRESSION_G711_ULAW,
	AF_COMPRESSION_G711_ALAW
};

const _InstParamInfo _af_wave_inst_params[_AF_WAVE_NUM_INSTPARAMS] =
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
	true,			/* trackSet */
	true,			/* instrumentSet */
	true,			/* miscellaneousSet  */
	1,			/* trackCount */
	NULL,			/* tracks */
	0,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

static status ParseFrameCount (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	uint32_t	totalFrames;
	_Track		*track;

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	af_read_uint32_le(&totalFrames, fp);

	track->totalfframes = totalFrames;

	return AF_SUCCEED;
}

static status ParseFormat (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	_Track		*track;
	uint16_t	formatTag, channelCount;
	uint32_t	sampleRate, averageBytesPerSecond;
	uint16_t	blockAlign;
	_WAVEInfo	*wave;

	assert(filehandle != NULL);
	assert(fp != NULL);
	assert(!memcmp(&id, "fmt ", 4));

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	assert(filehandle->formatSpecific != NULL);
	wave = (_WAVEInfo *) filehandle->formatSpecific;

	af_read_uint16_le(&formatTag, fp);
	af_read_uint16_le(&channelCount, fp);
	af_read_uint32_le(&sampleRate, fp);
	af_read_uint32_le(&averageBytesPerSecond, fp);
	af_read_uint16_le(&blockAlign, fp);

	track->f.channelCount = channelCount;
	track->f.sampleRate = sampleRate;
	track->f.byteOrder = AF_BYTEORDER_LITTLEENDIAN;

	/* Default to uncompressed audio data. */
	track->f.compressionType = AF_COMPRESSION_NONE;

	switch (formatTag)
	{
		case WAVE_FORMAT_PCM:
		{
			uint16_t	bitsPerSample;

			af_read_uint16_le(&bitsPerSample, fp);

			track->f.sampleWidth = bitsPerSample;

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

		case WAVE_FORMAT_IEEE_FLOAT:
		{
			uint16_t	bitsPerSample;

			af_read_uint16_le(&bitsPerSample, fp);

			if (bitsPerSample == 64)
			{
				track->f.sampleWidth = 64;
				track->f.sampleFormat = AF_SAMPFMT_DOUBLE;
			}
			else
			{
				track->f.sampleWidth = 32;
				track->f.sampleFormat = AF_SAMPFMT_FLOAT;
			}
		}
		break;

		case WAVE_FORMAT_ADPCM:
		{
			uint16_t	bitsPerSample, extraByteCount,
					samplesPerBlock, numCoefficients;
			int		i;
			AUpvlist	pv;
			long		l;
			void		*v;

			if (track->f.channelCount != 1 &&
				track->f.channelCount != 2)
			{
				_af_error(AF_BAD_CHANNELS,
					"WAVE file with MS ADPCM compression "
					"must have 1 or 2 channels");
			}

			af_read_uint16_le(&bitsPerSample, fp);
			af_read_uint16_le(&extraByteCount, fp);
			af_read_uint16_le(&samplesPerBlock, fp);
			af_read_uint16_le(&numCoefficients, fp);

			/* numCoefficients should be at least 7. */
			assert(numCoefficients >= 7 && numCoefficients <= 255);

			for (i=0; i<numCoefficients; i++)
			{
				int16_t	a0, a1;

				af_read_uint16_le(&a0, fp);
				af_read_uint16_le(&a1, fp);

				wave->msadpcmCoefficients[i][0] = a0;
				wave->msadpcmCoefficients[i][1] = a1;
			}

			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.compressionType = AF_COMPRESSION_MS_ADPCM;
			track->f.byteOrder = _AF_BYTEORDER_NATIVE;

			/* Create the parameter list. */
			pv = AUpvnew(4);
			AUpvsetparam(pv, 0, _AF_MS_ADPCM_NUM_COEFFICIENTS);
			AUpvsetvaltype(pv, 0, AU_PVTYPE_LONG);
			l = numCoefficients;
			AUpvsetval(pv, 0, &l);

			AUpvsetparam(pv, 1, _AF_MS_ADPCM_COEFFICIENTS);
			AUpvsetvaltype(pv, 1, AU_PVTYPE_PTR);
			v = wave->msadpcmCoefficients;
			AUpvsetval(pv, 1, &v);

			AUpvsetparam(pv, 2, _AF_FRAMES_PER_BLOCK);
			AUpvsetvaltype(pv, 2, AU_PVTYPE_LONG);
			l = samplesPerBlock;
			AUpvsetval(pv, 2, &l);

			AUpvsetparam(pv, 3, _AF_BLOCK_SIZE);
			AUpvsetvaltype(pv, 3, AU_PVTYPE_LONG);
			l = blockAlign;
			AUpvsetval(pv, 3, &l);

			track->f.compressionParams = pv;
		}
		break;

		case WAVE_FORMAT_DVI_ADPCM:
		{
			AUpvlist	pv;
			long		l;

			uint16_t	bitsPerSample, extraByteCount,
					samplesPerBlock;

			af_read_uint16_le(&bitsPerSample, fp);
			af_read_uint16_le(&extraByteCount, fp);
			af_read_uint16_le(&samplesPerBlock, fp);

			if (bitsPerSample != 4)
			{
				_af_error(AF_BAD_NOT_IMPLEMENTED,
					"IMA ADPCM compression supports only 4 bits per sample");
			}

			int bytesPerBlock = (samplesPerBlock + 14) / 8 * 4 * channelCount;
			if (bytesPerBlock > blockAlign || (samplesPerBlock % 8) != 1)
			{
				_af_error(AF_BAD_CODEC_CONFIG,
					"Invalid samples per block for IMA ADPCM compression");
			}

			track->f.sampleWidth = 16;
			track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
			track->f.compressionType = AF_COMPRESSION_IMA;
			track->f.byteOrder = _AF_BYTEORDER_NATIVE;

			/* Create the parameter list. */
			pv = AUpvnew(2);
			AUpvsetparam(pv, 0, _AF_FRAMES_PER_BLOCK);
			AUpvsetvaltype(pv, 0, AU_PVTYPE_LONG);
			l = samplesPerBlock;
			AUpvsetval(pv, 0, &l);

			AUpvsetparam(pv, 1, _AF_BLOCK_SIZE);
			AUpvsetvaltype(pv, 1, AU_PVTYPE_LONG);
			l = blockAlign;
			AUpvsetval(pv, 1, &l);

			track->f.compressionParams = pv;
		}
		break;

		case WAVE_FORMAT_YAMAHA_ADPCM:
		case WAVE_FORMAT_OKI_ADPCM:
		case WAVE_FORMAT_CREATIVE_ADPCM:
		case IBM_FORMAT_ADPCM:
			_af_error(AF_BAD_NOT_IMPLEMENTED, "WAVE ADPCM data format 0x%x is not currently supported", formatTag);
			return AF_FAIL;
			break;

		case WAVE_FORMAT_MPEG:
			_af_error(AF_BAD_NOT_IMPLEMENTED, "WAVE MPEG data format is not supported");
			return AF_FAIL;
			break;

		case WAVE_FORMAT_MPEGLAYER3:
			_af_error(AF_BAD_NOT_IMPLEMENTED, "WAVE MPEG layer 3 data format is not supported");
			return AF_FAIL;
			break;

		default:
			_af_error(AF_BAD_NOT_IMPLEMENTED, "WAVE file data format 0x%x not currently supported", formatTag);
			return AF_FAIL;
			break;
	}

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	return AF_SUCCEED;
}

static status ParseData (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
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

static status ParsePlayList (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	_Instrument	*instrument;
	uint32_t	segmentCount;
	int		segment;

	af_read_uint32_le(&segmentCount, fp);

	if (segmentCount == 0)
	{
		filehandle->instrumentCount = 0;
		filehandle->instruments = NULL;
		return AF_SUCCEED;
	}

	for (segment=0; segment<segmentCount; segment++)
	{
		uint32_t	startMarkID, loopLength, loopCount;

		af_read_uint32_le(&startMarkID, fp);
		af_read_uint32_le(&loopLength, fp);
		af_read_uint32_le(&loopCount, fp);
	}

	return AF_SUCCEED;
}

static status ParseCues (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	_Track		*track;
	uint32_t	markerCount;
	int		i;

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	af_read_uint32_le(&markerCount, fp);
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
		uint32_t	id, position, chunkid;
		uint32_t	chunkByteOffset, blockByteOffset;
		uint32_t	sampleFrameOffset;
		_Marker		*marker = &track->markers[i];

		af_read_uint32_le(&id, fp);
		af_read_uint32_le(&position, fp);
		af_read_uint32_le(&chunkid, fp);
		af_read_uint32_le(&chunkByteOffset, fp);
		af_read_uint32_le(&blockByteOffset, fp);

		/*
			sampleFrameOffset represents the position of
			the mark in units of frames.
		*/
		af_read_uint32_le(&sampleFrameOffset, fp);

		marker->id = id;
		marker->position = sampleFrameOffset;
		marker->name = _af_strdup("");
		marker->comment = _af_strdup("");
	}

	return AF_SUCCEED;
}

/* Parse an adtl sub-chunk within a LIST chunk. */
static status ParseADTLSubChunk (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	_Track		*track;
	AFfileoffset	endPos=af_ftell(fp)+size;

	track = _af_filehandle_get_track(filehandle, AF_DEFAULT_TRACK);

	while (af_ftell(fp) < endPos)
	{
		char		chunkID[4];
		uint32_t	chunkSize;

		af_fread(chunkID, 4, 1, fp);
		af_read_uint32_le(&chunkSize, fp);

		if (memcmp(chunkID, "labl", 4)==0 || memcmp(chunkID, "note", 4)==0)
		{
			_Marker *marker=NULL;
			uint32_t id;
			long length=chunkSize-4;
			char *p=_af_malloc(length);

			af_read_uint32_le(&id, fp);
			af_fread(p, length, 1, fp);

			marker = _af_marker_find_by_id(track, id);

			if (marker != NULL)
			{
				if (memcmp(chunkID, "labl", 4)==0)
				{
					free(marker->name);
					marker->name = p;
				}
				else if (memcmp(chunkID, "note", 4)==0)
				{
					free(marker->comment);
					marker->comment = p;
				}
				else
					free(p);
			}
			else
				free(p);

			/*
				If chunkSize is odd, skip an extra byte
				at the end of the chunk.
			*/
			if ((chunkSize % 2) != 0)
				af_fseek(fp, 1, SEEK_CUR);
		}
		else
		{
			/* If chunkSize is odd, skip an extra byte. */
			af_fseek(fp, chunkSize + (chunkSize % 2), SEEK_CUR);
		}
	}
	return AF_SUCCEED;
}

/* Parse an INFO sub-chunk within a LIST chunk. */
static status ParseINFOSubChunk (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	AFfileoffset	endPos=af_ftell(fp)+size;

	while (af_ftell(fp) < endPos)
	{
		int		misctype = AF_MISC_UNRECOGNIZED;
		uint32_t	miscid, miscsize;

		af_fread(&miscid, 4, 1, fp);
		af_read_uint32_le(&miscsize, fp);

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

			filehandle->miscellaneousCount++;
			filehandle->miscellaneous = _af_realloc(filehandle->miscellaneous, sizeof (_Miscellaneous) * filehandle->miscellaneousCount);

			filehandle->miscellaneous[filehandle->miscellaneousCount-1].id = filehandle->miscellaneousCount;
			filehandle->miscellaneous[filehandle->miscellaneousCount-1].type = misctype;
			filehandle->miscellaneous[filehandle->miscellaneousCount-1].size = miscsize;
			filehandle->miscellaneous[filehandle->miscellaneousCount-1].position = 0;
			filehandle->miscellaneous[filehandle->miscellaneousCount-1].buffer = string;
		}
		else
		{
			af_fseek(fp, miscsize, SEEK_CUR);
		}

		/* Make the current position an even number of bytes.  */
		if (miscsize % 2 != 0)
			af_fseek(fp, 1, SEEK_CUR);
	}
	return AF_SUCCEED;
}

static status ParseList (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	uint32_t	typeID;

	af_fread(&typeID, 4, 1, fp);
	size-=4;

	if (memcmp(&typeID, "adtl", 4) == 0)
	{
		/* Handle adtl sub-chunks. */
		return ParseADTLSubChunk(filehandle, fp, typeID, size);
	}
	else if (memcmp(&typeID, "INFO", 4) == 0)
	{
		/* Handle INFO sub-chunks. */
		return ParseINFOSubChunk(filehandle, fp, typeID, size);
	}
	else
	{
		/* Skip unhandled sub-chunks. */
		af_fseek(fp, size, SEEK_CUR);
		return AF_SUCCEED;
	}
	return AF_SUCCEED;
}

static status ParseInstrument (AFfilehandle filehandle, AFvirtualfile *fp,
	uint32_t id, size_t size)
{
	uint8_t	baseNote;
	int8_t	detune, gain;
	uint8_t	lowNote, highNote, lowVelocity, highVelocity;
	uint8_t	padByte;

	af_read_uint8(&baseNote, fp);
	af_read_uint8(&detune, fp);
	af_read_uint8(&gain, fp);
	af_read_uint8(&lowNote, fp);
	af_read_uint8(&highNote, fp);
	af_read_uint8(&lowVelocity, fp);
	af_read_uint8(&highVelocity, fp);
	af_read_uint8(&padByte, fp);

	return AF_SUCCEED;
}

bool _af_wave_recognize (AFvirtualfile *fh)
{
	uint8_t	buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_fread(buffer, 1, 8, fh) != 8 || memcmp(buffer, "RIFF", 4) != 0)
		return false;
	if (af_fread(buffer, 1, 4, fh) != 4 || memcmp(buffer, "WAVE", 4) != 0)
		return false;

	return true;
}

status _af_wave_read_init (AFfilesetup setup, AFfilehandle filehandle)
{
	_Track		*track;
	uint32_t	type, size, formtype;
	uint32_t	index = 0;
	bool		hasFormat, hasData, hasCue, hasList, hasPlayList, hasFrameCount,
			hasINST, hasINFO;
	_WAVEInfo	*wave = _af_malloc(sizeof (_WAVEInfo));

	assert(filehandle != NULL);
	assert(filehandle->fh != NULL);

	hasFormat = false;
	hasData = false;
	hasCue = false;
	hasList = false;
	hasPlayList = false;
	hasFrameCount = false;
	hasINST = false;
	hasINFO = false;

	filehandle->formatSpecific = wave;
	filehandle->instruments = NULL;
	filehandle->instrumentCount = 0;
	filehandle->miscellaneous = NULL;
	filehandle->miscellaneousCount = 0;

	track = _af_track_new();
	filehandle->tracks = track;
	filehandle->trackCount = 1;

	af_fseek(filehandle->fh, 0, SEEK_SET);

	af_fread(&type, 4, 1, filehandle->fh);
	af_read_uint32_le(&size, filehandle->fh);
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
		uint32_t	chunkid = 0, chunksize = 0;
		status		result;

#ifdef DEBUG
		printf("index: %d\n", index);
#endif
		af_fread(&chunkid, 4, 1, filehandle->fh);
		af_read_uint32_le(&chunksize, filehandle->fh);

#ifdef DEBUG
		_af_printid(BENDIAN_TO_HOST_INT32(chunkid));
		printf(" size: %d\n", chunksize);
#endif

		if (memcmp(&chunkid, "fmt ", 4) == 0)
		{
			result = ParseFormat(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;

			hasFormat = true;
		}
		else if (memcmp(&chunkid, "data", 4) == 0)
		{
			/* The format chunk must precede the data chunk. */
			if (!hasFormat)
			{
				_af_error(AF_BAD_HEADER, "missing format chunk in WAVE file");
				return AF_FAIL;
			}

			result = ParseData(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;

			hasData = true;
		}
		else if (memcmp(&chunkid, "inst", 4) == 0)
		{
			result = ParseInstrument(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "fact", 4) == 0)
		{
			hasFrameCount = true;
			result = ParseFrameCount(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "cue ", 4) == 0)
		{
			hasCue = true;
			result = ParseCues(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "LIST", 4) == 0 || memcmp(&chunkid, "list", 4) == 0)
		{
			hasList = true;
			result = ParseList(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "INST", 4) == 0)
		{
			hasINST = true;
			result = ParseInstrument(filehandle, filehandle->fh, chunkid, chunksize);
			if (result == AF_FAIL)
				return AF_FAIL;
		}
		else if (memcmp(&chunkid, "plst", 4) == 0)
		{
			hasPlayList = true;
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
	{
		return AF_FAIL;
	}

	/*
		At this point we know that the file has a format chunk
		and a data chunk, so we can assume that track->f and
		track->data_size have been initialized.
	*/
	if (!hasFrameCount)
	{
		track->totalfframes = track->data_size /
			(int) _af_format_frame_size(&track->f, false);
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE &&
		(track->f.compressionType == AF_COMPRESSION_G711_ULAW ||
		track->f.compressionType == AF_COMPRESSION_G711_ALAW))
	{
		track->totalfframes = track->data_size / track->f.channelCount;
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
				if (track->sampleWidthSet &&
					track->f.sampleWidth != 32)
				{
					_af_error(AF_BAD_WIDTH,
						"Warning: invalid sample width for floating-point WAVE file: %d (must be 32 bits)\n",
						track->f.sampleWidth);
					_af_set_sample_format(&track->f, AF_SAMPFMT_FLOAT, 32);
				}
				break;

			case AF_SAMPFMT_DOUBLE:
				if (track->sampleWidthSet &&
					track->f.sampleWidth != 64)
				{
					_af_error(AF_BAD_WIDTH,
						"Warning: invalid sample width for double-precision floating-point WAVE file: %d (must be 64 bits)\n",
						track->f.sampleWidth);
					_af_set_sample_format(&track->f, AF_SAMPFMT_DOUBLE, 64);
				}
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
		if (!track->sampleWidthSet)
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
				(!track->markersSet || track->markerCount == 0))
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
	newsetup = _af_filesetup_copy(setup, &_af_wave_default_filesetup, false);

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
			return true;

		default:
			return false;
	}

	return true;
}

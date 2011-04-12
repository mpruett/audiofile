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
	IFF.cpp

	This file contains routines for reading and writing IFF/8SVX
	sound files.
*/

#include "config.h"
#include "IFF.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Marker.h"
#include "Setup.h"
#include "Track.h"
#include "af_vfs.h"
#include "afinternal.h"
#include "audiofile.h"
#include "byteorder.h"
#include "util.h"

static _AFfilesetup iff_default_filesetup =
{
	_AF_VALID_FILESETUP,	/* valid */
	AF_FILE_IFF_8SVX,	/* fileFormat */
	true,			/* trackSet */
	true,			/* instrumentSet */
	true,			/* miscellaneousSet */
	1,			/* trackCount */
	NULL,			/* tracks */
	0,			/* instrumentCount */
	NULL,			/* instruments */
	0,			/* miscellaneousCount */
	NULL			/* miscellaneous */
};

bool IFFFile::recognize(File *fh)
{
	uint8_t buffer[8];

	af_fseek(fh, 0, SEEK_SET);

	if (af_read(buffer, 8, fh) != 8 || memcmp(buffer, "FORM", 4) != 0)
		return false;
	if (af_read(buffer, 4, fh) != 4 || memcmp(buffer, "8SVX", 4) != 0)
		return false;

	return true;
}

IFFFile::IFFFile()
{
	setFormatByteOrder(AF_BYTEORDER_BIGENDIAN);

	miscellaneousPosition = 0;
	VHDR_offset = 0;
	BODY_offset = 0;
}

/*
	Parse miscellaneous data chunks such as name, author, copyright,
	and annotation chunks.
*/
status IFFFile::parseMiscellaneous(uint32_t type, size_t size)
{
	int misctype = AF_MISC_UNRECOGNIZED;

	assert(!memcmp(&type, "NAME", 4) || !memcmp(&type, "AUTH", 4) ||
		!memcmp(&type, "(c) ", 4) || !memcmp(&type, "ANNO", 4));

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
	Parse voice header chunk.
*/
status IFFFile::parseVHDR(uint32_t type, size_t size)
{
	assert(!memcmp(&type, "VHDR", 4));

	Track *track = getTrack();

	uint32_t oneShotSamples, repeatSamples, samplesPerRepeat;
	uint16_t sampleRate;
	uint8_t octaves, compression;
	uint32_t volume;

	readU32(&oneShotSamples);
	readU32(&repeatSamples);
	readU32(&samplesPerRepeat);
	readU16(&sampleRate);
	readU8(&octaves);
	readU8(&compression);
	readU32(&volume);

	track->f.sampleWidth = 8; 
	track->f.sampleRate = sampleRate;
	track->f.sampleFormat = AF_SAMPFMT_TWOSCOMP;
	track->f.compressionType = AF_COMPRESSION_NONE;
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
	track->f.channelCount = 1;

	_af_set_sample_format(&track->f, track->f.sampleFormat, track->f.sampleWidth);

	return AF_SUCCEED;
}

status IFFFile::parseBODY(uint32_t type, size_t size)
{
	Track *track = getTrack();

	/*
		IFF/8SVX files have only one audio channel with one
		byte per sample, so the number of frames is equal to
		the number of bytes.
	*/
	track->totalfframes = size;
	track->data_size = size;

	/* Sound data follows. */
	track->fpos_first_frame = af_ftell(fh);

	return AF_SUCCEED;
}

status IFFFile::readInit(AFfilesetup setup)
{
	uint32_t type, size, formtype;

	af_fseek(fh, 0, SEEK_SET);

	af_read(&type, 4, fh);
	readU32(&size);
	af_read(&formtype, 4, fh);

	if (memcmp(&type, "FORM", 4) != 0 || memcmp(&formtype, "8SVX", 4) != 0)
		return AF_FAIL;

	instrumentCount = 0;
	instruments = NULL;
	miscellaneousCount = 0;
	miscellaneous = NULL;

	/* IFF/8SVX files have only one track. */
	Track *track = _af_track_new();
	trackCount = 1;
	tracks = track;

	/* Set the index to include the form type ('8SVX' in this case). */
	size_t index = 4;
	while (index < size)
	{
		uint32_t chunkid = 0, chunksize = 0;
		status result = AF_SUCCEED;

		af_read(&chunkid, 4, fh);
		readU32(&chunksize);

		if (!memcmp("VHDR", &chunkid, 4))
		{
			result = parseVHDR(chunkid, chunksize);
		}
		else if (!memcmp("BODY", &chunkid, 4))
		{
			result = parseBODY(chunkid, chunksize);
		}
		else if (!memcmp("NAME", &chunkid, 4) ||
			!memcmp("AUTH", &chunkid, 4) ||
			!memcmp("(c) ", &chunkid, 4) ||
			!memcmp("ANNO", &chunkid, 4))
		{
			parseMiscellaneous(chunkid, chunksize);
		}

		if (result == AF_FAIL)
			return AF_FAIL;

		/*
			Increment the index by the size of the chunk
			plus the size of the chunk header.
		*/
		index += chunksize + 8;

		/* All chunks must be aligned on an even number of bytes. */
		if ((index % 2) != 0)
			index++;

		/* Set the seek position to the beginning of the next chunk. */
		af_fseek(fh, index + 8, SEEK_SET);
	}

	/* The file has been successfully parsed. */
	return AF_SUCCEED;
}

AFfilesetup IFFFile::completeSetup(AFfilesetup setup)
{
	if (setup->trackSet && setup->trackCount != 1)
	{
		_af_error(AF_BAD_NUMTRACKS, "IFF/8SVX file must have 1 track");
		return AF_NULL_FILESETUP;
	}

	TrackSetup *track = &setup->tracks[0];

	if (track->sampleFormatSet &&
		track->f.sampleFormat != AF_SAMPFMT_TWOSCOMP)
	{
		_af_error(AF_BAD_SAMPFMT,
			"IFF/8SVX format supports only two's complement integer data");
		return AF_NULL_FILESETUP;
	}

	if (track->sampleFormatSet && track->f.sampleWidth != 8)
	{
		_af_error(AF_BAD_WIDTH,
			"IFF/8SVX file allows only 8 bits per sample "
			"(%d bits requested)", track->f.sampleWidth);
		return AF_NULL_FILESETUP;
	}

	if (track->channelCountSet && track->f.channelCount != 1)
	{
		_af_error(AF_BAD_CHANNELS,
			"invalid channel count (%d) for IFF/8SVX format "
			"(only 1 channel supported)",
			track->f.channelCount);
		return AF_NULL_FILESETUP;
	}

	if (track->f.compressionType != AF_COMPRESSION_NONE)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IFF/8SVX does not support compression");
		return AF_NULL_FILESETUP;
	}

	/* Ignore requested byte order since samples are only one byte. */
	track->f.byteOrder = AF_BYTEORDER_BIGENDIAN;
	/* Either one channel was requested or no request was made. */
	track->f.channelCount = 1;
	_af_set_sample_format(&track->f, AF_SAMPFMT_TWOSCOMP, 8);

	if (track->markersSet && track->markerCount != 0)
	{
		_af_error(AF_BAD_NUMMARKS,
			"IFF/8SVX format does not support markers");
		return AF_NULL_FILESETUP;
	}

	if (setup->instrumentSet && setup->instrumentCount != 0)
	{
		_af_error(AF_BAD_NUMINSTS,
			"IFF/8SVX format does not support instruments");
		return AF_NULL_FILESETUP;
	}

	return _af_filesetup_copy(setup, &iff_default_filesetup, true);
}

status IFFFile::writeInit(AFfilesetup setup)
{
	uint32_t fileSize = 0;

	if (_af_filesetup_make_handle(setup, this) == AF_FAIL)
		return AF_FAIL;

	af_write("FORM", 4, fh);
	writeU32(&fileSize);

	af_write("8SVX", 4, fh);

	writeVHDR();
	writeMiscellaneous();
	writeBODY();

	return AF_SUCCEED;
}

status IFFFile::update()
{
	uint32_t length;

	writeVHDR();
	writeMiscellaneous();
	writeBODY();

	/* Get the length of the file. */
	length = af_flength(fh);
	length -= 8;

	/* Set the length of the FORM chunk. */
	af_fseek(fh, 4, SEEK_SET);
	writeU32(&length);

	return AF_SUCCEED;
}

status IFFFile::writeVHDR()
{
	uint32_t chunkSize;
	uint32_t oneShotSamples, repeatSamples, samplesPerRepeat;
	uint16_t sampleRate;
	uint8_t octaves, compression;
	uint32_t volume;

	/*
		If VHDR_offset hasn't been set yet, set it to the
		current offset.
	*/
	if (VHDR_offset == 0)
		VHDR_offset = af_ftell(fh);
	else
		af_fseek(fh, VHDR_offset, SEEK_SET);

	Track *track = getTrack();

	af_write("VHDR", 4, fh);

	chunkSize = 20;
	writeU32(&chunkSize);

	/*
		IFF/8SVX files have only one audio channel, so the
		number of samples is equal to the number of frames.
	*/
	oneShotSamples = track->totalfframes;
	writeU32(&oneShotSamples);
	repeatSamples = 0;
	writeU32(&repeatSamples);
	samplesPerRepeat = 0;
	writeU32(&samplesPerRepeat);

	sampleRate = track->f.sampleRate;
	writeU16(&sampleRate);

	octaves = 0;
	compression = 0;
	writeU8(&octaves);
	writeU8(&compression);

	/* Volume is in fixed-point notation; 65536 means gain of 1.0. */
	volume = 65536;
	writeU32(&volume);

	return AF_SUCCEED;
}

status IFFFile::writeBODY()
{
	uint32_t chunkSize;

	Track *track = getTrack();

	if (BODY_offset == 0)
		BODY_offset = af_ftell(fh);
	else
		af_fseek(fh, BODY_offset, SEEK_SET);

	af_write("BODY", 4, fh);

	/*
		IFF/8SVX supports only one channel, so the number of
		frames is equal to the number of samples, and each
		sample is one byte.
	*/
	chunkSize = track->totalfframes;
	writeU32(&chunkSize);

	if (track->fpos_first_frame == 0)
		track->fpos_first_frame = af_ftell(fh);

	/* Add a pad byte to the end of the chunk if the chunk size is odd. */
	if ((chunkSize % 2) == 1)
	{
		uint8_t zero = 0;
		af_fseek(fh, BODY_offset + 8 + chunkSize, SEEK_SET);
		writeU8(&zero);
	}

	return AF_SUCCEED;
}

/*
	WriteMiscellaneous writes all the miscellaneous data chunks in a
	file handle structure to an IFF/8SVX file.
*/
status IFFFile::writeMiscellaneous()
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

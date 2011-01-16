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
	IFFWrite.cpp

	This file contains routines for writing IFF/8SVX format sound
	files.
*/

#include "config.h"
#include "IFF.h"

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

status _af_iff_update (AFfilehandle file);

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

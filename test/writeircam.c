/*
	Audio File Library

	Copyright 1998-1999, Michael Pruett <michael@68k.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this program; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
	MA 02111-1307, USA.
*/

/*
	writeircam.c

	This program tests the validity of the IRCAM/BICSF format
	reading and writing code.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define TEST_FILE "/tmp/test.au"

void cleanup (void)
{
#ifndef DEBUG
	unlink(TEST_FILE);
#endif
}

void ensure (int condition, const char *message)
{
	if (!condition)
	{
		printf("%s.\n", message);
		cleanup();
		exit(-1);
	}
}

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFfilesetup	setup;
	u_int16_t	samples[] = {11, 51, 101, 501, 1001, 5001, 10001, 50001};
	int		i;
	int		sampleFormat, sampleWidth;
	int		framesRead, framesWritten;

	setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_IRCAM);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);

	file = afOpenFile(TEST_FILE, "w", setup);
	ensure(file != AF_NULL_FILEHANDLE, "unable to open file for writing");

	framesWritten = afWriteFrames(file, AF_DEFAULT_TRACK, samples, 8);
	ensure(framesWritten == 8,
		"number of frames written does not match number of frames requested");

	ensure(afCloseFile(file) == 0, "error closing file");
	afFreeFileSetup(setup);

	file = afOpenFile(TEST_FILE, "r", NULL);
	ensure(file != AF_NULL_FILEHANDLE, "unable to open file for reading");

	ensure(afGetFileFormat(file, NULL) == AF_FILE_IRCAM,
		"test file not created as IRCAM/BICSF");

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	ensure(sampleFormat == AF_SAMPFMT_TWOSCOMP,
		"test file not two's complement");
	ensure(sampleWidth == 16,
		"test file sample format is not 16-bit");

	ensure(afGetChannels(file, AF_DEFAULT_TRACK) == 1,
		"test file doesn't have exactly one channel");

#ifdef WORDS_BIGENDIAN
	ensure(afGetByteOrder(file, AF_DEFAULT_TRACK) == AF_BYTEORDER_BIGENDIAN,
		"test file not big-endian");
#else
	ensure(afGetByteOrder(file, AF_DEFAULT_TRACK) == AF_BYTEORDER_LITTLEENDIAN,
		"test file not little-endian");
#endif

	for (i=0; i<8; i++)
	{
		u_int16_t	temporary;

		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, &temporary, 1);
		ensure(framesRead == 1,
			"number of frames read does not match number of frames requested");

		ensure(temporary == samples[i],
			"data written to file doesn't match data read from file");
	}

	ensure(afCloseFile(file) == 0, "error closing file");

	cleanup();

	printf("writeircam test passed.\n");

	exit(0);
}

/*
	Audio File Library

	Copyright (C) 2003, Michael Pruett <michael@68k.org>

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
	seek.c

	This program tests seeking within an audio file.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <audiofile.h>

#define TEST_FILE "/tmp/test.aiff"
#define FRAME_COUNT 2000
#define PAD_FRAME_COUNT (FRAME_COUNT + 5)
#define DATA_LENGTH (FRAME_COUNT * sizeof (short))

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
		exit(EXIT_FAILURE);
	}
}

int main (int argc, char **argv)
{
	AFfilehandle file;
	AFfilesetup setup;
	AFframecount framesread;
	short data[FRAME_COUNT];
	short readdata[PAD_FRAME_COUNT];
	int i;

	setup = afNewFileSetup();
	ensure(setup != NULL, "could not create file setup");

	afInitFileFormat(setup, AF_FILE_AIFF);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);

	file = afOpenFile(TEST_FILE, "w", setup);
	ensure(file != AF_NULL_FILEHANDLE, "could not open file for writing");

	afFreeFileSetup(setup);

	/* Initialize data to a nontrivial test pattern. */
	for (i=0; i<FRAME_COUNT; i++)
	{
		if ((i%2) != 0)
			data[i] = i;
		else
			data[i] = -i;
	}

	afWriteFrames(file, AF_DEFAULT_TRACK, data, FRAME_COUNT);

	afCloseFile(file);

	file = afOpenFile(TEST_FILE, "r", AF_NULL_FILESETUP);
	ensure(file != AF_NULL_FILEHANDLE, "could not open file for reading");

	/*
		For each position in the file, seek to that position and
		read to the end of the file, checking that the data read
		matches the data written.
	*/
	for (i=0; i<FRAME_COUNT; i++)
	{
		AFfileoffset	currentposition;

		memset(readdata, 0, DATA_LENGTH);

		afSeekFrame(file, AF_DEFAULT_TRACK, i);
		currentposition = afTellFrame(file, AF_DEFAULT_TRACK);
		ensure(currentposition == i, "incorrect seek position");

		framesread = afReadFrames(file, AF_DEFAULT_TRACK, readdata + i,
			PAD_FRAME_COUNT);
		ensure(framesread == FRAME_COUNT - i,
			"incorrect number of frames read");

		ensure(memcmp(data + i, readdata + i, framesread * sizeof (short)) == 0,
			"error in data read");
	}

	afCloseFile(file);

	cleanup();
	return 0;
}

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

#include <audiofile.h>

#define TEST_FILE "/tmp/test.aiff"
#define FRAME_COUNT 2000
#define PAD_FRAME_COUNT (FRAME_COUNT + 5)
#define DATA_LENGTH (FRAME_COUNT * sizeof (short))

int main (int argc, char **argv)
{
	AFfilehandle file;
	AFfilesetup setup;
	AFframecount framesread;
	short data[FRAME_COUNT];
	short readdata[PAD_FRAME_COUNT];
	int i;

	setup = afNewFileSetup();
	if (setup == NULL)
	{
		fprintf(stderr, "could not create file setup\n");
		exit(EXIT_FAILURE);
	}

	afInitFileFormat(setup, AF_FILE_AIFF);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);

	file = afOpenFile(TEST_FILE, "w", setup);
	if (file == AF_NULL_FILEHANDLE)
	{
		fprintf(stderr, "could not open file for writing\n");
		exit(EXIT_FAILURE);
	}

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
	if (file == AF_NULL_FILEHANDLE)
	{
		fprintf(stderr, "could not open file for reading\n");
		exit(EXIT_FAILURE);
	}

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
		if (currentposition != i)
		{
			fprintf(stderr, "incorrect seek position\n");
			exit(EXIT_FAILURE);
		}

		framesread = afReadFrames(file, AF_DEFAULT_TRACK, readdata + i,
			PAD_FRAME_COUNT);
		if (framesread != FRAME_COUNT - i)
		{
			fprintf(stderr, "incorrect number of frames read\n");
			exit(EXIT_FAILURE);
		}

		if (memcmp(data + i, readdata + i, framesread * sizeof (short)) != 0)
		{
			fprintf(stderr, "error in data read\n");
			exit(EXIT_FAILURE);
		}
	}

	afCloseFile(file);

	return 0;
}

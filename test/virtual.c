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
	virtual.c
*/

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#define TEST_FILE "/tmp/test"

#include <stdio.h>

void usage (void)
{
	fprintf(stderr, "usage: virtual filename\n");
	exit(-1);
}

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFfilesetup	setup;
	short		frames[] = {2,291,-49,20,40,29};
	short		buffer[6] = {1, 2, 3};
	AFframecount	result;

	setup = afNewFileSetup();
	afInitChannels(setup, AF_DEFAULT_TRACK, 2);
	file = afOpenFile(TEST_FILE, "w", setup);
	if (file == AF_NULL_FILEHANDLE)
	{
		fprintf(stderr, "could not open file for reading");
		exit(-1);
	}
	afFreeFileSetup(setup);

	result = afWriteFrames(file, AF_DEFAULT_TRACK, frames, 3);
	if (result != 3)
	{
		fprintf(stderr, "wrote %d frames: expected 3\n", result);
		exit(-1);
	}

	afCloseFile(file);

	file = afOpenFile(TEST_FILE, "r", AF_NULL_FILESETUP);
	if (file == AF_NULL_FILEHANDLE)
	{
		fprintf(stderr, "could not open file for reading");
		exit(-1);
	}

	afSetVirtualChannels(file, AF_DEFAULT_TRACK, 1);

	result = afReadFrames(file, AF_DEFAULT_TRACK, buffer, 3);
	if (result != 3)
	{
		fprintf(stderr, "wrote %d frames: expected 3\n", result);
		exit(-1);
	}

	printf("buffer: %d %d %d\n",
		buffer[0],
		buffer[1],
		buffer[2]);

	afCloseFile(file);
}

/*
	Audio File Library

	Copyright 1998, Michael Pruett <michael@68k.org>

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
	copy.c

	This program uses the Audio File Library to convert AIFF files to WAVE
	files.
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

int main (int ac, char **av)
{
	AFframecount	frameCount;
	AFfilehandle	inputFile, outputFile;
	AFfilesetup		outputSetup;
	float			frameSize;
	void			*buffer;
	int format = 0;

	if (ac < 3)
		exit(-1);
	if (ac >= 5)
	{
		if (!strcmp(av[4], "wave"))
			format = AF_FILE_WAVE;
		else if (!strcmp(av[4], "aiff"))
			format = AF_FILE_AIFF;
	}

	inputFile = afOpenFile(av[1], "r", NULL);

	frameCount = afGetFrameCount(inputFile, AF_DEFAULT_TRACK);
	frameSize = afGetFrameSize(inputFile, AF_DEFAULT_TRACK, 0);

	printf("frameSize: %f\n", frameSize);

	buffer = malloc(frameCount * frameSize);
	afReadFrames(inputFile, AF_DEFAULT_TRACK, buffer, frameCount);
	afCloseFile(inputFile);

	outputSetup = afNewFileSetup();
	afInitFileFormat(outputSetup, format);
	afInitRate(outputSetup, AF_DEFAULT_TRACK, 44100);
	afInitChannels(outputSetup, AF_DEFAULT_TRACK, 2);
	outputFile = afOpenFile(av[2], "w", outputSetup);
	afFreeFileSetup(outputSetup);

	afSetVirtualByteOrder(outputFile, AF_DEFAULT_TRACK, AF_BYTEORDER_BIGENDIAN);

	afWriteFrames(outputFile, AF_DEFAULT_TRACK, buffer, frameCount);
	afCloseFile(outputFile);

	return 0;
}

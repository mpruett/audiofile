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
	irixtest.c

	This program reads and plays a given audio file using Irix's
	default audio output device.  This file will not work on any
	operating system other than Irix.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dmedia/audio.h>
#include <dmedia/audiofile.h>

#include "sgi.h"

void usage (void)
{
	fprintf(stderr, "usage: irixtest filename\n");
	exit(-1);
}

main (int argc, char **argv)
{
	AFfilehandle	file;
	AFframecount	frameCount;
	int				sampleFormat, sampleWidth, channelCount, frameSize;
	double			sampleRate;
	char			*buffer;

	ALport			outport;
	ALconfig		outportconfig;

	if (argc < 2)
		usage();

	file = afOpenFile(argv[1], "r", NULL);

	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	printf("frame count: %d\n", frameCount);

	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 1);
	printf("frame size: %d\n", frameSize);

	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);
	printf("channel count: %d\n", channelCount);

	sampleRate = afGetRate(file, AF_DEFAULT_TRACK);
	printf("sample rate: %.2f\n", sampleRate);

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	/* Allocate memory to store the samples and read them from the file. */
	buffer = (char *) malloc(frameCount * frameSize);
	afReadFrames(file, AF_DEFAULT_TRACK, buffer, frameCount);

	afCloseFile(file);

	outportconfig = alNewConfig();
	setwidth(outportconfig, sampleWidth);
	alSetChannels(outportconfig, channelCount);

	outport = alOpenPort("dick", "w", outportconfig);
	setrate(outport, sampleRate);

	alWriteFrames(outport, buffer, frameCount);

	waitport(outport);

	alClosePort(outport);
	alFreeConfig(outportconfig);

	free(buffer);
}

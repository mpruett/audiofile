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
	irixread.c

	This program reads and plays a given audio file using Irix's
	default audio output device.  This file will not work on any
	operating system other than Irix.

	The only difference between this program and irixtest is that this
	program does not load the entire audio track into memory at once.
	Only a small number of frames are read into a buffer and then
	written to the audio port.  While there are more frames to be
	read, this process is repeated.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dmedia/audio.h>
#include <dmedia/audiofile.h>

#include "sgi.h"

const int BUFFERED_FRAME_COUNT = 65536;

void usage (void)
{
	fprintf(stderr, "usage: irixread filename\n");
	exit(-1);
}

main (int ac, char **av)
{
	AFfilehandle	file;
	AFframecount	count, frameCount;
	int				frameSize, channelCount, sampleFormat, sampleWidth;
	char			*buffer;
	double			sampleRate;

	ALport			outport;
	ALconfig		outportconfig;

	if (ac < 2)
		usage();

	file = afOpenFile(av[1], "r", NULL);
	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 1);
	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);
	sampleRate = afGetRate(file, AF_DEFAULT_TRACK);
	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	printf("frame count: %d\n", frameCount);
	printf("frame size: %d bytes\n", frameSize);
	printf("channel count: %d\n", channelCount);
	printf("sample rate: %.2f Hz\n", sampleRate);
	buffer = (char *) malloc(BUFFERED_FRAME_COUNT * frameSize);

	outportconfig = alNewConfig();
	setwidth(outportconfig, sampleWidth);
	alSetChannels(outportconfig, channelCount);

	count = afReadFrames(file, AF_DEFAULT_TRACK, buffer, BUFFERED_FRAME_COUNT);

	outport = alOpenPort("dick", "w", outportconfig);
	setrate(outport, sampleRate);

	do
	{
		printf("count = %d\n", count);
		alWriteFrames(outport, buffer, count);

	}
	while (count = afReadFrames(file, AF_DEFAULT_TRACK, buffer, BUFFERED_FRAME_COUNT));

	waitport(outport);

	alClosePort(outport);
	alFreeConfig(outportconfig);

	afCloseFile(file);
}

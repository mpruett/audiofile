/*
	Audio File Library

	Copyright (c) 1998-1999, Michael Pruett <michael@68k.org>
	Copyright (c) 2001, Silicon Graphics, Inc.

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
	linuxtest.c

	This file plays a 16-bit, 44.1 kHz monophonic or stereophonic
	audio file through a PC sound card on a Linux system.  This file
	will not compile under any operating system that does not support
	the Open Sound System API.
*/

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/soundcard.h>

#include <audiofile.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*
	If it's not defined already, define the native audio hardware
	byte order.
*/

#ifndef AFMT_S16_NE
#ifdef WORDS_BIGENDIAN /* defined in config.h */
#define AFMT_S16_NE AFMT_S16_BE
#else
#define AFMT_S16_NE AFMT_S16_LE
#endif /* WORDS_BIGENDIAN */
#endif /* AFMT_S16_NE */

void setupdsp (int audiofd, int channelCount);
void usage (void);

/* BUFFER_FRAMES represents the size of the buffer in frames. */
#define BUFFER_FRAMES 4096

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFframecount	frameCount, framesRead;
	int		sampleFormat, sampleWidth, channelCount;
	float		frameSize;
	void		*buffer;
	int		audiofd;

	if (argc != 2)
		usage();

	file = afOpenFile(argv[1], "r", NULL);
	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	printf("frame count: %d\n", (int) frameCount);

	channelCount = afGetVirtualChannels(file, AF_DEFAULT_TRACK);
	afGetVirtualSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat,
		&sampleWidth);

	frameSize = afGetVirtualFrameSize(file, AF_DEFAULT_TRACK, 1);

	printf("sample format: %d, sample width: %d, channels: %d\n",
		sampleFormat, sampleWidth, channelCount);

	if ((sampleFormat != AF_SAMPFMT_TWOSCOMP) &&
		(sampleFormat != AF_SAMPFMT_UNSIGNED))
	{
		fprintf(stderr, "The audio file must contain integer data in two's complement or unsigned format.\n");
		exit(EXIT_FAILURE);
	}

	if ((sampleWidth != 16) || (channelCount > 2))
	{
		fprintf(stderr, "The audio file must be of a 16-bit monophonic or stereophonic format.\n");
		exit(EXIT_FAILURE);
	}

	buffer = malloc(BUFFER_FRAMES * frameSize);

	audiofd = open("/dev/dsp", O_WRONLY);
	if (audiofd < 0)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}

	setupdsp(audiofd, channelCount);

	framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer,
		BUFFER_FRAMES);

	while (framesRead > 0)
	{
		printf("read %ld frames\n", framesRead);
		write(audiofd, buffer, framesRead * frameSize);
		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, buffer,
			BUFFER_FRAMES);
	}

	close(audiofd);
	free(buffer);

	return 0;
}

void setupdsp (int audiofd, int channelCount)
{
	int	format, frequency, channels;

	format = AFMT_S16_NE;
	if (ioctl(audiofd, SNDCTL_DSP_SETFMT, &format) == -1)
	{
		perror("set format");
		exit(EXIT_FAILURE);
	}

	if (format != AFMT_S16_NE)
	{
		fprintf(stderr, "format not correct.\n");
		exit(EXIT_FAILURE);
	}

	channels = channelCount;
	if (ioctl(audiofd, SNDCTL_DSP_CHANNELS, &channels) == -1)
	{
		perror("set channels");
		exit(EXIT_FAILURE);
	}

	frequency = 44100;
	if (ioctl(audiofd, SNDCTL_DSP_SPEED, &frequency) == -1)
	{
		perror("set frequency");
		exit(EXIT_FAILURE);
	}
}

void usage (void)
{
	fprintf(stderr, "usage: linuxtest file\n");
	fprintf(stderr,
		"where file refers to a 16-bit monophonic or stereophonic 44.1 kHz audio file\n");
	exit(EXIT_FAILURE);
}

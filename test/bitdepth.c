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

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFframecount	frameCount;
	int		channelCount;
	int		frameSize;
	char		*buffer;
	int		i;

	if (argc != 2)
	{
		fprintf(stderr, "usage: bitdepth filename\n");
		exit(-1);
	}

	file = afOpenFile(argv[1], "r", NULL);
	frameCount = afGetFrameCount(file, AF_DEFAULT_TRACK);
	channelCount = afGetChannels(file, AF_DEFAULT_TRACK);
	frameSize = afGetFrameSize(file, AF_DEFAULT_TRACK, 1);
	buffer = (char *) malloc(frameSize * frameCount);
	afReadFrames(file, AF_DEFAULT_TRACK, buffer, frameCount);

	for (i=0; i<frameCount; i+=4)
		printf("%d %d %d %d\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3]);
	afCloseFile(file);

	return 0;
}

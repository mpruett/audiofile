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
	sfinfo.c

	This program displays information about audio files.
*/

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdio.h>

/*
File Name      satan.aiff
File Format    Audio Interchange File Format (aiff)
Data Format    16-bit integer (2's complement, big endian)
Audio Data     512 KBytes begins at offset 124 (7C hex)
               2 channels, 131072 frames
Sampling Rate  44.1 KHz
Duration       2.97 seconds
*/

void printfileinfo (char *filename);

int main (int argc, char **argv)
{
	int				i = 1;

	while (i < argc)
	{
		printfileinfo(argv[i]);
		i++;
		if (i < argc)
			putchar('\n');
	}

	return 0;
}

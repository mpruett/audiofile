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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#ifdef DEBUG
#define DEBG printf
#else
#define DEBG
#endif

char copyright[] = "1998 Michael Pruett";
char name[] = "Michael Pruett's home-brew methamphetamines";

/* Two frames of 16-bit stereo samples. */
u_int16_t data[] = {0, 1, 2, 3};

int main (int argc, char **argv)
{
	AFfilehandle	file;
	AFfilesetup		setup;
	int				miscids[] = {1, 2};
	int				result;

	if (argc < 2)
	{
		fprintf(stderr, "usage: %s <audio file>\n", argv[0]);
		exit(0);
	}

	setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_AIFF);
	afInitMiscIDs(setup, miscids, 2);
	afInitMiscType(setup, 1, AF_MISC_COPY);
	afInitMiscType(setup, 2, AF_MISC_NAME);
	afInitMiscSize(setup, 1, strlen(copyright));
	afInitMiscSize(setup, 2, strlen(name));

	file = afOpenFile(argv[1], "w", setup);

	result = afWriteMisc(file, 1, copyright, strlen(copyright));
	DEBG("wrote miscellaneous data of type %d with length = %d\n",
		afGetMiscType(file, 1), result);
	result = afWriteMisc(file, 2, name, strlen(name));

	DEBG("wrote miscellaneous data of type %d with length = %d\n",
		afGetMiscType(file, 2), result);

	/* Write out two token frames of sample data. */
	afWriteFrames(file, AF_DEFAULT_TRACK, data, 2);

	afCloseFile(file);
	afFreeFileSetup(setup);

	return 0;
}

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
	results.c

	This file reports the values returned by some special cases of
	Audio File Library functions.
*/

#include <stdio.h>

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

int main (int ac, char **av)
{
	int result;

	afOpenFile("/tmp/dick", "r", NULL);

	result = afSyncFile(NULL);
	printf("\nafSyncFile(NULL) = %d\n", result);
	result = afCloseFile(NULL);
	printf("\nafCloseFile(NULL) = %d\n", result);

	return 0;
}

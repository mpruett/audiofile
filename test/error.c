/*
	Audio File Library

	Copyright (C) 2000-2001, Silicon Graphics, Inc.

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

#include <stdio.h>
#include <stdlib.h>
#include <audiofile.h>

int expectedError;
int verbose = 0;

void myerrorfunc (long error, const char *description)
{
	if (error != expectedError)
	{
		if (verbose)
		{
			printf("WRONG ERROR CODE:\n");
			printf("%s [error code %ld]--", description, error);
			printf("expected error code %d\n", expectedError);
		}
		exit(EXIT_FAILURE);
	}
	else
	{
		if (verbose)
		{
			printf("CORRECT ERROR CODE:\n");
			printf("%s [error code %ld]\n", description, error);
		}
	}
}

void test1 (void)
{
	expectedError = AF_BAD_FILEHANDLE;

	if (verbose) printf("closing null file handle\n");
	afCloseFile(AF_NULL_FILEHANDLE);

	expectedError = AF_BAD_FILESETUP;
	if (verbose) printf("freeing null file setup\n");
	afFreeFileSetup(AF_NULL_FILESETUP);

	expectedError = AF_BAD_OPEN;
	if (verbose) printf("opening nonexistent file\n");
	afOpenFile("sldkjflsdkfjalksdjflaksdjflsakfdj", "r", NULL);
}

void test2 (void)
{
	AFfilesetup	setup;
	setup = afNewFileSetup();

	expectedError = AF_BAD_FILEFMT;
	if (verbose) printf("initializing file format to invalid value\n");
	afInitFileFormat(setup, 91094);

	expectedError = AF_BAD_SAMPFMT;
	if (verbose) printf("initializing sample format and sample width to invalid value\n");
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, 3992, 3932);

	afFreeFileSetup(setup);

	expectedError = AF_BAD_FILESETUP;
	if (verbose) printf("initializing file format on a file setup which has been deallocated\n");
	afInitFileFormat(setup, AF_FILE_AIFFC);
}

void test3 (void)
{
	expectedError = AF_BAD_QUERY;
	if (verbose) printf("querying on bad selectors\n");
	afQueryLong(AF_QUERYTYPE_FILEFMT, 9999, 9999, 9999, 9999);

	expectedError = AF_BAD_QUERYTYPE;
	if (verbose) printf("querying using bad query type\n");
	afQueryLong(9999, 9999, 9999, 9999, 9999);
}

int main (int argc, char **argv)
{
	afSetErrorHandler(myerrorfunc);

	if (argc == 2 && strcmp(argv[1], "-v") == 0)
		verbose = 1;

	test1();
	test2();
	test3();

	return 0;
}

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
	printinfo.c

	This file contains the function used by the sf commands to print
	information regarding a file.
*/

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdio.h>
#include <stdlib.h>

char *copyrightstring (AFfilehandle file);

void printfileinfo (char *filename)
{
	int		version;
	AFfilehandle	file;
	int		sampleFormat, sampleWidth, byteOrder;
	char		*copyright, *formatstring, *labelstring;

	file = afOpenFile(filename, "r", NULL);

	if (file == NULL)
		return;

	formatstring = afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_DESC,
		afGetFileFormat(file, &version), 0, 0);
	labelstring = afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_LABEL,
		afGetFileFormat(file, &version), 0, 0);

	if (formatstring == NULL || labelstring == NULL)
		return;

	printf("File Name      %s\n", filename);
	printf("File Format    %s (%s)\n", formatstring, labelstring);

	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	byteOrder = afGetByteOrder(file, AF_DEFAULT_TRACK);

	printf("Data Format    ");

	switch (sampleFormat)
	{
		case AF_SAMPFMT_TWOSCOMP:
			printf("%d-bit integer (2's complement, %s)\n",
				sampleWidth,
				byteOrder == AF_BYTEORDER_BIGENDIAN ?
					"big endian" : "little endian");
			break;
		case AF_SAMPFMT_UNSIGNED:
			printf("%d-bit integer (unsigned, %s)\n", sampleWidth,
				byteOrder == AF_BYTEORDER_BIGENDIAN ?
					"big endian" : "little endian");
			break;
		case AF_SAMPFMT_FLOAT:
			printf("single-precision (32-bit) floating point, %s\n",
				byteOrder == AF_BYTEORDER_BIGENDIAN ?
					"big endian" : "little endian");
			break;
		case AF_SAMPFMT_DOUBLE:
			printf("double-precision (64-bit) floating point, %s\n",
				byteOrder == AF_BYTEORDER_BIGENDIAN ?
					"big endian" : "little endian");
			break;
		default:
			printf("unknown\n");
			break;
	}

	printf("Audio Data     %ld bytes begins at offset %ld (%lx hex)\n",
		afGetTrackBytes(file, AF_DEFAULT_TRACK),
		afGetDataOffset(file, AF_DEFAULT_TRACK),
		afGetDataOffset(file, AF_DEFAULT_TRACK));
	printf("               %d channel%s, %ld frames\n",
		afGetChannels(file, AF_DEFAULT_TRACK),
		afGetChannels(file, AF_DEFAULT_TRACK) > 1 ? "s" : "",
		afGetFrameCount(file, AF_DEFAULT_TRACK));

	printf("Sampling Rate  %.2f Hz\n", afGetRate(file, AF_DEFAULT_TRACK));

	printf("Duration       %.2f seconds\n",
		afGetFrameCount(file, AF_DEFAULT_TRACK) /
		afGetRate(file, AF_DEFAULT_TRACK));

	copyright = copyrightstring(file);
	if (copyright)
	{
		printf("Copyright      %s\n", copyright);
		free(copyright);
	}

	afCloseFile(file);
}

char *copyrightstring (AFfilehandle file)
{
	char	*copyright = NULL;
	int		*miscids;
	int		i, misccount;

	misccount = afGetMiscIDs(file, NULL);
	miscids = malloc(sizeof (int) * misccount);
	afGetMiscIDs(file, miscids);

	for (i=0; i<misccount; i++)
	{
		char	*data;
		int		datasize;

		if (afGetMiscType(file, miscids[i]) != AF_MISC_COPY)
			continue;

		/*
			If this code executes, the miscellaneous chunk is a
			copyright chunk.
		*/
		datasize = afGetMiscSize(file, miscids[i]);
		data = malloc(datasize);
		afReadMisc(file, miscids[i], data, datasize);
		copyright = data;
		break;
	}

	free(miscids);

	return copyright;
}

/*
	Audio File Library

	Copyright (C) 2001, Silicon Graphics, Inc.
	Copyright (C) 1998, Michael Pruett <michael@68k.org>

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
	sfconvert is a program which can convert various parameters of
	sound files.
*/

#include "config.h"

#ifdef __USE_SGI_HEADERS__
#include <dmedia/audiofile.h>
#else
#include <audiofile.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "printinfo.h"

#define BUFFER_FRAME_COUNT 65536

void printversion (void);
void printusage (void);
void usageerror (void);
bool copyaudiodata (AFfilehandle infile, AFfilehandle outfile, int trackid);

int main (int argc, char **argv)
{
	if (argc == 2)
	{
		if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))
		{
			printversion();
			exit(EXIT_SUCCESS);
		}

		if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
		{
			printusage();
			exit(EXIT_SUCCESS);
		}
	}

	if (argc < 3)
		usageerror();

	const char *inFileName = argv[1];
	const char *outFileName = argv[2];

	int outFileFormat = AF_FILE_UNKNOWN;

	int outSampleFormat = -1, outSampleWidth = -1, outChannelCount = -1;
	double outMaxAmp = 1.0;

	int i = 3;

	while (i < argc)
	{
		if (!strcmp(argv[i], "format"))
		{
			if (i + 1 >= argc)
				usageerror();

			if (!strcmp(argv[i+1], "aiff"))
				outFileFormat = AF_FILE_AIFF;
			else if (!strcmp(argv[i+1], "aifc"))
				outFileFormat = AF_FILE_AIFFC;
			else if (!strcmp(argv[i+1], "wave"))
				outFileFormat = AF_FILE_WAVE;
			else if (!strcmp(argv[i+1], "next"))
				outFileFormat = AF_FILE_NEXTSND;
			else if (!strcmp(argv[i+1], "bics"))
				outFileFormat = AF_FILE_BICSF;
			else if (!strcmp(argv[i+1], "smp"))
				outFileFormat = AF_FILE_SAMPLEVISION;
			else if (!strcmp(argv[i+1], "voc"))
				outFileFormat = AF_FILE_VOC;
			else if (!strcmp(argv[i+1], "nist"))
				outFileFormat = AF_FILE_NIST_SPHERE;
			else if (!strcmp(argv[i+1], "caf"))
				outFileFormat = AF_FILE_CAF;
			else
			{
				fprintf(stderr, "sfconvert: Unknown format %s.\n", argv[i+1]);
				exit(EXIT_FAILURE);
			}

			// Increment for argument.
			i++;
		}
		else if (!strcmp(argv[i], "channels"))
		{
			if (i + 1 >= argc)
				usageerror();

			outChannelCount = atoi(argv[i+1]);
			if (outChannelCount < 1)
				usageerror();

			// Increment for argument.
			i++;
		}
		else if (!strcmp(argv[i], "float"))
		{
			if (i + 1 >= argc)
				usageerror();

			outSampleFormat = AF_SAMPFMT_FLOAT;
			outSampleWidth = 32;
			outMaxAmp = atof(argv[i+1]);

			// outMaxAmp is currently unused.
			(void) outMaxAmp;

			// Increment for argument.
			i++;
		}
		else if (!strcmp(argv[i], "integer"))
		{
			if (i + 2 >= argc)
				usageerror();

			outSampleWidth = atoi(argv[i+1]);
			if (outSampleWidth < 1 || outSampleWidth > 32)
				usageerror();

			if (!strcmp(argv[i+2], "2scomp"))
				outSampleFormat = AF_SAMPFMT_TWOSCOMP;
			else if (!strcmp(argv[i+2], "unsigned"))
				outSampleFormat = AF_SAMPFMT_UNSIGNED;
			else
				usageerror();

			// Increment for arguments.
			i += 2;
		}
		else
		{
			printf("Unrecognized command %s\n", argv[i]);
		}

		i++;
	}

	AFfilehandle inFile = afOpenFile(inFileName, "r", AF_NULL_FILESETUP);
	if (!inFile)
	{
		printf("Could not open file '%s' for reading.\n", inFileName);
		return EXIT_FAILURE;
	}

	// Get audio format parameters from input file.
	int fileFormat = afGetFileFormat(inFile, NULL);
	int channelCount = afGetChannels(inFile, AF_DEFAULT_TRACK);
	double sampleRate = afGetRate(inFile, AF_DEFAULT_TRACK);
	int sampleFormat, sampleWidth;
	afGetSampleFormat(inFile, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);

	// Initialize output audio format parameters.
	AFfilesetup outFileSetup = afNewFileSetup();

	if (outFileFormat == -1)
		outFileFormat = fileFormat;

	if (outSampleFormat == -1 || outSampleWidth == -1)
	{
		outSampleFormat = sampleFormat;
		outSampleWidth = sampleWidth;
	}

	if (outChannelCount == -1)
		outChannelCount = channelCount;

	afInitFileFormat(outFileSetup, outFileFormat);
	afInitSampleFormat(outFileSetup, AF_DEFAULT_TRACK, outSampleFormat,
		outSampleWidth);
	afInitChannels(outFileSetup, AF_DEFAULT_TRACK, outChannelCount);
	afInitRate(outFileSetup, AF_DEFAULT_TRACK, sampleRate);

	AFfilehandle outFile = afOpenFile(outFileName, "w", outFileSetup);
	if (!outFile)
	{
		printf("Could not open file '%s' for writing.\n", outFileName);
		return EXIT_FAILURE;
	}

	afFreeFileSetup(outFileSetup);

	/*
		Set the output file's virtual audio format parameters
		to match the audio format parameters of the input file.
	*/
	afSetVirtualChannels(outFile, AF_DEFAULT_TRACK, channelCount);
	afSetVirtualSampleFormat(outFile, AF_DEFAULT_TRACK, sampleFormat,
		sampleWidth);

	copyaudiodata(inFile, outFile, AF_DEFAULT_TRACK);

	afCloseFile(inFile);
	afCloseFile(outFile);

	printfileinfo(inFileName);
	putchar('\n');
	printfileinfo(outFileName);

	return EXIT_SUCCESS;
}

void printusage (void)
{
	printf("usage: sfconvert infile outfile [ options ... ] [ output keywords ... ]\n");
	printf("\n");

	printf("Where keywords specify format of input or output soundfile:\n");
	printf("    byteorder e    endian (e is big or little)\n");
	printf("    channels n     n-channel file (1 or 2)\n");
	printf("    format f       file format f (see below)\n");
	printf("    integer n s    n-bit integer file, where s is one of\n");
	printf("                       2scomp: 2's complement signed data\n");
	printf("                       unsigned: unsigned data\n");
	printf("    float m        floating point file, maxamp m (usually 1.0)\n");
	printf("\n");

	printf("Currently supported formats are:\n");
	printf("\n");
	printf("    aiff    Audio Interchange File Format\n");
	printf("    aifc    AIFF-C File Format\n");
	printf("    next    NeXT/Sun Format\n");
	printf("    wave    MS RIFF WAVE Format\n");
	printf("    bics    Berkeley/IRCAM/CARL Sound File Format\n");
	printf("    smp     Sample Vision Format\n");
	printf("    voc     Creative Voice File\n");
	printf("    nist    NIST SPHERE Format\n");
	printf("    caf     Core Audio Format\n");
	printf("\n");
}

void usageerror (void)
{
	printusage();
	exit(EXIT_FAILURE);
}

void printversion (void)
{
	printf("sfconvert: Audio File Library version %s\n", VERSION);
}

/*
	Copy audio data from one file to another.  This function
	assumes that the virtual sample formats of the two files
	match.
*/
bool copyaudiodata (AFfilehandle infile, AFfilehandle outfile, int trackid)
{
	int frameSize = afGetVirtualFrameSize(infile, trackid, 1);

	void *buffer = malloc(BUFFER_FRAME_COUNT * frameSize);

	AFframecount totalFrames = afGetFrameCount(infile, AF_DEFAULT_TRACK);
	AFframecount totalFramesWritten = 0;

	bool ok = true, done = false;

	while (!done)
	{
		AFframecount framesToRead = BUFFER_FRAME_COUNT;
		AFframecount framesRead, framesWritten;

		framesRead = afReadFrames(infile, trackid, buffer,
			framesToRead);

		if (framesRead < 0)
		{
			fprintf(stderr, "Bad read of audio track data.\n");
			ok = false;
			done = true;
		}

		framesWritten = afWriteFrames(outfile, trackid, buffer,
			framesRead);

		if (framesWritten < 0)
		{
			fprintf(stderr, "Bad write of audio track data.\n");
			ok = false;
			done = true;
		}
		else
		{
			totalFramesWritten += framesWritten;
		}

		if (totalFramesWritten == totalFrames)
			done = true;
	}

	free(buffer);

	return ok;
}

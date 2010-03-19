/*
	Audio File Library

	Copyright (c) 2010, Michael Pruett <michael@68k.org>

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
	This program tests writing and reading audio files with more
	than 2^24 frames.

	This program serves as a regression test for a bug in which
	writing certain audio file formats with more than 2^24 frames
	would produce files with incorrect sizes.
*/

#include <audiofile.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char *testFileName = "/tmp/audiofile-test";

void cleanup ()
{
	unlink(testFileName);
}

void ensure (bool condition, const char *message)
{
	if (!condition)
	{
		fprintf(stderr, "%s\n", message);
		cleanup();
		exit(EXIT_FAILURE);
	}
}

void writeLargeFile(int fileFormat)
{
	fprintf(stderr, "Testing %s\n",
		afQueryPointer(AF_QUERYTYPE_FILEFMT, AF_QUERY_NAME, fileFormat, 0, 0));
	AFfilesetup setup = afNewFileSetup();
	afInitFileFormat(setup, fileFormat);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
	AFfilehandle file = afOpenFile(testFileName, "w", setup);
	afFreeFileSetup(setup);
	ensure(file, "Could not open file for writing");

	const int bufferFrameCount = 32768;
	int16_t *data = malloc(bufferFrameCount * sizeof (int16_t));
	const AFframecount frameCount = 0x1000007;
	AFframecount framesWritten = 0;
	while (framesWritten < frameCount)
	{
		AFframecount framesToWrite = bufferFrameCount;
		if (framesToWrite > frameCount - framesWritten)
			framesToWrite = frameCount - framesWritten;
		int dataValue = framesWritten % 32749;
		for (int i=0; i<bufferFrameCount; i++, dataValue++)
			data[i] = dataValue % 32749;
		afWriteFrames(file, AF_DEFAULT_TRACK, data, framesToWrite);
		framesWritten += framesToWrite;
	}
	ensure(afGetFrameCount(file, AF_DEFAULT_TRACK) == frameCount,
		"Incorrect frame count for file");
	afCloseFile(file);

	file = afOpenFile(testFileName, "r", AF_NULL_FILESETUP);
	ensure(file, "Could not open file for reading");
	ensure(afGetFrameCount(file, AF_DEFAULT_TRACK) == frameCount,
		"Incorrect frame count in file opened for reading");
	ensure(afGetTrackBytes(file, AF_DEFAULT_TRACK) == frameCount * sizeof (int16_t),
		"Incorrect byte size in file opened for reading");
	AFframecount framesRead = 0;
	while (framesRead < frameCount)
	{
		AFframecount framesToRead = bufferFrameCount;
		if (framesToRead > frameCount - framesRead)
			framesToRead = frameCount - framesRead;
		afReadFrames(file, AF_DEFAULT_TRACK, data, framesToRead);
		bool valid = true;
		int dataValue = framesRead % 32749;
		for (int i=0; i<framesToRead; i++, dataValue++)
			if (data[i] != dataValue % 32749)
			{
				valid = false;
				break;
			}
		ensure(valid, "Data read does not match data written");
		framesRead += framesToRead;
	}
	free(data);
	afCloseFile(file);
	cleanup();
}

int main (int argc, char **argv)
{
	int fileFormats[] = { AF_FILE_AIFF, AF_FILE_AIFFC, AF_FILE_NEXTSND, AF_FILE_WAVE, AF_FILE_IRCAM, AF_FILE_AVR };
	int numFileFormats = sizeof (fileFormats) / sizeof (fileFormats[0]);
	for (int i=0; i<numFileFormats; i++)
		writeLargeFile(fileFormats[i]);
}

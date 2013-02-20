/*
	Audio File Library
	Copyright (C) 2013 Michael Pruett <michael@68k.org>

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

#include <audiofile.h>
#include <gtest/gtest.h>
#include <stdint.h>
#include <unistd.h>

#include "TestUtilities.h"

template <typename SampleType>
static void testALAC(int fileFormat, int channelCount, int sampleWidth, int frameCount)
{
	std::string testFileName;
	ASSERT_TRUE(createTemporaryFile("ALAC", &testFileName));

	AFfilesetup setup = afNewFileSetup();
	afInitFileFormat(setup, fileFormat);
	afInitChannels(setup, AF_DEFAULT_TRACK, channelCount);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, sampleWidth);
	afInitCompression(setup, AF_DEFAULT_TRACK, AF_COMPRESSION_ALAC);
	AFfilehandle file = afOpenFile(testFileName.c_str(), "w", setup);
	ASSERT_TRUE(file);
	afFreeFileSetup(setup);

	unsigned shift = (((sampleWidth + 7) >> 3) << 3) - sampleWidth;
	SampleType *data = new SampleType[frameCount * channelCount];
	for (int i=0; i<frameCount; i++)
		for (int c=0; c<channelCount; c++)
			data[i*channelCount + c] = (i << shift) * ((c&1) ? -1 : 1);

	AFframecount framesWritten = afWriteFrames(file, AF_DEFAULT_TRACK, data, frameCount);
	ASSERT_EQ(framesWritten, frameCount);

	ASSERT_EQ(afCloseFile(file), 0);

	file = afOpenFile(testFileName.c_str(), "r", AF_NULL_FILESETUP);
	ASSERT_TRUE(file);
	ASSERT_EQ(afGetCompression(file, AF_DEFAULT_TRACK), AF_COMPRESSION_ALAC);
	int readSampleFormat, readSampleWidth;
	afGetSampleFormat(file, AF_DEFAULT_TRACK, &readSampleFormat, &readSampleWidth);
	ASSERT_EQ(readSampleFormat, AF_SAMPFMT_TWOSCOMP);
	ASSERT_EQ(readSampleWidth, sampleWidth);
	ASSERT_EQ(afGetFrameCount(file, AF_DEFAULT_TRACK), frameCount);

	SampleType *readData = new SampleType[frameCount * channelCount];
	AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, readData, frameCount);
	ASSERT_EQ(framesRead, frameCount);

	for (int i=0; i<frameCount; i++)
		for (int c=0; c<channelCount; c++)
			EXPECT_EQ(data[i*channelCount + c], readData[i*channelCount + c]) <<
				"failed at " << i;

	// Read entire file with a seek before each read operation.
	for (AFframecount offset = 0; offset < frameCount; offset += 373)
	{
		ASSERT_EQ(afSeekFrame(file, AF_DEFAULT_TRACK, offset), offset);

		AFframecount framesToRead = 1091;
		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, readData, framesToRead);
		ASSERT_EQ(framesRead, std::min(framesToRead, frameCount - offset));

		for (int i=0; i<framesRead; i++)
			for (int c=0; c<channelCount; c++)
				EXPECT_EQ(data[(i+offset)*channelCount + c],
					readData[i*channelCount + c]) << "failed at " << i;
	}

	// Read entire file sequentially in multiple read operations.
	ASSERT_EQ(afSeekFrame(file, AF_DEFAULT_TRACK, 0), 0);

	AFframecount framesToRead = 1087;
	for (AFframecount offset = 0; offset < frameCount; offset += framesToRead)
	{
		framesRead = afReadFrames(file, AF_DEFAULT_TRACK, readData, framesToRead);
		ASSERT_EQ(framesRead, std::min(framesToRead, frameCount - offset));

		for (int i=0; i<framesRead; i++)
			for (int c=0; c<channelCount; c++)
				EXPECT_EQ(data[(i+offset)*channelCount + c],
					readData[i*channelCount + c]) << "failed at " << i;
	}

	ASSERT_EQ(afCloseFile(file), 0);

	delete [] data;
	delete [] readData;

	ASSERT_EQ(::unlink(testFileName.c_str()), 0);
}

TEST(ALAC, ALAC_16)
{
	for (int channelCount=1; channelCount<=4; channelCount++)
		testALAC<int16_t>(AF_FILE_CAF, channelCount, 16, 82421);
}

TEST(ALAC, ALAC_20)
{
	for (int channelCount=1; channelCount<=4; channelCount++)
		testALAC<int32_t>(AF_FILE_CAF, channelCount, 20, 82421);
}

TEST(ALAC, ALAC_24)
{
	for (int channelCount=1; channelCount<=4; channelCount++)
		testALAC<int32_t>(AF_FILE_CAF, channelCount, 24, 82421);
}

TEST(ALAC, ALAC_32)
{
	for (int channelCount=1; channelCount<=4; channelCount++)
		testALAC<int32_t>(AF_FILE_CAF, channelCount, 32, 82421);
}

static void testInvalidSampleFormat(int sampleFormat, int sampleWidth)
{
	std::string testFileName;
	ASSERT_TRUE(createTemporaryFile("ALAC", &testFileName));

	AFfilesetup setup = afNewFileSetup();
	afInitFileFormat(setup, AF_FILE_CAF);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, sampleFormat, sampleWidth);
	afInitCompression(setup, AF_DEFAULT_TRACK, AF_COMPRESSION_ALAC);
	AFfilehandle file = afOpenFile(testFileName.c_str(), "w", setup);
	ASSERT_FALSE(file);
	afFreeFileSetup(setup);

	ASSERT_EQ(::unlink(testFileName.c_str()), 0);
}

TEST(ALAC, InvalidSampleWidths)
{
	IgnoreErrors ignoreErrors;

	for (int sampleWidth=1; sampleWidth<=32; sampleWidth++)
	{
		if (sampleWidth == 16 ||
			sampleWidth == 20 ||
			sampleWidth == 24 ||
			sampleWidth == 32)
			continue;

		testInvalidSampleFormat(AF_SAMPFMT_TWOSCOMP, sampleWidth);
	}
}

TEST(ALAC, InvalidSampleFormats)
{
	IgnoreErrors ignoreErrors;

	for (int sampleWidth=1; sampleWidth<=32; sampleWidth++)
		testInvalidSampleFormat(AF_SAMPFMT_UNSIGNED, sampleWidth);

	testInvalidSampleFormat(AF_SAMPFMT_FLOAT, 32);
	testInvalidSampleFormat(AF_SAMPFMT_DOUBLE, 64);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

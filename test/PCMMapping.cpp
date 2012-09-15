/*
	Copyright (C) 2010, Michael Pruett. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

	3. The name of the author may not be used to endorse or promote products
	derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <audiofile.h>
#include <gtest/gtest.h>
#include <limits>
#include <stdint.h>
#include <unistd.h>

class PCMMappingTest : public testing::Test
{
protected:
	virtual void SetUp()
	{
		int tmp = mkstemp(PCMMappingTest::kTestFileName);
	}
	virtual void TearDown()
	{
		::unlink(kTestFileName);
	}

	static char kTestFileName[];

	static AFfilehandle createTestFile(int sampleFormat, int sampleWidth)
	{
		AFfilesetup setup = afNewFileSetup();
		afInitFileFormat(setup, AF_FILE_AIFFC);
		afInitChannels(setup, AF_DEFAULT_TRACK, 1);
		afInitSampleFormat(setup, AF_DEFAULT_TRACK, sampleFormat, sampleWidth);
		AFfilehandle file = afOpenFile(kTestFileName, "w", setup);
		afFreeFileSetup(setup);
		return file;
	}
	static AFfilehandle openTestFile()
	{
		AFfilehandle file = afOpenFile(kTestFileName, "r", AF_NULL_FILESETUP);
		return file;
	}
};

char PCMMappingTest::kTestFileName[] = "/tmp/test.aiffXXXXXX";

TEST_F(PCMMappingTest, Float)
{
	AFfilehandle file = createTestFile(AF_SAMPFMT_FLOAT, 32);
	const float data[] = { -1, 0, 1 };
	const int frameCount = sizeof (data) / sizeof (data[0]);
	AFframecount framesWritten = afWriteFrames(file, AF_DEFAULT_TRACK, data, frameCount);
	ASSERT_EQ(framesWritten, frameCount);
	afCloseFile(file);
	file = openTestFile();
	ASSERT_TRUE(file != NULL);
	double slope = 2, intercept = 2, minClip = 0, maxClip = 4;
	afSetVirtualPCMMapping(file, AF_DEFAULT_TRACK, slope, intercept, minClip, maxClip);
	float readData[frameCount];
	AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, readData, frameCount);
	ASSERT_EQ(framesRead, frameCount);
	afCloseFile(file);
	const float expectedData[] = { 0, 2, 4 };
	for (int i=0; i<frameCount; i++)
		EXPECT_EQ(readData[i], expectedData[i]);
}

TEST_F(PCMMappingTest, Double)
{
	AFfilehandle file = createTestFile(AF_SAMPFMT_DOUBLE, 64);
	const double data[] = { -1, 0, 1 };
	const int frameCount = sizeof (data) / sizeof (data[0]);
	AFframecount framesWritten = afWriteFrames(file, AF_DEFAULT_TRACK, data, frameCount);
	ASSERT_EQ(framesWritten, frameCount);
	afCloseFile(file);
	file = openTestFile();
	ASSERT_TRUE(file != NULL);
	double slope = 2, intercept = 2, minClip = 0, maxClip = 4;
	afSetVirtualPCMMapping(file, AF_DEFAULT_TRACK, slope, intercept, minClip, maxClip);
	double readData[frameCount];
	AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, readData, frameCount);
	ASSERT_EQ(framesRead, frameCount);
	afCloseFile(file);
	const double expectedData[] = { 0, 2, 4 };
	for (int i=0; i<frameCount; i++)
		EXPECT_EQ(readData[i], expectedData[i]);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

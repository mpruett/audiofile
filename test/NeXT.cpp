/*
	Copyright (C) 2011, Michael Pruett. All rights reserved.

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
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const char kDataUnspecifiedLength[] =
{
	'.', 's', 'n', 'd',
	0, 0, 0, 24, // offset of 24 bytes
	0xff, 0xff, 0xff, 0xff, // unspecified length
	0, 0, 0, 3, // 16-bit linear
	0, 0, 172, 68, // 44100 Hz
	0, 0, 0, 1, // 1 channel
	0, 1,
	0, 1,
	0, 2,
	0, 3,
	0, 5,
	0, 8,
	0, 13,
	0, 21,
	0, 34,
	0, 55
};

const char kDataTruncated[] =
{
	'.', 's', 'n', 'd',
	0, 0, 0, 24, // offset of 24 bytes
	0, 0, 0, 20, // length of 20 bytes
	0, 0, 0, 3, // 16-bit linear
	0, 0, 172, 68, // 44100 Hz
	0, 0, 0, 1, // 1 channel
	0, 1,
	0, 1,
	0, 2,
	0, 3,
	0, 5,
	0, 8,
	0, 13,
	0, 21,
	0, 34,
	0, 55
};

const int16_t kFrames[] = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };
const int kFrameCount = sizeof (kFrames) / sizeof (kFrames[0]);

char kTestFileName[] = "/tmp/test.auXXXXXX";

TEST(NeXT, UnspecifiedLength)
{
	int fd = ::open(kTestFileName, O_RDWR | O_CREAT | O_TRUNC, 0644);
	ASSERT_GT(fd, -1);
	ASSERT_EQ(::write(fd, kDataUnspecifiedLength, sizeof (kDataUnspecifiedLength)), sizeof (kDataUnspecifiedLength));
	::close(fd);

	AFfilehandle file = afOpenFile(kTestFileName, "r", NULL);
	EXPECT_TRUE(file);

	int sampleFormat, sampleWidth;
	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	EXPECT_EQ(sampleFormat, AF_SAMPFMT_TWOSCOMP);
	EXPECT_EQ(sampleWidth, 16);
	EXPECT_EQ(afGetChannels(file, AF_DEFAULT_TRACK), 1);
	EXPECT_EQ(afGetTrackBytes(file, AF_DEFAULT_TRACK),
		kFrameCount * sizeof (int16_t));
	EXPECT_EQ(afGetFrameCount(file, AF_DEFAULT_TRACK), kFrameCount);

	int16_t *data = new int16_t[kFrameCount];
	AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, data,
		kFrameCount);
	EXPECT_EQ(framesRead, kFrameCount);
	for (int i=0; i<kFrameCount; i++)
		EXPECT_EQ(data[i], kFrames[i]);
	delete [] data;

	afCloseFile(file);

	::unlink(kTestFileName);
}

TEST(NeXT, Truncated)
{
	int fd = ::open(kTestFileName, O_RDWR | O_CREAT | O_TRUNC, 0644);
	ASSERT_GT(fd, -1);
	const size_t truncatedSize = sizeof (kDataTruncated) - 4;
	const int truncatedFrameCount = kFrameCount - 2;
	ASSERT_EQ(::write(fd, kDataTruncated, truncatedSize), truncatedSize);
	::close(fd);

	AFfilehandle file = afOpenFile(kTestFileName, "r", NULL);
	EXPECT_TRUE(file);

	int sampleFormat, sampleWidth;
	afGetSampleFormat(file, AF_DEFAULT_TRACK, &sampleFormat, &sampleWidth);
	EXPECT_EQ(sampleFormat, AF_SAMPFMT_TWOSCOMP);
	EXPECT_EQ(sampleWidth, 16);
	EXPECT_EQ(afGetChannels(file, AF_DEFAULT_TRACK), 1);
	EXPECT_EQ(afGetTrackBytes(file, AF_DEFAULT_TRACK),
		truncatedFrameCount * sizeof (int16_t));
	EXPECT_EQ(afGetFrameCount(file, AF_DEFAULT_TRACK), truncatedFrameCount);

	int16_t *data = new int16_t[truncatedFrameCount];
	AFframecount framesRead = afReadFrames(file, AF_DEFAULT_TRACK, data,
		truncatedFrameCount);
	EXPECT_EQ(framesRead, truncatedFrameCount);
	for (int i=0; i<truncatedFrameCount; i++)
		EXPECT_EQ(data[i], kFrames[i]);
	delete [] data;

	afCloseFile(file);

	::unlink(kTestFileName);
}

int main(int argc, char **argv)
{
	int tmp = mkstemp(kTestFileName);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

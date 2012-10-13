/*
	Audio File Library
	Copyright (C) 2012, Michael Pruett <michael@68k.org>

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
#include <stdlib.h>
#include <unistd.h>

#include "TestUtilities.h"

void runTest(int fileFormat, int compressionFormat)
{
	std::string testFileName;
	ASSERT_TRUE(createTemporaryFile("InvalidCompressionFormat", &testFileName));

	AFfilesetup setup = afNewFileSetup();
	afInitFileFormat(setup, fileFormat);
	afInitSampleFormat(setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
	afInitChannels(setup, AF_DEFAULT_TRACK, 1);
	afInitCompression(setup, AF_DEFAULT_TRACK, compressionFormat);
	ASSERT_TRUE(afOpenFile(testFileName.c_str(), "w", setup) == AF_NULL_FILEHANDLE);
	afFreeFileSetup(setup);

	ASSERT_EQ(::unlink(testFileName.c_str()), 0);
}

TEST(AIFF, mulaw) { runTest(AF_FILE_AIFF, AF_COMPRESSION_G711_ULAW); }
TEST(AIFF, Alaw) { runTest(AF_FILE_AIFF, AF_COMPRESSION_G711_ALAW); }
TEST(AIFF, IMA) { runTest(AF_FILE_AIFF, AF_COMPRESSION_IMA); }
TEST(AIFF, MSADPCM) { runTest(AF_FILE_AIFF, AF_COMPRESSION_MS_ADPCM); }

TEST(AIFFC, MSADPCM) { runTest(AF_FILE_AIFFC, AF_COMPRESSION_MS_ADPCM); }

TEST(NeXT, IMA) { runTest(AF_FILE_NEXTSND, AF_COMPRESSION_IMA); }
TEST(NeXT, MSADPCM) { runTest(AF_FILE_NEXTSND, AF_COMPRESSION_MS_ADPCM); }

TEST(IRCAM, IMA) { runTest(AF_FILE_BICSF, AF_COMPRESSION_IMA); }
TEST(IRCAM, MSADPCM) { runTest(AF_FILE_BICSF, AF_COMPRESSION_MS_ADPCM); }

TEST(AVR, mulaw) { runTest(AF_FILE_AVR, AF_COMPRESSION_G711_ULAW); }
TEST(AVR, Alaw) { runTest(AF_FILE_AVR, AF_COMPRESSION_G711_ALAW); }
TEST(AVR, IMA) { runTest(AF_FILE_AVR, AF_COMPRESSION_IMA); }
TEST(AVR, MSADPCM) { runTest(AF_FILE_AVR, AF_COMPRESSION_MS_ADPCM); }

TEST(IFF, mulaw) { runTest(AF_FILE_IFF_8SVX, AF_COMPRESSION_G711_ULAW); }
TEST(IFF, Alaw) { runTest(AF_FILE_IFF_8SVX, AF_COMPRESSION_G711_ALAW); }
TEST(IFF, IMA) { runTest(AF_FILE_IFF_8SVX, AF_COMPRESSION_IMA); }
TEST(IFF, MSADPCM) { runTest(AF_FILE_IFF_8SVX, AF_COMPRESSION_MS_ADPCM); }

TEST(SampleVision, mulaw) { runTest(AF_FILE_SAMPLEVISION, AF_COMPRESSION_G711_ULAW); }
TEST(SampleVision, Alaw) { runTest(AF_FILE_SAMPLEVISION, AF_COMPRESSION_G711_ALAW); }
TEST(SampleVision, IMA) { runTest(AF_FILE_SAMPLEVISION, AF_COMPRESSION_IMA); }
TEST(SampleVision, MSADPCM) { runTest(AF_FILE_SAMPLEVISION, AF_COMPRESSION_MS_ADPCM); }

TEST(VOC, IMA) { runTest(AF_FILE_VOC, AF_COMPRESSION_IMA); }
TEST(VOC, MSADPCM) { runTest(AF_FILE_VOC, AF_COMPRESSION_MS_ADPCM); }

TEST(NIST, IMA) { runTest(AF_FILE_NIST_SPHERE, AF_COMPRESSION_IMA); }
TEST(NIST, MSADPCM) { runTest(AF_FILE_NIST_SPHERE, AF_COMPRESSION_MS_ADPCM); }

TEST(CAF, MSADPCM) { runTest(AF_FILE_CAF, AF_COMPRESSION_MS_ADPCM); }

int main(int argc, char **argv)
{
	afSetErrorHandler(NULL);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

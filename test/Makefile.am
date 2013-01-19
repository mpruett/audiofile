INCLUDES = -I$(top_srcdir)/libaudiofile -I$(top_srcdir)

TESTS = \
	ADPCM \
	AES \
	ChannelMatrix \
	Error \
	FloatToInt \
	IntToFloat \
	InvalidCompressionFormat \
	InvalidSampleFormat \
	Large \
	Loop \
	Marker \
	Miscellaneous \
	NeXT \
	PCMData \
	PCMMapping \
	Pipe \
	Query \
	SampleFormat \
	Seek \
	Sign \
	VirtualFile \
	floatto24 \
	query2 \
	sixteen-to-eight \
	testchannelmatrix \
	testdouble \
	testfloat \
	testmarkers \
	twentyfour \
	twentyfour2 \
	writealaw \
	writeraw \
	writeulaw

check_PROGRAMS = \
	$(TESTS) \
	instparamtest \
	instparamwrite \
	printmarkers \
	testaupv

if ENABLE_VALGRIND
VALGRIND_FLAGS = --quiet --leak-check=full --show-reachable=yes --error-exitcode=1
TESTS_ENVIRONMENT = $(top_builddir)/libtool --mode=execute $(VALGRIND) $(VALGRIND_FLAGS)
endif

LIBAUDIOFILE = $(top_builddir)/libaudiofile/libaudiofile.la

LDADD = $(LIBAUDIOFILE)

DEPENDENCIES = $(LIBAUDIOFILE)

LIBGTEST = ../gtest/libgtest.la

ADPCM_SOURCES = ADPCM.cpp TestUtilities.cpp
ADPCM_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

AES_SOURCES = AES.cpp TestUtilities.cpp
AES_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

ChannelMatrix_SOURCES = ChannelMatrix.cpp TestUtilities.cpp
ChannelMatrix_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Error_SOURCES = Error.cpp TestUtilities.cpp
Error_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

FloatToInt_SOURCES = FloatToInt.cpp TestUtilities.cpp
FloatToInt_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

IntToFloat_SOURCES = IntToFloat.cpp TestUtilities.cpp
IntToFloat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

InvalidCompressionFormat_SOURCES = InvalidCompressionFormat.cpp TestUtilities.cpp
InvalidCompressionFormat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

InvalidSampleFormat_SOURCES = InvalidSampleFormat.cpp TestUtilities.cpp
InvalidSampleFormat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Large_SOURCES = Large.cpp TestUtilities.cpp
Large_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Loop_SOURCES = Loop.cpp TestUtilities.cpp
Loop_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Marker_SOURCES = Marker.cpp TestUtilities.cpp
Marker_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Miscellaneous_SOURCES = Miscellaneous.cpp TestUtilities.cpp
Miscellaneous_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

NeXT_SOURCES = NeXT.cpp TestUtilities.cpp
NeXT_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

PCMData_SOURCES = PCMData.cpp TestUtilities.cpp
PCMData_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

PCMMapping_SOURCES = PCMMapping.cpp TestUtilities.cpp
PCMMapping_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Pipe_SOURCES = Pipe.cpp TestUtilities.cpp
Pipe_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Query_SOURCES = Query.cpp TestUtilities.cpp
Query_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

SampleFormat_SOURCES = SampleFormat.cpp TestUtilities.cpp
SampleFormat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Seek_SOURCES = Seek.cpp TestUtilities.cpp
Seek_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Sign_SOURCES = Sign.cpp TestUtilities.cpp
Sign_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

VirtualFile_SOURCES = VirtualFile.cpp TestUtilities.cpp
VirtualFile_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

floatto24_SOURCES = floatto24.c TestUtilities.cpp

printmarkers_SOURCES = printmarkers.c
printmarkers_LDADD = $(LIBAUDIOFILE) -lm

sixteen_to_eight_SOURCES = sixteen-to-eight.c TestUtilities.cpp

testchannelmatrix_SOURCES = testchannelmatrix.c TestUtilities.cpp

testdouble_SOURCES = testdouble.c TestUtilities.cpp

testfloat_SOURCES = testfloat.c TestUtilities.cpp

testmarkers_SOURCES = testmarkers.c TestUtilities.cpp

twentyfour_SOURCES = twentyfour.c TestUtilities.cpp

twentyfour2_SOURCES = twentyfour2.c TestUtilities.cpp

writealaw_SOURCES = writealaw.c TestUtilities.cpp

writeraw_SOURCES = writeraw.c TestUtilities.cpp

writeulaw_SOURCES = writeulaw.c TestUtilities.cpp

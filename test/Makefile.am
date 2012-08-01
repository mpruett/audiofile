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
	Miscellaneous \
	NeXT \
	PCMData \
	PCMMapping \
	Pipe \
	Query \
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

ADPCM_SOURCES = ADPCM.cpp
ADPCM_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

AES_SOURCES = AES.cpp
AES_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

ChannelMatrix_SOURCES = ChannelMatrix.cpp
ChannelMatrix_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Error_SOURCES = Error.cpp
Error_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

FloatToInt_SOURCES = FloatToInt.cpp
FloatToInt_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

IntToFloat_SOURCES = IntToFloat.cpp
IntToFloat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

InvalidCompressionFormat_SOURCES = InvalidCompressionFormat.cpp
InvalidCompressionFormat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

InvalidSampleFormat_SOURCES = InvalidSampleFormat.cpp
InvalidSampleFormat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Large_SOURCES = Large.cpp
Large_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Loop_SOURCES = Loop.cpp
Loop_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Miscellaneous_SOURCES = Miscellaneous.cpp
Miscellaneous_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

NeXT_SOURCES = NeXT.cpp
NeXT_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

PCMData_SOURCES = PCMData.cpp
PCMData_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

PCMMapping_SOURCES = PCMMapping.cpp
PCMMapping_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Pipe_SOURCES = Pipe.cpp
Pipe_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Query_SOURCES = Query.cpp
Query_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Seek_SOURCES = Seek.cpp
Seek_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

Sign_SOURCES = Sign.cpp
Sign_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

VirtualFile_SOURCES = VirtualFile.cpp
VirtualFile_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

printmarkers_SOURCES = printmarkers.c
printmarkers_LDADD = $(LIBAUDIOFILE) -lm

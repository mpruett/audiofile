INCLUDES = -I. -I$(srcdir) -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	adddcoffset \
	bitdepth \
	copy \
	error \
	floatto24 \
	transparency \
	miscread \
	miscwrite \
	query2 \
	testaupv \
	writeaiff \
	writenext \
	writewave \
	writeulaw \
	writealaw \
	virtual \
	loop \
	instparamwrite \
	instparamtest \
	sixteen-to-eight \
	twentyfour \
	power \
	@TEST_BIN@

EXTRA_DIST = transparency.sh misc.sh

EXTRA_PROGRAMS = irixread irixtest irixtestloop linuxtest

TESTS = error writeaiff writenext writewave \
	writeulaw writealaw \
	misc.sh transparency.sh \
	query2 sixteen-to-eight twentyfour floatto24

irixread_SOURCES = irixread.c sgi.c
irixread_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

irixtest_SOURCES = irixtest.c sgi.c sgi.h
irixtest_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

irixtestloop_SOURCES = irixtestloop.c sgi.c
irixtestloop_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

linuxtest_SOURCES = linuxtest.c

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la

if BUILD_STATIC_LIBS
LDFLAGS=-static
endif
LIBS=

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la

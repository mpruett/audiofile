INCLUDES = -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	adddcoffset \
	copy \
	error \
	floatto24 \
	transparency \
	miscread \
	miscwrite \
	query2 \
	testaupv \
	writeaiff \
	writeircam \
	writenext \
	writewave \
	writeulaw \
	writealaw \
	virtual \
	loop \
	instparamwrite \
	instparamtest \
	sixteen-to-eight \
	testfloat \
	twentyfour \
	power \
	@TEST_BIN@

EXTRA_DIST = transparency.sh misc.sh

EXTRA_PROGRAMS = irixread irixtestloop linuxtest

TESTS = error writeaiff writeircam writenext writewave \
	writeulaw writealaw \
	misc.sh transparency.sh \
	query2 sixteen-to-eight twentyfour floatto24 testfloat

irixread_SOURCES = irixread.c sgi.c sgi.h
irixread_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

irixtestloop_SOURCES = irixtestloop.c sgi.c sgi.h
irixtestloop_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

linuxtest_SOURCES = linuxtest.c

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la

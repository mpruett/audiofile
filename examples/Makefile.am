INCLUDES = -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	adddcoffset \
	power \
	@TEST_BIN@

EXTRA_PROGRAMS = irixread irixtestloop linuxtest osxplay

irixread_SOURCES = irixread.c sgi.c sgi.h
irixread_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

irixtestloop_SOURCES = irixtestloop.c sgi.c sgi.h
irixtestloop_LDADD = $(top_builddir)/libaudiofile/libaudiofile.la @AUDIO_LIB@

linuxtest_SOURCES = linuxtest.c

osxplay_SOURCES = osxplay.c
osxplay_LDFLAGS = -framework AudioUnit -framework CoreServices

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la

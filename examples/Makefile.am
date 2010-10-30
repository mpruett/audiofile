INCLUDES = -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	adddcoffset \
	power \
	@TEST_BIN@

EXTRA_PROGRAMS = alsaplay irixread irixtestloop linuxtest osxplay

LIBAUDIOFILE = $(top_builddir)/libaudiofile/libaudiofile.la

alsaplay_SOURCES = alsaplay.cpp
alsaplay_LDADD = $(LIBAUDIOFILE) -lasound

irixread_SOURCES = irixread.c sgi.c sgi.h
irixread_LDADD = $(LIBAUDIOFILE) @AUDIO_LIB@

irixtestloop_SOURCES = irixtestloop.c sgi.c sgi.h
irixtestloop_LDADD = $(LIBAUDIOFILE) @AUDIO_LIB@

linuxtest_SOURCES = linuxtest.c

osxplay_SOURCES = osxplay.c
osxplay_LDFLAGS = -framework AudioUnit -framework CoreServices

LDADD = $(LIBAUDIOFILE)

DEPENDENCIES = $(LIBAUDIOFILE)

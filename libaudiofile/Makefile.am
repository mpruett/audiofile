## Process this file with automake to produce Makefile.in

SUBDIRS = modules

lib_LTLIBRARIES = libaudiofile.la

EXTRA_DIST = audiofile.exports

libaudiofile_la_SOURCES = \
	openclose.cpp setup.c format.cpp data.cpp pcm.cpp \
	error.c byteorder.h af_vfs.cpp \
	util.cpp util.h debug.c aupv.c units.c compression.c \
	aes.c instrument.c loop.c marker.c misc.c track.cpp query.c \
	raw.c raw.h \
	aiff.c aiffwrite.c extended.c aiff.h \
	next.c nextwrite.c next.h \
	wave.c wavewrite.c wave.h \
	ircam.c ircamwrite.c ircam.h \
	avr.c avrwrite.c avr.h \
	iff.c iffwrite.c iff.h \
	nist.c nistwrite.c nist.h \
	g711.c g711.h \
	afinternal.h aupvinternal.h aupvlist.h byteorder.h \
	compression.h error.h extended.h instrument.h marker.h \
	pcm.h setup.h track.h units.h \
	print.h util.h debug.h \
	File.cpp File.h Shared.h \
	AudioFormat.cpp

libaudiofile_la_LIBADD = modules/libmodules.la $(COVERAGE_LIBS)

libaudiofile_la_LDFLAGS = -version-info 0:2:0 -no-undefined \
	-export-symbols $(srcdir)/audiofile.exports

include_HEADERS = audiofile.h aupvlist.h af_vfs.h

# GNU gcc
# AM_CFLAGS = -Wall -g
# SGI MIPSpro cc
# AM_CFLAGS = -fullwarn -g
# No debugging.
AM_CFLAGS = -DNDEBUG -Wall $(COVERAGE_CFLAGS)
AM_CXXFLAGS = $(AM_CFLAGS) -fno-rtti -fno-exceptions

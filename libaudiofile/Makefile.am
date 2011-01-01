## Process this file with automake to produce Makefile.in

SUBDIRS = modules

lib_LTLIBRARIES = libaudiofile.la

EXTRA_DIST = audiofile.exports

libaudiofile_la_SOURCES = \
	openclose.cpp Setup.cpp Setup.h format.cpp data.cpp pcm.cpp \
	error.c byteorder.h af_vfs.cpp \
	util.cpp util.h debug.cpp aupv.c units.cpp compression.cpp \
	aes.cpp Instrument.cpp Instrument.h Loop.cpp Marker.cpp Marker.h \
	Miscellaneous.cpp Track.cpp Track.h query.cpp \
	Raw.cpp Raw.h \
	AIFF.cpp AIFFWrite.cpp AIFF.h extended.c \
	NeXT.cpp NeXTWrite.cpp NeXT.h \
	WAVE.cpp WAVEWrite.cpp WAVE.h \
	IRCAM.cpp IRCAMWrite.cpp IRCAM.h \
	AVR.cpp AVRWrite.cpp AVR.h \
	IFF.cpp IFFWrite.cpp IFF.h \
	NIST.cpp NISTWrite.cpp NIST.h \
	g711.c g711.h \
	afinternal.h aupvinternal.h aupvlist.h byteorder.h \
	compression.h error.h extended.h \
	pcm.h units.h \
	print.h util.h debug.h \
	File.cpp File.h Shared.h \
	AudioFormat.cpp AudioFormat.h FileHandle.cpp

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

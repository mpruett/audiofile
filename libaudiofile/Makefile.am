## Process this file with automake to produce Makefile.in

SUBDIRS = modules

lib_LTLIBRARIES = libaudiofile.la

libaudiofile_la_SOURCES = \
	openclose.c setup.c format.c data.c pcm.c \
	error.c byteorder.c af_vfs.c \
	util.c aupv.c units.c compression.c \
	aes.c instrument.c loop.c marker.c misc.c track.c query.c \
	raw.c raw.h \
	aiff.c aiffwrite.c extended.c aiff.h \
	next.c nextwrite.c next.h \
	wave.c wavewrite.c wave.h \
	ircam.c ircamwrite.c ircam.h \
	g711.c g711.h \
	afinternal.h aupvinternal.h aupvlist.h byteorder.h \
	compression.h error.h extended.h instrument.h marker.h \
	pcm.h setup.h track.h units.h \
	print.h util.h \
	modules.c modules.h

libaudiofile_la_LIBADD = modules/libmodules.la

libaudiofile_la_LDFLAGS = -version-info 0:2:0

include_HEADERS = audiofile.h aupvlist.h af_vfs.h

# GNU gcc
# AM_CFLAGS = -Wall -g
# SGI MIPSpro cc
# AM_CFLAGS = -fullwarn -g
# No debugging.
AM_CFLAGS = -DNDEBUG

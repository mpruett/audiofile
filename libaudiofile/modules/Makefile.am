noinst_LTLIBRARIES = libmodules.la

INCLUDES = -I$(srcdir)/..

libmodules_la_SOURCES = \
	g711.c g711.h \
	pcm.c pcm.h \
	rebuffer.c rebuffer.h \
	rebuffer.template

# GNU gcc
# AM_CFLAGS = -Wall -g
# SGI MIPSpro cc
# AM_CFLAGS = -fullwarn -g
# No debugging.
AM_CFLAGS = -DNDEBUG

INCLUDES = -I. -I$(srcdir) -I$(top_srcdir)/libaudiofile

EXTRA_DIST = README

bin_PROGRAMS = sfconvert sfinfo

sfconvert_SOURCES = sfconvert.c printinfo.c

sfinfo_SOURCES = sfinfo.c printinfo.c

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la
if BUILD_STATIC_LIBS
LDFLAGS=-static
endif

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la

INCLUDES = -I$(top_srcdir)/libaudiofile

EXTRA_DIST = README

bin_PROGRAMS = sfconvert sfinfo

sfconvert_SOURCES = sfconvert.c printinfo.c

sfinfo_SOURCES = sfinfo.c printinfo.c

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la

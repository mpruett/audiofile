## Process this file with automake to produce Makefile.in

bin_SCRIPTS = \
         audiofile-config

SRC_SUBDIRS = libaudiofile sfcommands test examples
SUBDIRS = $(SRC_SUBDIRS) docs win32

EXTRA_DIST= \
	ACKNOWLEDGEMENTS \
	NOTES \
	README \
	TODO \
	COPYING.GPL \
	configure configure.in\
	audiofile.m4 \
	audiofile.spec.in \
	audiofile.pc.in

m4datadir = $(datadir)/aclocal
m4data_DATA = audiofile.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audiofile.pc

dist-hook: audiofile.spec
	cp audiofile.spec $(distdir)

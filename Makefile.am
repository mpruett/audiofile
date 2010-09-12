## Process this file with automake to produce Makefile.in

SRC_SUBDIRS = libaudiofile sfcommands test examples
SUBDIRS = $(SRC_SUBDIRS) docs win32

EXTRA_DIST = \
	ACKNOWLEDGEMENTS \
	NOTES \
	README \
	TODO \
	COPYING.GPL \
	configure configure.ac \
	audiofile.spec.in \
	audiofile.pc.in \
	audiofile-uninstalled.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = audiofile.pc

dist-hook: audiofile.spec
	cp audiofile.spec $(distdir)

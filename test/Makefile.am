INCLUDES = -I$(top_srcdir)/libaudiofile

noinst_PROGRAMS = \
	error \
	floatto24 \
	transparency \
	miscread \
	miscwrite \
	query2 \
	testaupv \
	writeaiff \
	writeircam \
	writenext \
	writewave \
	writeraw \
	writeulaw \
	writealaw \
	loop \
	instparamwrite \
	instparamtest \
	sixteen-to-eight \
	testfloat \
	twentyfour \
	pipe \
	testmarkers \
	testchannelmatrix \
	printmarkers \
	seek

EXTRA_DIST = transparency.sh misc.sh pipe.sh

TESTS = error writeaiff writeircam writenext writewave writeraw \
	writeulaw writealaw \
	misc.sh transparency.sh pipe.sh \
	query2 sixteen-to-eight twentyfour floatto24 \
	testfloat testmarkers testchannelmatrix seek

LDADD = $(top_builddir)/libaudiofile/libaudiofile.la
LDFLAGS = -static

DEPENDENCIES = $(top_builddir)/libaudiofile/libaudiofile.la

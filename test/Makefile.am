SUBDIRS = gtest

INCLUDES = -I$(top_srcdir)/libaudiofile

TESTS = \
	aes \
	channelmatrix \
	error \
	floatto24 \
	floattoint \
	inttofloat \
	large \
	loop \
	miscellaneous \
	next \
	pcmdata \
	pcmmapping \
	pipe \
	query2 \
	seek \
	sign \
	sixteen-to-eight \
	testchannelmatrix \
	testdouble \
	testfloat \
	testmarkers \
	twentyfour \
	twentyfour2 \
	virtualfile \
	writealaw \
	writeraw \
	writeulaw

check_PROGRAMS = \
	$(TESTS) \
	instparamtest \
	instparamwrite \
	printmarkers \
	testaupv

if ENABLE_VALGRIND
VALGRIND_FLAGS = --quiet --leak-check=full --show-reachable=yes --error-exitcode=1
TESTS_ENVIRONMENT = $(top_builddir)/libtool --mode=execute $(VALGRIND) $(VALGRIND_FLAGS)
endif

LIBAUDIOFILE = $(top_builddir)/libaudiofile/libaudiofile.la

LDADD = $(LIBAUDIOFILE)

DEPENDENCIES = $(LIBAUDIOFILE)

LIBGTEST = gtest/libgtest.la

aes_SOURCES = aes.cpp
aes_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

channelmatrix_SOURCES = channelmatrix.cpp
channelmatrix_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

error_SOURCES = error.cpp
error_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

floattoint_SOURCES = floattoint.cpp
floattoint_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

inttofloat_SOURCES = inttofloat.cpp
inttofloat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

large_SOURCES = large.cpp
large_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

loop_SOURCES = loop.cpp
loop_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

miscellaneous_SOURCES = miscellaneous.cpp
miscellaneous_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

next_SOURCES = next.cpp
next_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

pcmdata_SOURCES = pcmdata.cpp
pcmdata_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

pcmmapping_SOURCES = pcmmapping.cpp
pcmmapping_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

pipe_SOURCES = pipe.cpp
pipe_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

printmarkers_SOURCES = printmarkers.c
printmarkers_LDADD = $(LIBAUDIOFILE) -lm

seek_SOURCES = seek.cpp
seek_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

sign_SOURCES = sign.cpp
sign_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

virtualfile_SOURCES = virtualfile.cpp
virtualfile_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

coverage-reset:
	$(LCOV) --base-directory=@top_srcdir@ --directory @top_srcdir@/libaudiofile --zerocounters

coverage-report:
	$(LCOV) --directory @top_srcdir@/libaudiofile \
		--capture \
		--output-file @top_builddir@/lcov.info

	$(LCOV) --directory @top_srcdir@/libaudiofile \
		--output-file @top_builddir@/lcov.info \
		--remove @top_builddir@/lcov.info \
		"/usr/include/*"

	$(mkdir_p) @top_builddir@/test/coverage
	git_commit=`GIT_DIR=@top_srcdir@/.git git log -1 --pretty=format:%h 2>/dev/null`; \
	$(GENHTML) --title "@PACKAGE@ @VERSION@ $$git_commit" \
		--output-directory @top_builddir@/test/coverage @top_builddir@/lcov.info
	@echo
	@echo 'lcov report can be found here:'
	@echo 'file://@abs_top_builddir@/test/coverage/index.html'
	@echo

clean-local:
	-rm -rf coverage

.PHONY: coverage-report

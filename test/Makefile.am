INCLUDES = -I$(top_srcdir)/libaudiofile

check_PROGRAMS = \
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
	writeavr \
	writeiff \
	writenist \
	writeulaw \
	writealaw \
	loop \
	instparamwrite \
	instparamtest \
	sixteen-to-eight \
	testfloat \
	testdouble \
	twentyfour \
	twentyfour2 \
	pipe \
	testmarkers \
	testchannelmatrix \
	printmarkers \
	seek \
	large \
	sign \
	inttofloat \
	floattoint \
	pcmmapping

EXTRA_DIST = transparency.sh misc.sh pipe.sh

TESTS = error writeaiff writeircam writenext writewave writeraw \
	writeavr writeiff writenist \
	writeulaw writealaw \
	misc.sh transparency.sh pipe.sh \
	query2 sixteen-to-eight twentyfour twentyfour2 floatto24 \
	testfloat testdouble testmarkers testchannelmatrix seek large \
	sign inttofloat floattoint pcmmapping

LIBAUDIOFILE = $(top_builddir)/libaudiofile/libaudiofile.la

LDADD = $(LIBAUDIOFILE)

DEPENDENCIES = $(LIBAUDIOFILE)

LIBGTEST = -lgtest

sign_SOURCES = sign.cpp
sign_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

inttofloat_SOURCES = inttofloat.cpp
inttofloat_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

floattoint_SOURCES = floattoint.cpp
floattoint_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

pcmmapping_SOURCES = pcmmapping.cpp
pcmmapping_LDADD = $(LIBGTEST) $(LIBAUDIOFILE)

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

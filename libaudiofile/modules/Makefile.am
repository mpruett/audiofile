noinst_LTLIBRARIES = libmodules.la

INCLUDES = -I$(srcdir)/..

libmodules_la_SOURCES = \
	FileModule.cpp \
	FileModule.h \
	G711.cpp \
	G711.h \
	IMA.cpp \
	IMA.h \
	Module.cpp \
	Module.h \
	ModuleState.cpp \
	ModuleState.h \
	MSADPCM.cpp \
	MSADPCM.h \
	PCM.cpp \
	PCM.h \
	RebufferModule.cpp \
	RebufferModule.h \
	SimpleModule.cpp \
	SimpleModule.h

# GNU gcc
# AM_CFLAGS = -Wall -g
# SGI MIPSpro cc
# AM_CFLAGS = -fullwarn -g
# No debugging.
AM_CFLAGS = -DNDEBUG -Wall $(WERROR_CFLAGS) $(COVERAGE_CFLAGS) -fvisibility=hidden
AM_CXXFLAGS = $(AM_CFLAGS) -fno-rtti -fno-exceptions -fvisibility-inlines-hidden

CLEANFILES = *.gcda *.gcno

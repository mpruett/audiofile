# Microsoft Developer Studio Generated NMAKE File, Based on dynmodules.dsp
!IF "$(CFG)" == ""
CFG=dynmodules - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dynmodules - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dynmodules - Win32 Release" && "$(CFG)" != "dynmodules - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dynmodules.mak" CFG="dynmodules - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dynmodules - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dynmodules - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dynmodules - Win32 Release"

OUTDIR=.\Release\dynmodules
INTDIR=.\Release\dynmodules
# Begin Custom Macros
OutDir=.\Release\dynmodules
# End Custom Macros

ALL : "$(OUTDIR)\dynmodules.lib"


CLEAN :
	-@erase "$(INTDIR)\adpcm.obj"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\ima.obj"
	-@erase "$(INTDIR)\msadpcm.obj"
	-@erase "$(INTDIR)\pcm.obj"
	-@erase "$(INTDIR)\rebuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\dynmodules.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /D "HAVE_CONFIG_H" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dynmodules.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dynmodules.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adpcm.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\ima.obj" \
	"$(INTDIR)\msadpcm.obj" \
	"$(INTDIR)\pcm.obj" \
	"$(INTDIR)\rebuffer.obj"

"$(OUTDIR)\dynmodules.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dynmodules - Win32 Debug"

OUTDIR=.\Debug\dynmodules
INTDIR=.\Debug\dynmodules
# Begin Custom Macros
OutDir=.\Debug\dynmodules
# End Custom Macros

ALL : "$(OUTDIR)\dynmodules.lib"


CLEAN :
	-@erase "$(INTDIR)\adpcm.obj"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\ima.obj"
	-@erase "$(INTDIR)\msadpcm.obj"
	-@erase "$(INTDIR)\pcm.obj"
	-@erase "$(INTDIR)\rebuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dynmodules.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /D "HAVE_CONFIG_H" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dynmodules.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dynmodules.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adpcm.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\ima.obj" \
	"$(INTDIR)\msadpcm.obj" \
	"$(INTDIR)\pcm.obj" \
	"$(INTDIR)\rebuffer.obj"

"$(OUTDIR)\dynmodules.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dynmodules.dep")
!INCLUDE "dynmodules.dep"
!ELSE 
!MESSAGE Warning: cannot find "dynmodules.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dynmodules - Win32 Release" || "$(CFG)" == "dynmodules - Win32 Debug"
SOURCE=..\libaudiofile\modules\adpcm.c

"$(INTDIR)\adpcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\libaudiofile\modules\g711.c

"$(INTDIR)\g711.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\libaudiofile\modules\ima.c

"$(INTDIR)\ima.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\libaudiofile\modules\msadpcm.c

"$(INTDIR)\msadpcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\libaudiofile\modules\pcm.c

"$(INTDIR)\pcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\libaudiofile\modules\rebuffer.c

"$(INTDIR)\rebuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 


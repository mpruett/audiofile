# Microsoft Developer Studio Generated NMAKE File, Based on modules.dsp
!IF "$(CFG)" == ""
CFG=modules - Win32 Debug
!MESSAGE No configuration specified. Defaulting to modules - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "modules - Win32 Release" && "$(CFG)" != "modules - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "modules.mak" CFG="modules - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "modules - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "modules - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "modules - Win32 Release"

OUTDIR=.\Release\staticmod
INTDIR=.\Release\staticmod
# Begin Custom Macros
OutDir=.\Release\staticmod
# End Custom Macros

ALL : "$(OUTDIR)\modules.lib"


CLEAN :
	-@erase "$(INTDIR)\adpcm.obj"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\ima.obj"
	-@erase "$(INTDIR)\msadpcm.obj"
	-@erase "$(INTDIR)\pcm.obj"
	-@erase "$(INTDIR)\rebuffer.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\modules.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\modules.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\modules.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\modules.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adpcm.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\ima.obj" \
	"$(INTDIR)\msadpcm.obj" \
	"$(INTDIR)\pcm.obj" \
	"$(INTDIR)\rebuffer.obj"

"$(OUTDIR)\modules.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "modules - Win32 Debug"

OUTDIR=.\Debug\staticmod
INTDIR=.\Debug\staticmod
# Begin Custom Macros
OutDir=.\Debug\staticmod
# End Custom Macros

ALL : "$(OUTDIR)\modulesD.lib" "$(OUTDIR)\modules.bsc"


CLEAN :
	-@erase "$(INTDIR)\adpcm.obj"
	-@erase "$(INTDIR)\adpcm.sbr"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\g711.sbr"
	-@erase "$(INTDIR)\ima.obj"
	-@erase "$(INTDIR)\ima.sbr"
	-@erase "$(INTDIR)\msadpcm.obj"
	-@erase "$(INTDIR)\msadpcm.sbr"
	-@erase "$(INTDIR)\pcm.obj"
	-@erase "$(INTDIR)\pcm.sbr"
	-@erase "$(INTDIR)\rebuffer.obj"
	-@erase "$(INTDIR)\rebuffer.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\modules.bsc"
	-@erase "$(OUTDIR)\modulesD.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\modules.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\adpcm.sbr" \
	"$(INTDIR)\g711.sbr" \
	"$(INTDIR)\ima.sbr" \
	"$(INTDIR)\msadpcm.sbr" \
	"$(INTDIR)\pcm.sbr" \
	"$(INTDIR)\rebuffer.sbr"

"$(OUTDIR)\modules.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\modulesD.lib" 
LIB32_OBJS= \
	"$(INTDIR)\adpcm.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\ima.obj" \
	"$(INTDIR)\msadpcm.obj" \
	"$(INTDIR)\pcm.obj" \
	"$(INTDIR)\rebuffer.obj"

"$(OUTDIR)\modulesD.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("modules.dep")
!INCLUDE "modules.dep"
!ELSE 
!MESSAGE Warning: cannot find "modules.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "modules - Win32 Release" || "$(CFG)" == "modules - Win32 Debug"
SOURCE=..\libaudiofile\modules\adpcm.c

!IF  "$(CFG)" == "modules - Win32 Release"


"$(INTDIR)\adpcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modules - Win32 Debug"


"$(INTDIR)\adpcm.obj"	"$(INTDIR)\adpcm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\modules\g711.c

!IF  "$(CFG)" == "modules - Win32 Release"


"$(INTDIR)\g711.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modules - Win32 Debug"


"$(INTDIR)\g711.obj"	"$(INTDIR)\g711.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\modules\ima.c

!IF  "$(CFG)" == "modules - Win32 Release"


"$(INTDIR)\ima.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modules - Win32 Debug"


"$(INTDIR)\ima.obj"	"$(INTDIR)\ima.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\modules\msadpcm.c

!IF  "$(CFG)" == "modules - Win32 Release"


"$(INTDIR)\msadpcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modules - Win32 Debug"


"$(INTDIR)\msadpcm.obj"	"$(INTDIR)\msadpcm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\modules\pcm.c

!IF  "$(CFG)" == "modules - Win32 Release"


"$(INTDIR)\pcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modules - Win32 Debug"


"$(INTDIR)\pcm.obj"	"$(INTDIR)\pcm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\modules\rebuffer.c

!IF  "$(CFG)" == "modules - Win32 Release"


"$(INTDIR)\rebuffer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "modules - Win32 Debug"


"$(INTDIR)\rebuffer.obj"	"$(INTDIR)\rebuffer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 


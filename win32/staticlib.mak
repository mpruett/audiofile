# Microsoft Developer Studio Generated NMAKE File, Based on staticlib.dsp
!IF "$(CFG)" == ""
CFG=staticlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to staticlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "staticlib - Win32 Release" && "$(CFG)" != "staticlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "staticlib.mak" CFG="staticlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "staticlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "staticlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "staticlib - Win32 Release"

OUTDIR=.\Release\staticlib
INTDIR=.\Release\staticlib

!IF "$(RECURSE)" == "0" 

ALL : ".\lib\libaudiofile.lib"

!ELSE 

ALL : "modules - Win32 Release" ".\lib\libaudiofile.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"modules - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\aes.obj"
	-@erase "$(INTDIR)\af_vfs.obj"
	-@erase "$(INTDIR)\aiff.obj"
	-@erase "$(INTDIR)\aiffwrite.obj"
	-@erase "$(INTDIR)\aupv.obj"
	-@erase "$(INTDIR)\avr.obj"
	-@erase "$(INTDIR)\avrwrite.obj"
	-@erase "$(INTDIR)\byteorder.obj"
	-@erase "$(INTDIR)\compression.obj"
	-@erase "$(INTDIR)\data.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\extended.obj"
	-@erase "$(INTDIR)\format.obj"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\iff.obj"
	-@erase "$(INTDIR)\iffwrite.obj"
	-@erase "$(INTDIR)\instrument.obj"
	-@erase "$(INTDIR)\ircam.obj"
	-@erase "$(INTDIR)\ircamwrite.obj"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\marker.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\modules.obj"
	-@erase "$(INTDIR)\next.obj"
	-@erase "$(INTDIR)\nextwrite.obj"
	-@erase "$(INTDIR)\nist.obj"
	-@erase "$(INTDIR)\nistwrite.obj"
	-@erase "$(INTDIR)\openclose.obj"
	-@erase "$(INTDIR)\pcm.obj"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\setup.obj"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(INTDIR)\wavewrite.obj"
	-@erase ".\lib\libaudiofile.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\staticlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lib\libaudiofile.lib" 
LIB32_OBJS= \
	"$(INTDIR)\aes.obj" \
	"$(INTDIR)\af_vfs.obj" \
	"$(INTDIR)\aiff.obj" \
	"$(INTDIR)\aiffwrite.obj" \
	"$(INTDIR)\aupv.obj" \
	"$(INTDIR)\byteorder.obj" \
	"$(INTDIR)\compression.obj" \
	"$(INTDIR)\data.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\extended.obj" \
	"$(INTDIR)\format.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\instrument.obj" \
	"$(INTDIR)\ircam.obj" \
	"$(INTDIR)\ircamwrite.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\marker.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\modules.obj" \
	"$(INTDIR)\next.obj" \
	"$(INTDIR)\nextwrite.obj" \
	"$(INTDIR)\nist.obj" \
	"$(INTDIR)\openclose.obj" \
	"$(INTDIR)\pcm.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\raw.obj" \
	"$(INTDIR)\setup.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\wavewrite.obj" \
	"$(INTDIR)\nistwrite.obj" \
	"$(INTDIR)\iffwrite.obj" \
	"$(INTDIR)\iff.obj" \
	"$(INTDIR)\avrwrite.obj" \
	"$(INTDIR)\avr.obj" \
	".\Release\staticmod\modules.lib"

".\lib\libaudiofile.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"

OUTDIR=.\Debug\staticlib
INTDIR=.\Debug\staticlib
# Begin Custom Macros
OutDir=.\Debug\staticlib
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\lib\libaudiofileD.lib" "$(OUTDIR)\staticlib.bsc"

!ELSE 

ALL : "modules - Win32 Debug" ".\lib\libaudiofileD.lib" "$(OUTDIR)\staticlib.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"modules - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\aes.obj"
	-@erase "$(INTDIR)\aes.sbr"
	-@erase "$(INTDIR)\af_vfs.obj"
	-@erase "$(INTDIR)\af_vfs.sbr"
	-@erase "$(INTDIR)\aiff.obj"
	-@erase "$(INTDIR)\aiff.sbr"
	-@erase "$(INTDIR)\aiffwrite.obj"
	-@erase "$(INTDIR)\aiffwrite.sbr"
	-@erase "$(INTDIR)\aupv.obj"
	-@erase "$(INTDIR)\aupv.sbr"
	-@erase "$(INTDIR)\avr.obj"
	-@erase "$(INTDIR)\avr.sbr"
	-@erase "$(INTDIR)\avrwrite.obj"
	-@erase "$(INTDIR)\avrwrite.sbr"
	-@erase "$(INTDIR)\byteorder.obj"
	-@erase "$(INTDIR)\byteorder.sbr"
	-@erase "$(INTDIR)\compression.obj"
	-@erase "$(INTDIR)\compression.sbr"
	-@erase "$(INTDIR)\data.obj"
	-@erase "$(INTDIR)\data.sbr"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\error.sbr"
	-@erase "$(INTDIR)\extended.obj"
	-@erase "$(INTDIR)\extended.sbr"
	-@erase "$(INTDIR)\format.obj"
	-@erase "$(INTDIR)\format.sbr"
	-@erase "$(INTDIR)\g711.obj"
	-@erase "$(INTDIR)\g711.sbr"
	-@erase "$(INTDIR)\iff.obj"
	-@erase "$(INTDIR)\iff.sbr"
	-@erase "$(INTDIR)\iffwrite.obj"
	-@erase "$(INTDIR)\iffwrite.sbr"
	-@erase "$(INTDIR)\instrument.obj"
	-@erase "$(INTDIR)\instrument.sbr"
	-@erase "$(INTDIR)\ircam.obj"
	-@erase "$(INTDIR)\ircam.sbr"
	-@erase "$(INTDIR)\ircamwrite.obj"
	-@erase "$(INTDIR)\ircamwrite.sbr"
	-@erase "$(INTDIR)\loop.obj"
	-@erase "$(INTDIR)\loop.sbr"
	-@erase "$(INTDIR)\marker.obj"
	-@erase "$(INTDIR)\marker.sbr"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\misc.sbr"
	-@erase "$(INTDIR)\modules.obj"
	-@erase "$(INTDIR)\modules.sbr"
	-@erase "$(INTDIR)\next.obj"
	-@erase "$(INTDIR)\next.sbr"
	-@erase "$(INTDIR)\nextwrite.obj"
	-@erase "$(INTDIR)\nextwrite.sbr"
	-@erase "$(INTDIR)\nist.obj"
	-@erase "$(INTDIR)\nist.sbr"
	-@erase "$(INTDIR)\nistwrite.obj"
	-@erase "$(INTDIR)\nistwrite.sbr"
	-@erase "$(INTDIR)\openclose.obj"
	-@erase "$(INTDIR)\openclose.sbr"
	-@erase "$(INTDIR)\pcm.obj"
	-@erase "$(INTDIR)\pcm.sbr"
	-@erase "$(INTDIR)\query.obj"
	-@erase "$(INTDIR)\query.sbr"
	-@erase "$(INTDIR)\raw.obj"
	-@erase "$(INTDIR)\raw.sbr"
	-@erase "$(INTDIR)\setup.obj"
	-@erase "$(INTDIR)\setup.sbr"
	-@erase "$(INTDIR)\track.obj"
	-@erase "$(INTDIR)\track.sbr"
	-@erase "$(INTDIR)\units.obj"
	-@erase "$(INTDIR)\units.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wave.obj"
	-@erase "$(INTDIR)\wave.sbr"
	-@erase "$(INTDIR)\wavewrite.obj"
	-@erase "$(INTDIR)\wavewrite.sbr"
	-@erase "$(OUTDIR)\staticlib.bsc"
	-@erase ".\lib\libaudiofileD.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_CONFIG_H" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\staticlib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\aes.sbr" \
	"$(INTDIR)\af_vfs.sbr" \
	"$(INTDIR)\aiff.sbr" \
	"$(INTDIR)\aiffwrite.sbr" \
	"$(INTDIR)\aupv.sbr" \
	"$(INTDIR)\byteorder.sbr" \
	"$(INTDIR)\compression.sbr" \
	"$(INTDIR)\data.sbr" \
	"$(INTDIR)\error.sbr" \
	"$(INTDIR)\extended.sbr" \
	"$(INTDIR)\format.sbr" \
	"$(INTDIR)\g711.sbr" \
	"$(INTDIR)\instrument.sbr" \
	"$(INTDIR)\ircam.sbr" \
	"$(INTDIR)\ircamwrite.sbr" \
	"$(INTDIR)\loop.sbr" \
	"$(INTDIR)\marker.sbr" \
	"$(INTDIR)\misc.sbr" \
	"$(INTDIR)\modules.sbr" \
	"$(INTDIR)\next.sbr" \
	"$(INTDIR)\nextwrite.sbr" \
	"$(INTDIR)\nist.sbr" \
	"$(INTDIR)\openclose.sbr" \
	"$(INTDIR)\pcm.sbr" \
	"$(INTDIR)\query.sbr" \
	"$(INTDIR)\raw.sbr" \
	"$(INTDIR)\setup.sbr" \
	"$(INTDIR)\track.sbr" \
	"$(INTDIR)\units.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\wave.sbr" \
	"$(INTDIR)\wavewrite.sbr" \
	"$(INTDIR)\nistwrite.sbr" \
	"$(INTDIR)\iffwrite.sbr" \
	"$(INTDIR)\iff.sbr" \
	"$(INTDIR)\avrwrite.sbr" \
	"$(INTDIR)\avr.sbr"

"$(OUTDIR)\staticlib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"lib\libaudiofileD.lib" 
LIB32_OBJS= \
	"$(INTDIR)\aes.obj" \
	"$(INTDIR)\af_vfs.obj" \
	"$(INTDIR)\aiff.obj" \
	"$(INTDIR)\aiffwrite.obj" \
	"$(INTDIR)\aupv.obj" \
	"$(INTDIR)\byteorder.obj" \
	"$(INTDIR)\compression.obj" \
	"$(INTDIR)\data.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\extended.obj" \
	"$(INTDIR)\format.obj" \
	"$(INTDIR)\g711.obj" \
	"$(INTDIR)\instrument.obj" \
	"$(INTDIR)\ircam.obj" \
	"$(INTDIR)\ircamwrite.obj" \
	"$(INTDIR)\loop.obj" \
	"$(INTDIR)\marker.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\modules.obj" \
	"$(INTDIR)\next.obj" \
	"$(INTDIR)\nextwrite.obj" \
	"$(INTDIR)\nist.obj" \
	"$(INTDIR)\openclose.obj" \
	"$(INTDIR)\pcm.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\raw.obj" \
	"$(INTDIR)\setup.obj" \
	"$(INTDIR)\track.obj" \
	"$(INTDIR)\units.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\wave.obj" \
	"$(INTDIR)\wavewrite.obj" \
	"$(INTDIR)\nistwrite.obj" \
	"$(INTDIR)\iffwrite.obj" \
	"$(INTDIR)\iff.obj" \
	"$(INTDIR)\avrwrite.obj" \
	"$(INTDIR)\avr.obj" \
	".\Debug\staticmod\modulesD.lib"

".\lib\libaudiofileD.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("staticlib.dep")
!INCLUDE "staticlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "staticlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "staticlib - Win32 Release" || "$(CFG)" == "staticlib - Win32 Debug"
SOURCE=..\libaudiofile\aes.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\aes.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\aes.obj"	"$(INTDIR)\aes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\af_vfs.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\af_vfs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\af_vfs.obj"	"$(INTDIR)\af_vfs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\aiff.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\aiff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\aiff.obj"	"$(INTDIR)\aiff.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\aiffwrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\aiffwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\aiffwrite.obj"	"$(INTDIR)\aiffwrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\aupv.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\aupv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\aupv.obj"	"$(INTDIR)\aupv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\avr.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\avr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\avr.obj"	"$(INTDIR)\avr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\avrwrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\avrwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\avrwrite.obj"	"$(INTDIR)\avrwrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\byteorder.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\byteorder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\byteorder.obj"	"$(INTDIR)\byteorder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\compression.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\compression.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\compression.obj"	"$(INTDIR)\compression.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\data.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\data.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\data.obj"	"$(INTDIR)\data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\error.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\error.obj"	"$(INTDIR)\error.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\extended.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\extended.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\extended.obj"	"$(INTDIR)\extended.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\format.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\format.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\format.obj"	"$(INTDIR)\format.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\g711.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\g711.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\g711.obj"	"$(INTDIR)\g711.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\iff.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\iff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\iff.obj"	"$(INTDIR)\iff.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\iffwrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\iffwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\iffwrite.obj"	"$(INTDIR)\iffwrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\instrument.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\instrument.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\instrument.obj"	"$(INTDIR)\instrument.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\ircam.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\ircam.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\ircam.obj"	"$(INTDIR)\ircam.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\ircamwrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\ircamwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\ircamwrite.obj"	"$(INTDIR)\ircamwrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\loop.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\loop.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\loop.obj"	"$(INTDIR)\loop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\marker.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\marker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\marker.obj"	"$(INTDIR)\marker.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\misc.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\misc.obj"	"$(INTDIR)\misc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\modules.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\modules.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\modules.obj"	"$(INTDIR)\modules.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\next.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\next.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\next.obj"	"$(INTDIR)\next.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\nextwrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\nextwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\nextwrite.obj"	"$(INTDIR)\nextwrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\nist.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\nist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\nist.obj"	"$(INTDIR)\nist.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\nistwrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\nistwrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\nistwrite.obj"	"$(INTDIR)\nistwrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\openclose.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\openclose.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\openclose.obj"	"$(INTDIR)\openclose.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\pcm.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\pcm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\pcm.obj"	"$(INTDIR)\pcm.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\query.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\query.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\query.obj"	"$(INTDIR)\query.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\raw.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\raw.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\raw.obj"	"$(INTDIR)\raw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\setup.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\setup.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\setup.obj"	"$(INTDIR)\setup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\track.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\track.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\track.obj"	"$(INTDIR)\track.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\units.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\units.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\units.obj"	"$(INTDIR)\units.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\util.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\wave.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\wave.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\wave.obj"	"$(INTDIR)\wave.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\libaudiofile\wavewrite.c

!IF  "$(CFG)" == "staticlib - Win32 Release"


"$(INTDIR)\wavewrite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"


"$(INTDIR)\wavewrite.obj"	"$(INTDIR)\wavewrite.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "staticlib - Win32 Release"

"modules - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\modules.mak" CFG="modules - Win32 Release" 
   cd "."

"modules - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\modules.mak" CFG="modules - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "staticlib - Win32 Debug"

"modules - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\modules.mak" CFG="modules - Win32 Debug" 
   cd "."

"modules - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\modules.mak" CFG="modules - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 


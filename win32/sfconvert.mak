# Microsoft Developer Studio Generated NMAKE File, Based on sfconvert.dsp
!IF "$(CFG)" == ""
CFG=sfconvert - Win32 Debug using DLL
!MESSAGE No configuration specified. Defaulting to sfconvert - Win32 Debug using DLL.
!ENDIF 

!IF "$(CFG)" != "sfconvert - Win32 Release" && "$(CFG)" != "sfconvert - Win32 Debug" && "$(CFG)" != "sfconvert - Win32 Debug using DLL" && "$(CFG)" != "sfconvert - Win32 Release using DLL"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sfconvert.mak" CFG="sfconvert - Win32 Debug using DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sfconvert - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sfconvert - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "sfconvert - Win32 Debug using DLL" (based on "Win32 (x86) Console Application")
!MESSAGE "sfconvert - Win32 Release using DLL" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "sfconvert - Win32 Release"

OUTDIR=.\Release\sfconvert
INTDIR=.\Release\sfconvert

!IF "$(RECURSE)" == "0" 

ALL : ".\Release\bin\static\sfconvert.exe"

!ELSE 

ALL : "staticlib - Win32 Release" "dynamiclib - Win32 Release" ".\Release\bin\static\sfconvert.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dynamiclib - Win32 ReleaseCLEAN" "staticlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\sfconvert.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Release\bin\static\sfconvert.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\sfconvert.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfconvert.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofile.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\sfconvert.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /out:"Release\bin\static\sfconvert.exe" /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfconvert.obj" \
	".\Release\dynamiclib\audiofile.lib" \
	".\lib\libaudiofile.lib"

".\Release\bin\static\sfconvert.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug"

OUTDIR=.\Debug\sfconvert
INTDIR=.\Debug\sfconvert
# Begin Custom Macros
OutDir=.\Debug\sfconvert
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\Debug\bin\static\sfconvert.exe" "$(OUTDIR)\sfconvert.bsc"

!ELSE 

ALL : "staticlib - Win32 Debug" "dynamiclib - Win32 Debug" ".\Debug\bin\static\sfconvert.exe" "$(OUTDIR)\sfconvert.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dynamiclib - Win32 DebugCLEAN" "staticlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\printinfo.sbr"
	-@erase "$(INTDIR)\sfconvert.obj"
	-@erase "$(INTDIR)\sfconvert.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sfconvert.bsc"
	-@erase "$(OUTDIR)\sfconvert.pdb"
	-@erase ".\Debug\bin\static\sfconvert.exe"
	-@erase ".\Debug\bin\static\sfconvert.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfconvert.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\printinfo.sbr" \
	"$(INTDIR)\sfconvert.sbr"

"$(OUTDIR)\sfconvert.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofileD.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\sfconvert.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"Debug\bin\static\sfconvert.exe" /pdbtype:sept /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfconvert.obj" \
	".\Debug\dynamiclib\audiofileD.lib" \
	".\lib\libaudiofileD.lib"

".\Debug\bin\static\sfconvert.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug using DLL"

OUTDIR=.\Debug\sfconvert_using_DLL
INTDIR=.\Debug\sfconvert_using_DLL
# Begin Custom Macros
OutDir=.\Debug\sfconvert_using_DLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\sfconvert.exe"

!ELSE 

ALL : "$(OUTDIR)\sfconvert.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\sfconvert.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sfconvert.exe"
	-@erase "$(OUTDIR)\sfconvert.ilk"
	-@erase "$(OUTDIR)\sfconvert.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfconvert.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofileD.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\sfconvert.pdb" /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"$(OUTDIR)\sfconvert.exe" /pdbtype:sept /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfconvert.obj"

"$(OUTDIR)\sfconvert.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Release using DLL"

OUTDIR=.\Release\sfconvert_using_DLL
INTDIR=.\Release\sfconvert_using_DLL

!IF "$(RECURSE)" == "0" 

ALL : ".\Release\bin\dynamic\sfconvert.exe"

!ELSE 

ALL : ".\Release\bin\dynamic\sfconvert.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\sfconvert.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Release\bin\dynamic\sfconvert.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\sfconvert.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfconvert.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib audiofile.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\sfconvert.pdb" /machine:I386 /nodefaultlib:"libcd" /out:"Release\bin\dynamic\sfconvert.exe" /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfconvert.obj"

".\Release\bin\dynamic\sfconvert.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("sfconvert.dep")
!INCLUDE "sfconvert.dep"
!ELSE 
!MESSAGE Warning: cannot find "sfconvert.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "sfconvert - Win32 Release" || "$(CFG)" == "sfconvert - Win32 Debug" || "$(CFG)" == "sfconvert - Win32 Debug using DLL" || "$(CFG)" == "sfconvert - Win32 Release using DLL"
SOURCE=..\sfcommands\printinfo.c

!IF  "$(CFG)" == "sfconvert - Win32 Release"


"$(INTDIR)\printinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug"


"$(INTDIR)\printinfo.obj"	"$(INTDIR)\printinfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug using DLL"


"$(INTDIR)\printinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfconvert - Win32 Release using DLL"


"$(INTDIR)\printinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\sfcommands\sfconvert.c

!IF  "$(CFG)" == "sfconvert - Win32 Release"


"$(INTDIR)\sfconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug"


"$(INTDIR)\sfconvert.obj"	"$(INTDIR)\sfconvert.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug using DLL"


"$(INTDIR)\sfconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfconvert - Win32 Release using DLL"


"$(INTDIR)\sfconvert.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "sfconvert - Win32 Release"

"dynamiclib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Release" 
   cd "."

"dynamiclib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug"

"dynamiclib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Debug" 
   cd "."

"dynamiclib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug using DLL"

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Release using DLL"

!ENDIF 

!IF  "$(CFG)" == "sfconvert - Win32 Release"

"staticlib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Release" 
   cd "."

"staticlib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug"

"staticlib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Debug" 
   cd "."

"staticlib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug using DLL"

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Release using DLL"

!ENDIF 


!ENDIF 


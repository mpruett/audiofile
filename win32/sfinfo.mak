# Microsoft Developer Studio Generated NMAKE File, Based on sfinfo.dsp
!IF "$(CFG)" == ""
CFG=sfinfo - Win32 Release using DLL
!MESSAGE No configuration specified. Defaulting to sfinfo - Win32 Release using DLL.
!ENDIF 

!IF "$(CFG)" != "sfinfo - Win32 Release" && "$(CFG)" != "sfinfo - Win32 Debug" && "$(CFG)" != "sfinfo - Win32 Debug using DLL" && "$(CFG)" != "sfinfo - Win32 Release using DLL"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sfinfo.mak" CFG="sfinfo - Win32 Release using DLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sfinfo - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "sfinfo - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "sfinfo - Win32 Debug using DLL" (based on "Win32 (x86) Console Application")
!MESSAGE "sfinfo - Win32 Release using DLL" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "sfinfo - Win32 Release"

OUTDIR=.\Release\sfinfo\static
INTDIR=.\Release\sfinfo\static

!IF "$(RECURSE)" == "0" 

ALL : ".\Release\bin\static\sfinfo.exe"

!ELSE 

ALL : "staticlib - Win32 Release" "dynamiclib - Win32 Release" ".\Release\bin\static\sfinfo.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dynamiclib - Win32 ReleaseCLEAN" "staticlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\sfinfo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\Release\bin\static\sfinfo.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fp"$(INTDIR)\sfinfo.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfinfo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofile.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\sfinfo.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /out:"Release\bin\static\sfinfo.exe" /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfinfo.obj" \
	".\Release\dynamiclib\audiofile.lib" \
	".\lib\libaudiofile.lib"

".\Release\bin\static\sfinfo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug"

OUTDIR=.\Debug\sfinfo
INTDIR=.\Debug\sfinfo

!IF "$(RECURSE)" == "0" 

ALL : ".\Debug\bin\static\sfinfo.exe"

!ELSE 

ALL : "staticlib - Win32 Debug" "dynamiclib - Win32 Debug" ".\Debug\bin\static\sfinfo.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dynamiclib - Win32 DebugCLEAN" "staticlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\sfinfo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sfinfo.pdb"
	-@erase ".\Debug\bin\static\sfinfo.exe"
	-@erase ".\Debug\bin\static\sfinfo.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfinfo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofileD.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\sfinfo.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"Debug\bin\static\sfinfo.exe" /pdbtype:sept /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfinfo.obj" \
	".\Debug\dynamiclib\audiofileD.lib" \
	".\lib\libaudiofileD.lib"

".\Debug\bin\static\sfinfo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug using DLL"

OUTDIR=.\Debug\sfinfo_using_DLL
INTDIR=.\Debug\sfinfo_using_DLL
# Begin Custom Macros
OutDir=.\Debug\sfinfo_using_DLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\Debug\bin\dynamic\sfinfo.exe" "$(OUTDIR)\sfinfo.bsc"

!ELSE 

ALL : ".\Debug\bin\dynamic\sfinfo.exe" "$(OUTDIR)\sfinfo.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\printinfo.sbr"
	-@erase "$(INTDIR)\sfinfo.obj"
	-@erase "$(INTDIR)\sfinfo.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sfinfo.bsc"
	-@erase "$(OUTDIR)\sfinfo.pdb"
	-@erase ".\Debug\bin\dynamic\sfinfo.exe"
	-@erase ".\Debug\bin\dynamic\sfinfo.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfinfo.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\printinfo.sbr" \
	"$(INTDIR)\sfinfo.sbr"

"$(OUTDIR)\sfinfo.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib audiofileD.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\sfinfo.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"Debug\bin\dynamic\sfinfo.exe" /pdbtype:sept /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfinfo.obj"

".\Debug\bin\dynamic\sfinfo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Release using DLL"

OUTDIR=.\Release\sfinfo_using_DLL
INTDIR=.\Release\sfinfo_using_DLL

!IF "$(RECURSE)" == "0" 

ALL : ".\Release\bin\dynamic\sfinfo.exe"

!ELSE 

ALL : ".\Release\bin\dynamic\sfinfo.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\printinfo.obj"
	-@erase "$(INTDIR)\sfinfo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sfinfo.pdb"
	-@erase ".\Release\bin\dynamic\sfinfo.exe"
	-@erase ".\Release\bin\dynamic\sfinfo.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /D "USE_DLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfinfo.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib audiofile.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\sfinfo.pdb" /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"Release\bin\dynamic\sfinfo.exe" /pdbtype:sept /libpath:"lib" 
LINK32_OBJS= \
	"$(INTDIR)\printinfo.obj" \
	"$(INTDIR)\sfinfo.obj"

".\Release\bin\dynamic\sfinfo.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("sfinfo.dep")
!INCLUDE "sfinfo.dep"
!ELSE 
!MESSAGE Warning: cannot find "sfinfo.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "sfinfo - Win32 Release" || "$(CFG)" == "sfinfo - Win32 Debug" || "$(CFG)" == "sfinfo - Win32 Debug using DLL" || "$(CFG)" == "sfinfo - Win32 Release using DLL"
SOURCE=..\sfcommands\printinfo.c

!IF  "$(CFG)" == "sfinfo - Win32 Release"


"$(INTDIR)\printinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug"


"$(INTDIR)\printinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug using DLL"


"$(INTDIR)\printinfo.obj"	"$(INTDIR)\printinfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfinfo - Win32 Release using DLL"


"$(INTDIR)\printinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\sfcommands\sfinfo.c

!IF  "$(CFG)" == "sfinfo - Win32 Release"


"$(INTDIR)\sfinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug"


"$(INTDIR)\sfinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug using DLL"


"$(INTDIR)\sfinfo.obj"	"$(INTDIR)\sfinfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "sfinfo - Win32 Release using DLL"


"$(INTDIR)\sfinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "sfinfo - Win32 Release"

"dynamiclib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Release" 
   cd "."

"dynamiclib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug"

"dynamiclib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Debug" 
   cd "."

"dynamiclib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\dynamiclib.mak" CFG="dynamiclib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug using DLL"

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Release using DLL"

!ENDIF 

!IF  "$(CFG)" == "sfinfo - Win32 Release"

"staticlib - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Release" 
   cd "."

"staticlib - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug"

"staticlib - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Debug" 
   cd "."

"staticlib - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\staticlib.mak" CFG="staticlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Debug using DLL"

!ELSEIF  "$(CFG)" == "sfinfo - Win32 Release using DLL"

!ENDIF 


!ENDIF 


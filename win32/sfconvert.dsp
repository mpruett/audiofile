# Microsoft Developer Studio Project File - Name="sfconvert" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sfconvert - Win32 Debug using DLL
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "sfconvert.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sfconvert - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "sfconvert___Win32_Release"
# PROP BASE Intermediate_Dir "sfconvert___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\sfconvert"
# PROP Intermediate_Dir "Release\sfconvert"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofile.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcmt.lib" /out:"Release\bin\static\sfconvert.exe" /libpath:"lib"

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sfconvert___Win32_Debug"
# PROP BASE Intermediate_Dir "sfconvert___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\sfconvert"
# PROP Intermediate_Dir "Debug\sfconvert"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofileD.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmtd" /out:"Debug\bin\static\sfconvert.exe" /pdbtype:sept /libpath:"lib"

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Debug using DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "sfconvert___Win32_Debug_using_DLL"
# PROP BASE Intermediate_Dir "sfconvert___Win32_Debug_using_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\sfconvert_using_DLL"
# PROP Intermediate_Dir "Debug\sfconvert_using_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofileD.lib modulesD.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmtd" /pdbtype:sept /libpath:"lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libaudiofileD.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmtd" /pdbtype:sept /libpath:"lib"

!ELSEIF  "$(CFG)" == "sfconvert - Win32 Release using DLL"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "sfconvert___Win32_Release_using_DLL"
# PROP BASE Intermediate_Dir "sfconvert___Win32_Release_using_DLL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\sfconvert_using_DLL"
# PROP Intermediate_Dir "Release\sfconvert_using_DLL"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "HAVE_CONFIG_H" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib audiofile.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcd" /out:"Release\bin\dynamic\sfconvert.exe" /libpath:"lib"

!ENDIF 

# Begin Target

# Name "sfconvert - Win32 Release"
# Name "sfconvert - Win32 Debug"
# Name "sfconvert - Win32 Debug using DLL"
# Name "sfconvert - Win32 Release using DLL"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\sfcommands\printinfo.c
# End Source File
# Begin Source File

SOURCE=..\sfcommands\sfconvert.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

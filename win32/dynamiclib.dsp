# Microsoft Developer Studio Project File - Name="dynamiclib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dynamiclib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dynamiclib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dynamiclib.mak" CFG="dynamiclib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dynamiclib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dynamiclib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dynamiclib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dynamiclib___Win32_Release"
# PROP BASE Intermediate_Dir "dynamiclib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\dynamiclib"
# PROP Intermediate_Dir "Release\dynamiclib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /D "HAVE_CONFIG_H" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dynmodules.lib /nologo /dll /machine:I386 /nodefaultlib:"msvcrt" /out:"lib\audiofile.dll" /libpath:"Release\dynmodules"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=get export library
PostBuild_Cmds=echo on	copy Release\dynamiclib\audiofile.lib .\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "dynamiclib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dynamiclib___Win32_Debug"
# PROP BASE Intermediate_Dir "dynamiclib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug/dynamiclib"
# PROP Intermediate_Dir "Debug/dynamiclib"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\libaudiofile" /I "..\..\vorbis\vorbis\include" /I "..\..\vorbis\ogg\include" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /D "HAVE_CONFIG_H" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dynmodules.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"lib/audiofileD.dll" /pdbtype:sept /libpath:"Debug\dynmodules"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=get export library
PostBuild_Cmds=echo off	copy Debug\dynamiclib\audiofileD.lib .\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "dynamiclib - Win32 Release"
# Name "dynamiclib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\libaudiofile\aes.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\af_vfs.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\aiff.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\aiffwrite.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\aupv.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\avr.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\avrwrite.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\byteorder.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\compression.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\data.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\error.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\extended.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\format.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\g711.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\iff.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\iffwrite.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\instrument.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\ircam.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\ircamwrite.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\loop.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\marker.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\misc.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\next.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\nextwrite.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\nist.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\nistwrite.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\openclose.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\pcm.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\query.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\raw.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\setup.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\track.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\units.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\util.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\wave.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\wavewrite.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\libaudiofile\af_vfs.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\afinternal.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\aiff.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\audiofile.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\aupvinternal.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\aupvlist.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\avr.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\byteorder.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\compression.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\error.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\extended.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\g711.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\iff.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\instrument.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\ircam.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\marker.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\next.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\nist.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\pcm.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\print.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\raw.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\setup.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\track.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\units.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\util.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\wave.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "module definition"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audiofile.def
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="dynmodules" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dynmodules - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dynmodules.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dynmodules - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dynmodules___Win32_Release"
# PROP BASE Intermediate_Dir "dynmodules___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release\dynmodules"
# PROP Intermediate_Dir "Release\dynmodules"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\libaudiofile" /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /D "HAVE_CONFIG_H" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dynmodules - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dynmodules___Win32_Debug"
# PROP BASE Intermediate_Dir "dynmodules___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug\dynmodules"
# PROP Intermediate_Dir "Debug\dynmodules"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\libaudiofile" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_USRDLL" /D "DYNAMICLIB_EXPORTS" /D "HAVE_CONFIG_H" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dynmodules - Win32 Release"
# Name "dynmodules - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\libaudiofile\modules\adpcm.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\g711.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\ima.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\msadpcm.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\pcm.c
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\rebuffer.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\libaudiofile\modules\adpcm.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\g711.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\ima.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\msadpcm.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\pcm.h
# End Source File
# Begin Source File

SOURCE=..\libaudiofile\modules\rebuffer.h
# End Source File
# End Group
# End Target
# End Project

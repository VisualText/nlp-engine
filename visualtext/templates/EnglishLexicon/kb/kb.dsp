# Microsoft Developer Studio Project File - Name="kb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=kb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "kb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kb.mak" CFG="kb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "kb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "kb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "$(TAI)\include\Api" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KB_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libkbm.lib /nologo /dll /debug /machine:I386 /def:"kb.def" /libpath:"$(TAI)\lib" /fixed:no
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "kb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(TAI)\include\Api" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "KB_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libkbmd.lib /nologo /dll /incremental:no /debug /machine:I386 /def:"kb.def" /libpath:"$(TAI)\lib" /fixed:no
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "kb - Win32 Release"
# Name "kb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Cc_code.cpp
# End Source File
# Begin Source File

SOURCE=.\Con0.cpp
# End Source File
# Begin Source File

SOURCE=.\Con_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\kb.cpp
# End Source File
# Begin Source File

SOURCE=.\kb_setup.cpp
# End Source File
# Begin Source File

SOURCE=.\Ptr0.cpp
# End Source File
# Begin Source File

SOURCE=.\Ptr_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\St0.cpp
# End Source File
# Begin Source File

SOURCE=.\St_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Sym0.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym1.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym10.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym11.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym12.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym13.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym14.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym15.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym16.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym17.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym18.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym19.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym2.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym20.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym21.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym3.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym4.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym5.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym6.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym7.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym8.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym9.cpp
# End Source File
# Begin Source File

SOURCE=.\Sym_ini.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Cc_code.h
# End Source File
# Begin Source File

SOURCE=.\Con_ini.h
# End Source File
# Begin Source File

SOURCE=.\kb.h
# End Source File
# Begin Source File

SOURCE=.\kb_setup.h
# End Source File
# Begin Source File

SOURCE=.\Ptr_ini.h
# End Source File
# Begin Source File

SOURCE=.\St_ini.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Sym_ini.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project

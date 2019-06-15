# Microsoft Developer Studio Project File - Name="libconsh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libconsh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libconsh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libconsh.mak" CFG="libconsh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libconsh - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libconsh - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libconsh - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBCONSH_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "$(TAI)\cs\include" /I "$(TAI)\include\Api" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBCONSH_EXPORTS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libprim.lib libkbm.lib /nologo /dll /debug /machine:I386 /libpath:"$(TAI)\lib" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del /q $(TAI)\bin\libconsh.dll	del /q $(TAI)\lib\libconsh.lib	del /q $(TAI)\lib\libconsh.exp	copy release\libconsh.dll $(TAI)\bin\libconsh.dll	copy release\libconsh.lib $(TAI)\lib\libconsh.lib	copy release\libconsh.exp $(TAI)\lib\libconsh.exp
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libconsh - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBCONSH_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(TAI)\cs\include" /I "$(TAI)\include\Api" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBCONSH_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libprimd.lib libkbmd.lib /nologo /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Debug/libconshd.dll" /libpath:"$(TAI)\lib" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del $(TAI)\bin\libconshd.dll	del $(TAI)\lib\libconshd.lib	del $(TAI)\lib\libconshd.exp	copy debug\libconshd.dll $(TAI)\bin\libconshd.dll	copy debug\libconshd.lib $(TAI)\lib\libconshd.lib	copy debug\libconshd.exp $(TAI)\lib\libconshd.exp
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libconsh - Win32 Release"
# Name "libconsh - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Aconsh.cpp
# End Source File
# Begin Source File

SOURCE=.\arg.cpp
# End Source File
# Begin Source File

SOURCE=.\bind.cpp
# End Source File
# Begin Source File

SOURCE=.\cc_code.cpp
# End Source File
# Begin Source File

SOURCE=.\cc_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\cg.cpp
# End Source File
# Begin Source File

SOURCE=.\cg_global.cpp
# End Source File
# Begin Source File

SOURCE=.\cmd.cpp
# End Source File
# Begin Source File

SOURCE=.\con_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\con_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\consh_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\consh_kb.cpp
# End Source File
# Begin Source File

SOURCE=.\dyn.cpp
# End Source File
# Begin Source File

SOURCE=.\gen.cpp
# End Source File
# Begin Source File

SOURCE=.\help.cpp
# End Source File
# Begin Source File

SOURCE=.\ind.cpp
# End Source File
# Begin Source File

SOURCE=.\io.cpp
# End Source File
# Begin Source File

SOURCE=.\libconsh.cpp
# End Source File
# Begin Source File

SOURCE=.\ptr_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\ptr_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\st_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\st_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\sym_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\sym_ini.cpp
# End Source File
# Begin Source File

SOURCE=.\ui.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cg_global.h
# End Source File
# Begin Source File

SOURCE=.\dyn.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
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

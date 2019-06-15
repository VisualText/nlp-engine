# Microsoft Developer Studio Project File - Name="words" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=words - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "words.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "words.mak" CFG="words - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "words - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "words - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "words - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WORDS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "$(TAI)\include\Api" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WORDS_EXPORTS" /Yu"stdafx.h" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del /q $(TAI)\bin\words.dll	del /q $(TAI)\lib\words.lib	del /q $(TAI)\lib\words.exp	copy release\words.dll $(TAI)\bin\words.dll	copy release\words.lib $(TAI)\lib\words.lib	copy release\words.exp $(TAI)\lib\words.exp
# End Special Build Tool

!ELSEIF  "$(CFG)" == "words - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WORDS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(TAI)\include\Api" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WORDS_EXPORTS" /Yu"stdafx.h" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"Debug/wordsd.dll" /pdbtype:sept /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del $(TAI)\bin\wordsd.dll	del $(TAI)\lib\wordsd.lib	del $(TAI)\lib\wordsd.exp	copy debug\wordsd.dll $(TAI)\bin\wordsd.dll	copy debug\wordsd.lib $(TAI)\lib\wordsd.lib	copy debug\wordsd.exp $(TAI)\lib\wordsd.exp
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "words - Win32 Release"
# Name "words - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\word1.cpp
# End Source File
# Begin Source File

SOURCE=.\word10.cpp
# End Source File
# Begin Source File

SOURCE=.\word11.cpp
# End Source File
# Begin Source File

SOURCE=.\word12.cpp
# End Source File
# Begin Source File

SOURCE=.\word13.cpp
# End Source File
# Begin Source File

SOURCE=.\word14.cpp
# End Source File
# Begin Source File

SOURCE=.\word15.cpp
# End Source File
# Begin Source File

SOURCE=.\word16.cpp
# End Source File
# Begin Source File

SOURCE=.\word17.cpp
# End Source File
# Begin Source File

SOURCE=.\word18.cpp
# End Source File
# Begin Source File

SOURCE=.\word19.cpp
# End Source File
# Begin Source File

SOURCE=.\word2.cpp
# End Source File
# Begin Source File

SOURCE=.\word20.cpp
# End Source File
# Begin Source File

SOURCE=.\word21.cpp
# End Source File
# Begin Source File

SOURCE=.\word22.cpp
# End Source File
# Begin Source File

SOURCE=.\word23.cpp
# End Source File
# Begin Source File

SOURCE=.\word24.cpp
# End Source File
# Begin Source File

SOURCE=.\word25.cpp
# End Source File
# Begin Source File

SOURCE=.\word26.cpp
# End Source File
# Begin Source File

SOURCE=.\word27.cpp
# End Source File
# Begin Source File

SOURCE=.\word28.cpp
# End Source File
# Begin Source File

SOURCE=.\word29.cpp
# End Source File
# Begin Source File

SOURCE=.\word3.cpp
# End Source File
# Begin Source File

SOURCE=.\word30.cpp
# End Source File
# Begin Source File

SOURCE=.\word4.cpp
# End Source File
# Begin Source File

SOURCE=.\word5.cpp
# End Source File
# Begin Source File

SOURCE=.\word6.cpp
# End Source File
# Begin Source File

SOURCE=.\word7.cpp
# End Source File
# Begin Source File

SOURCE=.\word8.cpp
# End Source File
# Begin Source File

SOURCE=.\word9.cpp
# End Source File
# Begin Source File

SOURCE=.\wordarrs.cpp
# End Source File
# Begin Source File

SOURCE=.\words.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
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

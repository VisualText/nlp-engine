# Microsoft Developer Studio Project File - Name="lite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=lite - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lite.mak" CFG="lite - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lite - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "lite - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lite - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LITE_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "$(TAI)\include\Api\lite" /I "$(TAI)\include\Api\web" /I "$(TAI)\include\Api" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LITE_EXPORTS" /Yu"stdafx.h" /FD /Zm250 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib web.lib libconsh.lib words.lib libprim.lib /nologo /dll /debug /machine:I386 /libpath:"$(TAI)\lib" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del /q $(TAI)\bin\lite.dll	del /q $(TAI)\lib\lite.lib	del /q $(TAI)\lib\lite.exp	copy release\lite.dll $(TAI)\bin\lite.dll	copy release\lite.lib $(TAI)\lib\lite.lib	copy release\lite.exp $(TAI)\lib\lite.exp
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lite - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LITE_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(TAI)\include\Api\lite" /I "$(TAI)\include\Api\web" /I "$(TAI)\include\Api" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LITE_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /Zm200 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib webd.lib libconshd.lib wordsd.lib libprimd.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"Debug/lited.dll" /libpath:"$(TAI)\lib" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del $(TAI)\bin\lited.dll	del $(TAI)\lib\lited.lib	del $(TAI)\lib\lited.exp	copy debug\lited.dll $(TAI)\bin\lited.dll	copy debug\lited.lib $(TAI)\lib\lited.lib	copy debug\lited.exp $(TAI)\lib\lited.exp
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lite - Win32 Release"
# Name "lite - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Agram.cpp
# End Source File
# Begin Source File

SOURCE=.\algo.cpp
# End Source File
# Begin Source File

SOURCE=.\ana.cpp
# End Source File
# Begin Source File

SOURCE=.\angen.cpp
# End Source File
# Begin Source File

SOURCE=.\angen.h
# End Source File
# Begin Source File

SOURCE=.\arg.cpp
# End Source File
# Begin Source File

SOURCE=.\Arug.cpp
# End Source File
# Begin Source File

SOURCE=.\Arug.h
# End Source File
# Begin Source File

SOURCE=.\Arun.cpp
# End Source File
# Begin Source File

SOURCE=.\Aseq.cpp
# End Source File
# Begin Source File

SOURCE=.\Auser.cpp
# End Source File
# Begin Source File

SOURCE=.\Avar.cpp
# End Source File
# Begin Source File

SOURCE=.\chars.cpp
# End Source File
# Begin Source File

SOURCE=.\chars.h
# End Source File
# Begin Source File

SOURCE=.\check.cpp
# End Source File
# Begin Source File

SOURCE=.\check.h
# End Source File
# Begin Source File

SOURCE=.\code.cpp
# End Source File
# Begin Source File

SOURCE=.\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\dsem.cpp
# End Source File
# Begin Source File

SOURCE=.\dsem.h
# End Source File
# Begin Source File

SOURCE=.\dyn.cpp
# End Source File
# Begin Source File

SOURCE=.\dyn.h
# End Source File
# Begin Source File

SOURCE=.\eana.cpp
# End Source File
# Begin Source File

SOURCE=.\eana.h
# End Source File
# Begin Source File

SOURCE=.\fn.cpp
# End Source File
# Begin Source File

SOURCE=.\fn.h
# End Source File
# Begin Source File

SOURCE=.\fnrun.cpp
# End Source File
# Begin Source File

SOURCE=.\folder.cpp
# End Source File
# Begin Source File

SOURCE=.\folder.h
# End Source File
# Begin Source File

SOURCE=.\func_defs.h
# End Source File
# Begin Source File

SOURCE=.\funcs.h
# End Source File
# Begin Source File

SOURCE=.\gen.cpp
# End Source File
# Begin Source File

SOURCE=.\gen.h
# End Source File
# Begin Source File

SOURCE=.\genhash.cpp
# End Source File
# Begin Source File

SOURCE=.\genhash.h
# End Source File
# Begin Source File

SOURCE=.\global.cpp
# End Source File
# Begin Source File

SOURCE=.\Gui.cpp
# End Source File
# Begin Source File

SOURCE=.\htab.cpp
# End Source File
# Begin Source File

SOURCE=.\htab.h
# End Source File
# Begin Source File

SOURCE=.\iaction.cpp
# End Source File
# Begin Source File

SOURCE=.\iaction.h
# End Source File
# Begin Source File

SOURCE=.\iarg.cpp
# End Source File
# Begin Source File

SOURCE=.\ielement.cpp
# End Source File
# Begin Source File

SOURCE=.\ielement.h
# End Source File
# Begin Source File

SOURCE=.\ielt.cpp
# End Source File
# Begin Source File

SOURCE=.\ielt.h
# End Source File
# Begin Source File

SOURCE=.\iexpr.cpp
# End Source File
# Begin Source File

SOURCE=.\iexpr.h
# End Source File
# Begin Source File

SOURCE=.\iexprstmt.cpp
# End Source File
# Begin Source File

SOURCE=.\iexprstmt.h
# End Source File
# Begin Source File

SOURCE=.\ifile.cpp
# End Source File
# Begin Source File

SOURCE=.\ifile.h
# End Source File
# Begin Source File

SOURCE=.\ifunc.cpp
# End Source File
# Begin Source File

SOURCE=.\ifunc.h
# End Source File
# Begin Source File

SOURCE=.\iifstmt.cpp
# End Source File
# Begin Source File

SOURCE=.\iifstmt.h
# End Source File
# Begin Source File

SOURCE=.\intern.cpp
# End Source File
# Begin Source File

SOURCE=.\intern.h
# End Source File
# Begin Source File

SOURCE=.\io.cpp
# End Source File
# Begin Source File

SOURCE=.\io.h
# End Source File
# Begin Source File

SOURCE=.\iop.cpp
# End Source File
# Begin Source File

SOURCE=.\iop.h
# End Source File
# Begin Source File

SOURCE=.\ipair.cpp
# End Source File
# Begin Source File

SOURCE=.\ipair.h
# End Source File
# Begin Source File

SOURCE=.\ipnode.cpp
# End Source File
# Begin Source File

SOURCE=.\ipnode.h
# End Source File
# Begin Source File

SOURCE=.\ipost.cpp
# End Source File
# Begin Source File

SOURCE=.\ipost.h
# End Source File
# Begin Source File

SOURCE=.\ipre.cpp
# End Source File
# Begin Source File

SOURCE=.\ipre.h
# End Source File
# Begin Source File

SOURCE=.\irecurse.cpp
# End Source File
# Begin Source File

SOURCE=.\irecurse.h
# End Source File
# Begin Source File

SOURCE=.\iregion.cpp
# End Source File
# Begin Source File

SOURCE=.\iregion.h
# End Source File
# Begin Source File

SOURCE=.\ireturn.cpp
# End Source File
# Begin Source File

SOURCE=.\ireturn.h
# End Source File
# Begin Source File

SOURCE=.\irule.cpp
# End Source File
# Begin Source File

SOURCE=.\irule.h
# End Source File
# Begin Source File

SOURCE=.\istmt.cpp
# End Source File
# Begin Source File

SOURCE=.\istmt.h
# End Source File
# Begin Source File

SOURCE=.\istr.cpp
# End Source File
# Begin Source File

SOURCE=.\istr.h
# End Source File
# Begin Source File

SOURCE=.\isugg.cpp
# End Source File
# Begin Source File

SOURCE=.\isugg.h
# End Source File
# Begin Source File

SOURCE=.\ivar.cpp
# End Source File
# Begin Source File

SOURCE=.\ivar.h
# End Source File
# Begin Source File

SOURCE=.\iwhilestmt.cpp
# End Source File
# Begin Source File

SOURCE=.\iwhilestmt.h
# End Source File
# Begin Source File

SOURCE=.\line.cpp
# End Source File
# Begin Source File

SOURCE=.\line.h
# End Source File
# Begin Source File

SOURCE=.\lite.cpp
# End Source File
# Begin Source File

SOURCE=.\literug.cpp
# End Source File
# Begin Source File

SOURCE=.\ln.cpp
# End Source File
# Begin Source File

SOURCE=.\ln.h
# End Source File
# Begin Source File

SOURCE=.\mach.cpp
# End Source File
# Begin Source File

SOURCE=.\mode.cpp
# End Source File
# Begin Source File

SOURCE=.\mode.h
# End Source File
# Begin Source File

SOURCE=.\multi.cpp
# End Source File
# Begin Source File

SOURCE=.\multi.h
# End Source File
# Begin Source File

SOURCE=.\nintern.cpp
# End Source File
# Begin Source File

SOURCE=.\nintern.h
# End Source File
# Begin Source File

SOURCE=.\nlp.cpp
# End Source File
# Begin Source File

SOURCE=.\nlppp.cpp
# End Source File
# Begin Source File

SOURCE=.\nug.cpp
# End Source File
# Begin Source File

SOURCE=.\nug.h
# End Source File
# Begin Source File

SOURCE=.\parse.cpp
# End Source File
# Begin Source File

SOURCE=.\pat.cpp
# End Source File
# Begin Source File

SOURCE=.\pat.h
# End Source File
# Begin Source File

SOURCE=.\path.cpp
# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=.\phash.cpp
# End Source File
# Begin Source File

SOURCE=.\phash.h
# End Source File
# Begin Source File

SOURCE=.\pn.cpp
# End Source File
# Begin Source File

SOURCE=.\post.cpp
# End Source File
# Begin Source File

SOURCE=.\post.h
# End Source File
# Begin Source File

SOURCE=.\postrfa.cpp
# End Source File
# Begin Source File

SOURCE=.\postrfa.h
# End Source File
# Begin Source File

SOURCE=.\pre.cpp
# End Source File
# Begin Source File

SOURCE=.\rec.cpp
# End Source File
# Begin Source File

SOURCE=.\rec.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\rfa.cpp
# End Source File
# Begin Source File

SOURCE=.\rfa.h
# End Source File
# Begin Source File

SOURCE=.\rfasem.cpp
# End Source File
# Begin Source File

SOURCE=.\rug.cpp
# End Source File
# Begin Source File

SOURCE=.\sem.cpp
# End Source File
# Begin Source File

SOURCE=.\seqn.cpp
# End Source File
# Begin Source File

SOURCE=.\seqn.h
# End Source File
# Begin Source File

SOURCE=.\stab.cpp
# End Source File
# Begin Source File

SOURCE=.\stab.h
# End Source File
# Begin Source File

SOURCE=.\starr.cpp
# End Source File
# Begin Source File

SOURCE=.\starr.h
# End Source File
# Begin Source File

SOURCE=.\std.cpp
# End Source File
# Begin Source File

SOURCE=.\std.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\str.cpp
# End Source File
# Begin Source File

SOURCE=.\str.h
# End Source File
# Begin Source File

SOURCE=.\string.cpp
# End Source File
# Begin Source File

SOURCE=.\string.h
# End Source File
# Begin Source File

SOURCE=.\stub_e.cpp
# End Source File
# Begin Source File

SOURCE=.\stub_e.h
# End Source File
# Begin Source File

SOURCE=.\stub_s.cpp
# End Source File
# Begin Source File

SOURCE=.\stub_s.h
# End Source File
# Begin Source File

SOURCE=.\sym.cpp
# End Source File
# Begin Source File

SOURCE=.\sym.h
# End Source File
# Begin Source File

SOURCE=.\thtab.h
# End Source File
# Begin Source File

SOURCE=.\tok.cpp
# End Source File
# Begin Source File

SOURCE=.\tok.h
# End Source File
# Begin Source File

SOURCE=.\tsym.cpp
# End Source File
# Begin Source File

SOURCE=.\tsym.h
# End Source File
# Begin Source File

SOURCE=.\uAlgo.cpp
# End Source File
# Begin Source File

SOURCE=.\uAlgo.h
# End Source File
# Begin Source File

SOURCE=.\var.cpp
# End Source File
# Begin Source File

SOURCE=.\var.h
# End Source File
# Begin Source File

SOURCE=.\vtrun.cpp
# End Source File
# Begin Source File

SOURCE=.\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\xml.h
# End Source File
# End Target
# End Project

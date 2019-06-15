@echo off
rem  SUBJ:  Batch file for compiling release versions of everything.
rem  CR:    06/22/00 AM.
rem  CALL:  compile-rel


if "%1" == "" goto ok

:bad
echo.
echo Usage: compile-rel
echo        Compiles release versions for VisualText and related projects.
goto exit

:ok

set fldr=d:\dev

msdev %fldr%\cs\libprim\libprim.dsp /MAKE "libprim - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\cs\libkbm\libkbm.dsp /MAKE "libkbm - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\cs\libconsh\libconsh.dsp /MAKE "libconsh - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\src\web\web.dsp /MAKE "web - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\src\words\words.dsp /MAKE "words - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\lite\lite.dsp /MAKE "lite - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\VisualText\VisualText.dsp /MAKE "VisualText - Win32 Release" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\VisualTextM\VisualText.dsp /MAKE "VisualText - Win32 Release_Mono" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\VisualTextM\VisualText.dsp /MAKE "VisualText - Win32 Release_Mono_Noweb" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem RESUME APP

rem msdev %fldr%\apps\resume\user\user.dsp /MAKE "user - Win32 Release" /REBUILD
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

rem msdev %fldr%\apps\resume\resume.dsp /MAKE "Resume - Win32 Release" /REBUILD
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

rem CHATTER APP

rem msdev %fldr%\apps\chatter\kb\kb.dsp /MAKE "kb - Win32 Release" /REBUILD
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

rem msdev %fldr%\apps\chatter\run\run.dsp /MAKE "run - Win32 Release" /REBUILD
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

rem msdev %fldr%\apps\chatter\user\user.dsp /MAKE "user - Win32 Release" /REBUILD
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

rem msdev %fldr%\apps\chatter\chatter.dsp /MAKE "chatter - Win32 Release" /REBUILD
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort


goto exit

:lowmemory
echo.
echo Error in compile-rel.
echo Some projects may not have compiled.
goto exit

:abort
echo.
echo Backup aborted.
goto exit

:exit

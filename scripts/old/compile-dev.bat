@echo off
rem  SUBJ:  Batch file for compiling debug versions of everything.
rem  CR:    11/10/00 AM.
rem  CALL:  compile-dev


if "%1" == "" goto ok

:bad
echo.
echo Usage: compile-dev
echo        Compiles debug versions for VisualText and related projects.
goto exit

:ok

set fldr=d:\dev

msdev %fldr%\cs\libprim\libprim.dsp /MAKE "libprim - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\cs\libkbm\libkbm.dsp /MAKE "libkbm - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\cs\libconsh\libconsh.dsp /MAKE "libconsh - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\src\web\web.dsp /MAKE "web - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\src\words\words.dsp /MAKE "words - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\lite\lite.dsp /MAKE "lite - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

msdev %fldr%\VisualText\VisualText.dsp /MAKE "VisualText - Win32 Debug" /REBUILD
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort



goto exit

:lowmemory
echo.
echo Error in compile-dev
echo Some projects may not have compiled.
goto exit

:abort
echo.
echo Backup aborted.
goto exit

:exit

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

devenv /rebuild release %fldr%\cs\libprim\libprim.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release %fldr%\cs\libkbm\libkbm.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release %fldr%\cs\libconsh\libconsh.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release %fldr%\src\web\web.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release %fldr%\src\words\words.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release %fldr%\lite\lite.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release %fldr%\VisualText\VisualText.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release_mono %fldr%\VisualTextM\VisualText.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild release_mono_noweb %fldr%\VisualTextM\VisualText.sln
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

@echo off
rem  SUBJ:  Batch file for compiling debug version of Lite. Workaround to vcspawn.exe error.
rem  CR:    10/13/03 AM.
rem  CALL:  compile-dev


if "%1" == "" goto ok

:bad
echo.
echo Usage: compile-dev
echo        Compiles debug versions for VisualText and related projects.
goto exit

:ok

set fldr=d:\dev

msdev %fldr%\cs\libconsh\libconsh.dsp /MAKE "libconsh - Win32 Debug"
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

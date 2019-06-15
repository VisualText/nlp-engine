@echo off
rem  SUBJ:  Batch file for compiling debug version of VisualText. Workaround to vcspawn.exe error.
rem  CR:    10/13/03 AM.
rem  CALL:  compile-dev


if "%1" == "" goto bad
goto ok

:bad
echo.
echo Usage: compile-prog analyzername
echo        Compiles debug versions analyzer projects
goto exit

:ok

set fldr=d:\apps

msdev %fldr%\%1\prog.dsp /MAKE "prog - Win32 Release"
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

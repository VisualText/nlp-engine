@echo off
rem  SUBJ:  Batch file for compiling User project of VisualText analyzer. Workaround to vcspawn.exe error.
rem  CR:    12/03/03 AM.
rem  CALL:  compile-user NAME


if "%1" == "" goto bad
goto ok

:bad
echo.
echo Usage: compile-user analyzername
echo        Compiles debug versions analyzer projects
goto exit

:ok

set fldr=d:\apps

msdev %fldr%\%1\User\user.dsp /MAKE "user - Win32 Debug"
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort



goto exit

:lowmemory
echo.
echo Error in compile-user
echo Some projects may not have compiled.
goto exit

:abort
echo.
echo Backup aborted.
goto exit

:exit

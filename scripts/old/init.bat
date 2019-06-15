@echo off
rem  FILE:  %nlplite%scripts\init.bat
rem  SUBJ:  Batch file for initializing a distributed NLPLite system.
rem  CR:    02/01/99 AM.
rem  CALL:  init

if "%apps%" == "" goto ok
goto ok

:bad
echo.
echo Error: NLPLite is already installed.
echo Please manually edit c:\autoexec.bat to remove the APPS variable
echo and to remove Lite, NLPLite or related directories from the PATH
echo variable.  Reboot your machine.  Then retry this init program.
goto exit

:ok

echo This program prepares the NLPLite distribution for use,
echo by modifying your c:\AUTOEXEC.BAT file.  We recommend
echo you first make a safe copy of that file, naming it
echo c:\autoexec.saf, for example.

echo.
echo To exit, type the Ctrl and C key together.
pause

rem INITIALIZE HERE.

copy c:\autoexec.bat /a + nlplite.txt /a tmp.txt /a

REM INCOMPLETE. Could copy dictionary files etc.

echo Done initializing the NLPLite environment.
goto exit

:lowmemory
echo.
echo Not enough memory for copying files.
echo Some may have been copied.
goto exit

:abort
echo.
echo Backup aborted.
goto exit

:exit

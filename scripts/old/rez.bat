@echo off
rem  SUBJ:  Batch file for running Resume analyzer on an input text file.
rem  CR:    04/10/99 AM.
rem  CALL:  rez dehilster.txt

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: test infile
goto exit

:ok

resume -dev c:\apps\resume\input\resume\%1

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

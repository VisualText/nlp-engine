@echo off
rem  SUBJ:  Batch file for running webco on input text files.
rem  CR:    02/10/99 AM.
rem  CALL:  test test.txt

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: test infile
goto exit

:ok

webco -dev c:\apps\webco\input\text\%1

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

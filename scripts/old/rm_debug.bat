@echo off
rem  SUBJ:  Batch file for removing debug folders.
rem  CR:    12/05/00 AM.
rem  CALL:  rm_debugs

if "%1" == "" goto ok

:bad
echo.
echo Usage: rm_debugs
echo        Removes selected Debug directories.
goto exit

:ok

echo.

set fldr=d:\dev

rmdir /s /q %fldr%\cs\libprim\Debug
rmdir /s /q %fldr%\cs\libkbm\Debug
rmdir /s /q %fldr%\cs\libconsh\Debug

rmdir /s /q %fldr%\lite\Debug


rmdir /s /q %fldr%\src\web\Debug
rmdir /s /q %fldr%\src\words\Debug

rmdir /s /q %fldr%\VisualText\Debug


echo Done backing up.
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

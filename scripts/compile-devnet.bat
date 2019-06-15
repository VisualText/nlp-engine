@echo off
rem  SUBJ:  Batch file for compiling debug versions of everything.
rem  CR:    03/19/03 AM.
rem  CALL:  compile-devnet


if "%1" == "" goto ok

:bad
echo.
echo Usage: compile-dev
echo        Compiles debug versions for VisualText and related projects.
goto exit

:ok

set fldr=d:\dev

devenv /rebuild debug %fldr%\cs\libprim\libprim.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild debug %fldr%\cs\libkbm\libkbm.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild debug %fldr%\cs\libconsh\libconsh.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild debug %fldr%\src\web\web.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild debug %fldr%\src\words\words.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild debug %fldr%\lite\lite.sln
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

devenv /rebuild debug %fldr%\VisualText\VisualText.sln
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

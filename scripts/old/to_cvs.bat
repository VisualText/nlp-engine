@echo off
rem  SUBJ:  Batch file for copying to the CVS area for administrator.
rem  CR:    04/20/01 AM.
rem  CALL:  to_cvs
rem  NOTE:  Particular to Amnon's development environment.


if "%1" == "" goto ok

:bad
echo.
echo Usage: to_cvs
echo        Copies to <drive>\CYGWIN directory.
goto exit

:ok

echo.
echo Copying from development directories to CVS directory.
echo (Control-C aborts)
echo.

set to_dir=e:\cygwin\home\administrator
set fm_dir=d:\dev


rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle first guy.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                INCLUDE
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\include\Api %to_dir%\include\Api /w /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                   CS
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\cs\include\conan\*.h %to_dir%\cs\include\conan /e /h /r /i
xcopy %fm_dir%\cs\include\consh\*.h %to_dir%\cs\include\consh /e /h /r /i
xcopy %fm_dir%\cs\include\ext\*.h %to_dir%\cs\include\ext /e /h /r /i
xcopy %fm_dir%\cs\include\kbm\*.h %to_dir%\cs\include\kbm /e /h /r /i
xcopy %fm_dir%\cs\include\prim\*.h %to_dir%\cs\include\prim /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\cs\libconsh\*.cpp %to_dir%\cs\libconsh /e /h /r /i
xcopy %fm_dir%\cs\libconsh\*.h %to_dir%\cs\libconsh /e /h /r /i
xcopy %fm_dir%\cs\libconsh\*.dsw %to_dir%\cs\libconsh /e /h /r /i
xcopy %fm_dir%\cs\libconsh\*.dsp %to_dir%\cs\libconsh /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\cs\libkbm\*.cpp %to_dir%\cs\libkbm /e /h /r /i
xcopy %fm_dir%\cs\libkbm\*.h %to_dir%\cs\libkbm /e /h /r /i
xcopy %fm_dir%\cs\libkbm\*.dsw %to_dir%\cs\libkbm /e /h /r /i
xcopy %fm_dir%\cs\libkbm\*.dsp %to_dir%\cs\libkbm /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\cs\libprim\*.cpp %to_dir%\cs\libprim /e /h /r /i
xcopy %fm_dir%\cs\libprim\*.h %to_dir%\cs\libprim /e /h /r /i
xcopy %fm_dir%\cs\libprim\*.dsw %to_dir%\cs\libprim /e /h /r /i
xcopy %fm_dir%\cs\libprim\*.dsp %to_dir%\cs\libprim /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                  DATA
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\data\spec %to_dir%\data\spec /e /h /r /i
xcopy %fm_dir%\data\Rfb\spec %to_dir%\data\Rfb\spec /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                  LITE
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\lite\*.cpp %to_dir%\lite /e /h /r /i
xcopy %fm_dir%\lite\*.h %to_dir%\lite /e /h /r /i
xcopy %fm_dir%\lite\*.dsw %to_dir%\lite /e /h /r /i
xcopy %fm_dir%\lite\*.dsp %to_dir%\lite /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                SRC
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\src\web\*.cpp %to_dir%\src\web /e /h /r /i
xcopy %fm_dir%\src\web\*.h %to_dir%\src\web /e /h /r /i
xcopy %fm_dir%\src\web\*.dsw %to_dir%\src\web /e /h /r /i
xcopy %fm_dir%\src\web\*.dsp %to_dir%\src\web /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\src\words\*.cpp %to_dir%\src\words /e /h /r /i
xcopy %fm_dir%\src\words\*.h %to_dir%\src\words /e /h /r /i
xcopy %fm_dir%\src\words\*.dsw %to_dir%\src\words /e /h /r /i
xcopy %fm_dir%\src\words\*.dsp %to_dir%\src\words /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort


rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                APP
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\VisualText\App %to_dir%\VisualText\App /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem                SCRIPTS
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\scripts\*.bat %to_dir%\scripts /e /h /r /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort


echo Done copying to CVS.
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

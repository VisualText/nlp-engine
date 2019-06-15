@echo off
rem  SUBJ:  Batch file for creating a developer distribution
rem         of the apps and lite development areas.
rem  CR:    05/04/99 AM.
rem  CALL:  distdev 990122
rem            will create the directory c:\distdev\990122 appropriately

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: distdev dir
echo Example: distdev 990504
echo      copies c:\apps, c:\cs c:\lite, c:\lib to c:\distdev\990504\
echo      NOTE - COPIES SELECTED STUFF.
goto exit

:ok

echo.
echo Copying c:\apps, c:\cs c:\lite, c:\lib to c:\distdev\%1
echo (Control-C aborts)
echo.

set to_dir=c:\distdev\%1\

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle WEB LIBRARY.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy c:\lib\web\include %to_dir%lib\web\include\ /w /e /h

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

mkdir %to_dir%lib\web\Debug
copy c:\lib\web\debug\web.dll  %to_dir%lib\web\debug\
copy c:\lib\web\debug\web.lib  %to_dir%lib\web\debug\
copy c:\lib\web\debug\web.exp  %to_dir%lib\web\debug\

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle LITE LIBRARY.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

mkdir %to_dir%lite
mkdir %to_dir%lite\Debug
copy c:\lite\debug\lite.dll  %to_dir%lite\debug\
copy c:\lite\debug\lite.lib  %to_dir%lite\debug\
copy c:\lite\debug\lite.exp  %to_dir%lite\debug\

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle the new CS area.  05/04/99 AM.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

mkdir %to_dir%cs
mkdir %to_dir%cs\lib
mkdir %to_dir%cs\include

xcopy c:\cs\include %to_dir%cs\include /e /h /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem Copy the sample file that uses the API.
mkdir %to_dir%cs\sample
copy c:\dev\VisualText\kb.cpp %to_dir%cs\sample\
copy c:\dev\VisualText\kb.h   %to_dir%cs\sample\

rem Copy CS library files
copy c:\cs\libprim\debug\libprim.dll  %to_dir%cs\lib\
copy c:\cs\libprim\debug\libprim.lib  %to_dir%cs\lib\
copy c:\cs\libprim\debug\libprim.exp  %to_dir%cs\lib\

copy c:\cs\libkbm\debug\libkbm.dll  %to_dir%cs\lib\
copy c:\cs\libkbm\debug\libkbm.lib  %to_dir%cs\lib\
copy c:\cs\libkbm\debug\libkbm.exp  %to_dir%cs\lib\

copy c:\cs\libconsh\debug\libconsh.dll  %to_dir%cs\lib\
copy c:\cs\libconsh\debug\libconsh.lib  %to_dir%cs\lib\
copy c:\cs\libconsh\debug\libconsh.exp  %to_dir%cs\lib\

copy c:\cs\librug\debug\librug.dll  %to_dir%cs\lib\
copy c:\cs\librug\debug\librug.lib  %to_dir%cs\lib\
copy c:\cs\librug\debug\librug.exp  %to_dir%cs\lib\

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle include area.  06/01/99 AM.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

mkdir %to_dir%dev
mkdir %to_dir%dev\include

rem NOTE. Copying from APPS to DEV.  Don't want Dave's dev stuff to go back to him.
xcopy c:\apps\include %to_dir%dev\include /e /h /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

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

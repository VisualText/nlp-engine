@echo off
rem  SUBJ:  Batch file for creating a developer distribution of VT GUI source code.
rem  CR:    01/30/03 AM.
rem  CALL:  distgui 990624
rem            will create the directory <drive>\distsrc\990124 appropriately

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: distgui dir
echo Example: distgui vt030130
echo      copies apps, cs lite, lib to distsrc\vt030130\
echo      NOTE - COPIES SELECTED STUFF, INCLUDING SOURCE CODE.
goto exit

:ok

echo.
echo Copying parts of DEV area to distsrc\%1
echo (Control-C aborts)
echo.

set to_dir=d:\distsrc\%1\dev
set fm_dir=d:\dev

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle VISUALTEXT DIR.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\VisualText\*.dsw  %to_dir%\VisualText\ /w /h /i
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\VisualText\*.sln  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.vcproj  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.dsp  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.cpp  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.h  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.rc  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.bmp  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.hm  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.ico  %to_dir%\VisualText\ /h /i
xcopy %fm_dir%\VisualText\*.wav  %to_dir%\VisualText\ /h /i

xcopy %fm_dir%\VisualText\res %to_dir%\VisualText\res /h /i

xcopy %fm_dir%\VisualTextM\*.lib  %to_dir%\VisualTextM\ /h /i
xcopy %fm_dir%\VisualTextM\*.dsp  %to_dir%\VisualTextM\ /h /i
xcopy %fm_dir%\VisualTextM\*.dsw  %to_dir%\VisualTextM\ /h /i
xcopy %fm_dir%\VisualTextM\*.sln  %to_dir%\VisualTextM\ /h /i
xcopy %fm_dir%\VisualTextM\*.vcproj  %to_dir%\VisualTextM\ /h /i


rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle include area.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\include\Vt %to_dir%\include\Vt /e /h /i

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

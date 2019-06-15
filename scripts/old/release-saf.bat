@echo off
rem  SUBJ:  Batch file for creating a release distribution.
rem  CR:    04/07/00 AM.
rem  CALL:  release
rem            will create the directory <drive>\textai.
rem		To save elsewhere, copy the TextAI directory manually.

if "%1" == "" goto ok

:bad
echo.
echo Usage: release
echo        Creates <drive>\TextAI directory.
goto exit

:ok

echo.
echo Copying to TextAI directory.
echo (Control-C aborts)
echo.

set to_dir=d:\TextAI\
set fm_dir=d:

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle first guy.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\lib\web\release\*.dll %to_dir%bin\ /w /e /h /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle libraries.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\lite\release\lite.dll  %to_dir%bin\ /e /h /i
xcopy %fm_dir%\cs\libprim\release\libprim.dll  %to_dir%bin\ /e /h /i
xcopy %fm_dir%\cs\libkbm\release\libkbm.dll  %to_dir%bin\ /e /h /i
xcopy %fm_dir%\cs\libconsh\release\libconsh.dll  %to_dir%bin\ /e /h /i

rem Dave's GUI libraries.
xcopy %fm_dir%\dev\dll\*.dll  %to_dir%bin\ /e /h /i

rem Need VC++ libraries....

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle compilable libs.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\lite\release\lite.exp  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\cs\libprim\release\libprim.exp  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\cs\libkbm\release\libkbm.exp  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\cs\libconsh\release\libconsh.exp  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\web\release\web.exp  %to_dir%lib\ /e /h /i

xcopy %fm_dir%\lite\release\lite.lib  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\cs\libprim\release\libprim.lib  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\cs\libkbm\release\libkbm.lib  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\cs\libconsh\release\libconsh.lib  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\web\release\web.lib  %to_dir%lib\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle TextAI data.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
xcopy %fm_dir%\apps\include %to_dir%include\ /e /h /i
xcopy %fm_dir%\apps\data %to_dir%data\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle VisualText
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
xcopy %fm_dir%\dev\VisualText\release\VisualText.exe  %to_dir%bin\ /e /h /i

xcopy %fm_dir%\dev\VisualText\*.ico  %to_dir%bin\ /h /i
xcopy %fm_dir%\dev\VisualText\*.bmp  %to_dir%bin\ /h /i

xcopy %fm_dir%\dev\VisualText\App  %to_dir%bin\App\ /e /h /i
xcopy %fm_dir%\dev\VisualText\res  %to_dir%bin\res\ /e /h /i

xcopy %fm_dir%\dev\VisualText\Help\Help.chm  %to_dir%bin\Help\ /h /i
xcopy %fm_dir%\dev\VisualText\Help\*.gif  %to_dir%bin\Help\ /h /i
xcopy %fm_dir%\dev\VisualText\Help\*.bmp  %to_dir%bin\Help\ /h /i
xcopy %fm_dir%\dev\VisualText\Help\"Audio Files"  %to_dir%bin\Help\"Audio Files" /e /h /i
xcopy %fm_dir%\dev\VisualText\Help\"Image Files"  %to_dir%bin\Help\"Image Files" /e /h /i
xcopy %fm_dir%\dev\VisualText\Help\Videos  %to_dir%bin\Help\Videos /e /h /i
rem xcopy %fm_dir%\dev\VisualText\Help\WebHelp  %to_dir%bin\Help\WebHelp /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle sample applications.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Method: Copy everything, then remove standard garbage.
xcopy %fm_dir%\apps\chatter  %to_dir%apps\chatter\ /e /h /i
rmdir /s /q %to_dir%apps\chatter\Debug\
rmdir /s /q %to_dir%apps\chatter\user\Debug\

xcopy %fm_dir%\apps\resume  %to_dir%apps\resume\ /e /h /i
rmdir /s /q %to_dir%apps\resume\Debug\
rmdir /s /q %to_dir%apps\resume\user\Debug\

del /f /s /q %to_dir%*.pch
del /f /s /q %to_dir%*.obj
del /f /s /q %to_dir%*.idb
del /f /s /q %to_dir%*.sbr
del /f /s /q %to_dir%*.res
del /f /s /q %to_dir%*.rc
del /f /s /q %to_dir%*.aps
del /f /s /q %to_dir%*.clw
del /f /s /q %to_dir%*.ilk
del /f /s /q %to_dir%*.ncb
del /f /s /q %to_dir%*.opt
del /f /s /q %to_dir%*.plg
del /f /s /q %to_dir%*.bsc
del /f /s /q %to_dir%*.pdb

del /f /s /q %to_dir%*.log
del /f /s /q %to_dir%output.txt
del /f /s /q %to_dir%*.txt_log

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

@echo off
rem  SUBJ:  Batch file for creating a developer distribution
rem         of the apps and lite development areas WITH SOURCE CODE.
rem  CR:    06/24/99 AM.
rem  CALL:  distsrc 990624
rem            will create the directory <drive>\distsrc\990124 appropriately
rem         09/12/99 AM. Parameterizing the FROM drive also.
rem	    12/04/00 AM. Update.

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: distsrc dir
echo Example: distsrc 990912
echo      copies apps, cs lite, lib to distsrc\990912\
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
rem Handle WEB LIBRARY.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\src\web\*.dsw  %to_dir%\src\web\ /w /e /h /i
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\src\web\*.sln  %to_dir%\src\web\ /e /h /i
xcopy %fm_dir%\src\web\*.vcproj  %to_dir%\src\web\ /e /h /i
xcopy %fm_dir%\src\web\*.dsp  %to_dir%\src\web\ /e /h /i
xcopy %fm_dir%\src\web\*.cpp  %to_dir%\src\web\ /e /h /i
xcopy %fm_dir%\src\web\*.h  %to_dir%\src\web\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle WORDS LIBRARY.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem 10/16/00 AM. Doing the WORDS.DLL library.

xcopy %fm_dir%\src\words\*.sln  %to_dir%\src\words\ /e /h /i
xcopy %fm_dir%\src\words\*.vcproj  %to_dir%\src\words\ /e /h /i
xcopy %fm_dir%\src\words\*.dsw  %to_dir%\src\words\ /e /h /i
xcopy %fm_dir%\src\words\*.dsp  %to_dir%\src\words\ /e /h /i
xcopy %fm_dir%\src\words\*.cpp  %to_dir%\src\words\ /e /h /i
xcopy %fm_dir%\src\words\*.h  %to_dir%\src\words\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle LITE LIBRARY.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\lite\*.sln  %to_dir%\lite\ /e /h /i
xcopy %fm_dir%\lite\*.vcproj  %to_dir%\lite\ /e /h /i
xcopy %fm_dir%\lite\*.dsw  %to_dir%\lite\ /e /h /i
xcopy %fm_dir%\lite\*.dsp  %to_dir%\lite\ /e /h /i
xcopy %fm_dir%\lite\*.cpp  %to_dir%\lite\ /e /h /i
xcopy %fm_dir%\lite\*.h  %to_dir%\lite\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle WN LIBRARY.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\wn\*.sln  %to_dir%\wn\ /e /h /i
xcopy %fm_dir%\wn\*.vcproj  %to_dir%\wn\ /e /h /i
xcopy %fm_dir%\wn\*.dsw  %to_dir%\wn\ /e /h /i
xcopy %fm_dir%\wn\*.dsp  %to_dir%\wn\ /e /h /i
xcopy %fm_dir%\wn\*.cpp  %to_dir%\wn\ /e /h /i
xcopy %fm_dir%\wn\*.c  %to_dir%\wn\ /e /h /i
xcopy %fm_dir%\wn\*.h  %to_dir%\wn\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle the new CS area.  05/04/99 AM.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\cs\include %to_dir%\cs\include /e /h /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem Copy CS library files
xcopy %fm_dir%\cs\libprim\*.sln  %to_dir%\cs\libprim\ /e /h /i
xcopy %fm_dir%\cs\libprim\*.vcproj  %to_dir%\cs\libprim\ /e /h /i
xcopy %fm_dir%\cs\libprim\*.dsw  %to_dir%\cs\libprim\ /e /h /i
xcopy %fm_dir%\cs\libprim\*.dsp  %to_dir%\cs\libprim\ /e /h /i
xcopy %fm_dir%\cs\libprim\*.cpp  %to_dir%\cs\libprim\ /e /h /i
xcopy %fm_dir%\cs\libprim\*.h  %to_dir%\cs\libprim\ /e /h /i

xcopy %fm_dir%\cs\libkbm\*.sln  %to_dir%\cs\libkbm\ /e /h /i
xcopy %fm_dir%\cs\libkbm\*.vcproj  %to_dir%\cs\libkbm\ /e /h /i
xcopy %fm_dir%\cs\libkbm\*.dsw  %to_dir%\cs\libkbm\ /e /h /i
xcopy %fm_dir%\cs\libkbm\*.dsp  %to_dir%\cs\libkbm\ /e /h /i
xcopy %fm_dir%\cs\libkbm\*.cpp  %to_dir%\cs\libkbm\ /e /h /i
xcopy %fm_dir%\cs\libkbm\*.h  %to_dir%\cs\libkbm\ /e /h /i

xcopy %fm_dir%\cs\libconsh\*.sln  %to_dir%\cs\libconsh\ /e /h /i
xcopy %fm_dir%\cs\libconsh\*.vcproj  %to_dir%\cs\libconsh\ /e /h /i
xcopy %fm_dir%\cs\libconsh\*.dsw  %to_dir%\cs\libconsh\ /e /h /i
xcopy %fm_dir%\cs\libconsh\*.dsp  %to_dir%\cs\libconsh\ /e /h /i
xcopy %fm_dir%\cs\libconsh\*.cpp  %to_dir%\cs\libconsh\ /e /h /i
xcopy %fm_dir%\cs\libconsh\*.h  %to_dir%\cs\libconsh\ /e /h /i

xcopy %fm_dir%\cs\libstream\*.sln  %to_dir%\cs\libstream\ /e /h /i
xcopy %fm_dir%\cs\libstream\*.vcproj  %to_dir%\cs\libstream\ /e /h /i
xcopy %fm_dir%\cs\libstream\*.dsw  %to_dir%\cs\libstream\ /e /h /i
xcopy %fm_dir%\cs\libstream\*.dsp  %to_dir%\cs\libstream\ /e /h /i
xcopy %fm_dir%\cs\libstream\*.cpp  %to_dir%\cs\libstream\ /e /h /i
xcopy %fm_dir%\cs\libstream\*.h  %to_dir%\cs\libstream\ /e /h /i

rem Zapped librug.  12/13/99 AM.

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle include area.  06/01/99 AM.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\data\Rfb %to_dir%\data\Rfb /e /h /i
xcopy %fm_dir%\data\spec %to_dir%\data\spec /e /h /i
rem skipping the doom.words file.

xcopy %fm_dir%\include\Api %to_dir%\include\Api /e /h /i
rem skipping dave's Vt includes.

xcopy %fm_dir%\scripts %to_dir%\scripts /e /h /i

rem 07/02/00 AM. Save my changes to the template analyzers.
rem xcopy %fm_dir%\Templates %to_dir%\Templates /e /h /i


rmdir /s /q %to_dir%\cs\libconsh\Debug
rmdir /s /q %to_dir%\cs\libconsh\Release
rmdir /s /q %to_dir%\cs\libconsh\Unicode_Debug
rmdir /s /q %to_dir%\cs\libconsh\Unicode_Release

rmdir /s /q %to_dir%\cs\libkbm\Debug
rmdir /s /q %to_dir%\cs\libkbm\Release
rmdir /s /q %to_dir%\cs\libkbm\Unicode_Debug
rmdir /s /q %to_dir%\cs\libkbm\Unicode_Release

rmdir /s /q %to_dir%\cs\libprim\Debug
rmdir /s /q %to_dir%\cs\libprim\Release
rmdir /s /q %to_dir%\cs\libprim\Unicode_Debug
rmdir /s /q %to_dir%\cs\libprim\Unicode_Release

rmdir /s /q %to_dir%\lite\Debug
rmdir /s /q %to_dir%\lite\Release
rmdir /s /q %to_dir%\lite\Unicode_Debug
rmdir /s /q %to_dir%\lite\Unicode_Release

rmdir /s /q %to_dir%\src\web\Debug
rmdir /s /q %to_dir%\src\web\Release
rmdir /s /q %to_dir%\src\web\Unicode_Debug
rmdir /s /q %to_dir%\src\web\Unicode_Release

rmdir /s /q %to_dir%\src\words\Debug
rmdir /s /q %to_dir%\src\words\Release
rmdir /s /q %to_dir%\src\words\Unicode_Debug
rmdir /s /q %to_dir%\src\words\Unicode_Release

rmdir /s /q %to_dir%\cs\libstream\unicode_debug
rmdir /s /q %to_dir%\cs\libstream\unicode_release

rmdir /s /q %to_dir%\wn\Debug
rmdir /s /q %to_dir%\wn\Release
rmdir /s /q %to_dir%\wn\Unicode_Debug
rmdir /s /q %to_dir%\wn\Unicode_Release

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

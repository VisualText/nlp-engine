@echo off
rem  SUBJ:  Batch file for creating a distribution of NLPLite, Lite, and Webco.
rem	    An interim solution till install shield can be set up for these.
rem		NO NLPLite SOURCE CODE IN THIS DISTRIBUTION.
rem  CR:    02/01/99 AM.
rem  CALL:  dist
rem      will create a c:\

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: dist dir
echo Example: dist 990201
echo   Creates a distribution copy of nlplite, lite, and webco in c:\dist\990201\
goto exit

:ok

echo.
echo Warning: Run cp_include script first.
echo Warning: Make sure Lite, NLPLite, Webco, Webco\User, apps\App\User
echo are all compiled and up to date.
echo Copying files from c:\apps, c:\lite and c:\dev\nlplite to c:\dist\%1
echo NOTE: Using NLPLITE Debug version, not Release!
echo Make sure to compile Release version before running dist.
echo (Control-C aborts)
echo.

set to_dir=c:\dist\%1\

xcopy c:\apps\App     %to_dir%App\     /w /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\apps\include %to_dir%include\ /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\apps\Webco   %to_dir%Webco\   /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\apps\Resume   %to_dir%Resume\   /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\apps\scripts     %to_dir%scripts\    /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem 2/18/99 AM.
mkdir %to_dir%doc
copy c:\apps\dev\doc\man.txt %to_dir%doc\
copy c:\apps\dev\doc\webco-user.txt %to_dir%doc\

mkdir %to_dir%bin
rem copy c:\dev\nlplite\Release\nlplite.exe  %to_dir%bin\
copy c:\dev\nlplite\Debug\nlplite.exe  %to_dir%bin\
rem copy c:\dev\nlplite\Release\nlplite.res  %to_dir%bin\
copy c:\dev\nlplite\Debug\nlplite.res  %to_dir%bin\
copy c:\dev\nlplite\Splsh16.bmp  %to_dir%bin\
copy c:\dev\nlplite\mdi.ico  %to_dir%bin\
copy c:\dev\nlplite\NLPLite.rc  %to_dir%bin\
copy c:\dev\nlplite\NLPLite.aps  %to_dir%bin\

xcopy c:\dev\nlplite\Help %to_dir%bin\Help\    /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\dev\nlplite\res %to_dir%bin\res\    /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\dev\nlplite\Graphics %to_dir%bin\Graphics\    /e /h
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

copy c:\lite\debug\lite.dll  %to_dir%bin\
copy c:\lite\debug\lite.lib  %to_dir%bin\
copy c:\lite\debug\lite.exp  %to_dir%bin\

copy c:\lib\web\debug\web.dll  %to_dir%bin\
copy c:\lib\web\debug\web.lib  %to_dir%bin\
copy c:\lib\web\debug\web.exp  %to_dir%bin\

copy c:\apps\spider\Debug\spider.exe %to_dir%bin\

rem Save some files.

move %to_dir%Webco\Debug\Webco.exe %to_dir%bin\Webco.exe

mkdir %to_dir%data
rem move %to_dir%Webco\data\doom.words %to_dir%data\doom.words
rem move %to_dir%Resume\data\doom.words %to_dir%data\doom.words

mkdir %to_dir%Webco\saf
move %to_dir%Webco\user\Debug\user.dll %to_dir%Webco\saf\user.dll
move %to_dir%Webco\user\Debug\user.lib %to_dir%Webco\saf\user.lib
move %to_dir%Webco\user\Debug\user.exp %to_dir%Webco\saf\user.exp

mkdir %to_dir%Resume\saf
move %to_dir%Resume\user\Debug\user.dll %to_dir%Resume\saf\user.dll
move %to_dir%Resume\user\Debug\user.lib %to_dir%Resume\saf\user.lib
move %to_dir%Resume\user\Debug\user.exp %to_dir%Resume\saf\user.exp

mkdir %to_dir%App\saf
move %to_dir%App\user\Debug\user.dll %to_dir%App\saf\user.dll
move %to_dir%App\user\Debug\user.lib %to_dir%App\saf\user.lib
move %to_dir%App\user\Debug\user.exp %to_dir%App\saf\user.exp


rem Remove excess garbage.

del %to_dir%Webco\Debug\*.* /q
rmdir %to_dir%Webco\setup /s /q
del %to_dir%Webco\webco.dsp
del %to_dir%Webco\webco.dsw
del %to_dir%Webco\webco.ncb
del %to_dir%Webco\webco.opt
del %to_dir%Webco\webco.plg
del %to_dir%Webco\output\*.* /q

del %to_dir%Resume\Debug\*.* /q
rmdir %to_dir%Resume\setup /s /q
del %to_dir%Resume\resume.dsp
del %to_dir%Resume\resume.dsw
del %to_dir%Resume\resume.ncb
del %to_dir%Resume\resume.opt
del %to_dir%Resume\resume.plg
del %to_dir%Resume\output\*.* /q

rem Remove User stuff.
del %to_dir%Webco\user\Debug\*.* /q
del %to_dir%Webco\user\user.ncb
del %to_dir%Webco\user\user.opt

rem Remove User stuff.
del %to_dir%Resume\user\Debug\*.* /q
del %to_dir%Resume\user\user.ncb
del %to_dir%Resume\user\user.opt

rem Remove the same stuff from APP area
echo Remove app area stuff.
del %to_dir%App\Debug\*.* /q
del %to_dir%App\app.dep
del %to_dir%App\app.dsp
del %to_dir%App\app.dsw
del %to_dir%App\app.ncb
del %to_dir%App\app.opt
del %to_dir%App\app.plg
del %to_dir%App\output\*.* /q
rem del %to_dir%App\data\doom.words

rem Remove User area also.
del %to_dir%App\user\Debug\*.* /q
del %to_dir%App\user\user.ncb
del %to_dir%App\user\user.opt

rem Put some things back where they belong.

copy %to_dir%Webco\saf\*.* %to_dir%Webco\user\Debug\
rmdir %to_dir%Webco\saf /s /q

copy %to_dir%App\saf\*.* %to_dir%App\user\Debug\
rmdir %to_dir%App\saf /s /q

copy %to_dir%Resume\saf\*.* %to_dir%Resume\user\Debug\
rmdir %to_dir%Resume\saf /s /q

rem final stuff

move %to_dir%App %to_dir%bin\App

copy c:\winnt\system32\mfc42.dll  %to_dir%bin\
copy c:\winnt\system32\mfc42d.dll  %to_dir%bin\
copy c:\winnt\system32\mfco42d.dll %to_dir%bin\
copy c:\winnt\system32\msvcrtd.dll %to_dir%bin\

rem Need to place these in new system dir, if absent.
mkdir %to_dir%sys
copy c:\winnt\system32\hhctrl.ocx  %to_dir%sys\
copy c:\winnt\system32\popup.ocx   %to_dir%sys\

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

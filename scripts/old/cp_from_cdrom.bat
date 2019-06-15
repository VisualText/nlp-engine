@echo off
rem  FILE:  cp_from_cdrom
rem  CR:    04/18/00 AM.
rem  SUBJ:  Copy mainstuff from a CDROM to D: drive.
rem         To have a copy of everything on Amnon's PC on here (exc programs).


if "%1" == "" goto ok


:bad
echo.
echo Usage: cp_from_cdrom
goto exit

:ok

echo.
echo Copy from cdrom.  OVERWRITES FILES, makes sure not to copy readonly.
echo (Control-C aborts)
echo.

rem from is the cdrom drive, to is the drive to copy to.
set to_dir=c:
set fm_dir=d:

xcopy %fm_dir%\dev     %to_dir%\dev     /e /w /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy %fm_dir%\Admin %to_dir%\Admin\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem xcopy %fm_dir%\"My Installations" %to_dir%\"My Installations"\ /e /i /h /r
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

xcopy %fm_dir%\Taii %to_dir%\Taii\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

echo Done copying from cdrom.
goto exit

:lowmemory
echo.
echo Not enough memory for copying files.
echo Some may have been copied.
goto exit

:abort
echo.
echo Copy aborted.
goto exit

:exit

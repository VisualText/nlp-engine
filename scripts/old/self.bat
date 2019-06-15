@echo off
rem  SUBJ:  Batch file for creating a SELF-backup of critical stuff.
rem  CR:    04/15/99 AM.
rem  CALL:  self
rem         Moves stuff from drive 0 (c:) to drive 1 (j:).
rem  Note:  Meant for overwriting, just to have one backup copy of everything.
rem  Doesn't prune the destination.
rem  Not backing up Dave's DEV area.  Already in the J:\from Dave.

if "%1" == "" goto ok

:bad
echo.
echo Usage: self
echo (Takes no arguments).
echo      copies from c: to j:\backself
goto exit

:ok

echo.
echo Copying all relevant stuff from c:\ to j:\backself
echo (Control-C aborts)
echo.

set to_dir=j:\backself\


xcopy d:\apps %to_dir%apps\ /w /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy d:\cs %to_dir%cs\ /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\domain %to_dir%domain\ /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy d:\lib %to_dir%lib\ /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy d:\lite %to_dir%lite\ /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\"My Installations" %to_dir%"My Installations"\ /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\taii %to_dir%taii\ /e /h /r

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy d:\util %to_dir%util\ /e /h /r

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
echo Self backup aborted.
goto exit

:exit

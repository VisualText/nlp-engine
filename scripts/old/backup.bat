@echo off
rem  SUBJ:  Batch file for creating a backup of the apps and lite
rem         development areas.
rem  CR:    01/22/99 AM.
rem NOTE:   05/07/99 AM. Update to include C:\CS, the conceptual grammar stuff.
rem  CALL:  backup 990122
rem            will create the directory c:\backups\990122 appropriately

if "%1" == "" goto bad
if "%2" == "" goto ok

:bad
echo.
echo Usage: backup dir
echo Example: backup 990507
echo      copies c:\apps, c:\cs c:\lite, c:\lib to c:\backups\990507\
echo      NOTE - IT PRUNES THE DESTINATION OF DEBUG AREAS, ETC.
goto exit

:ok

echo.
echo Copying c:\apps, c:\cs, c:\lite, c:\lib to c:\backups\%1
echo (Control-C aborts)
echo.

set to_dir=c:\backups\%1\

xcopy c:\apps %to_dir%apps\ /w /e /h

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem COPY AND PRUNE THE C:\CS AREA.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy c:\cs %to_dir%cs\ /e /h

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rmdir %to_dir%cs\Consh\Debug /s /q
rmdir %to_dir%cs\Consh\tmp /s /q
del %to_dir%cs\Consh\consh.ncb
del %to_dir%cs\Consh\consh.opt

rmdir %to_dir%cs\libconsh\Debug /s /q
del %to_dir%cs\libconsh\libconsh.ncb
del %to_dir%cs\libconsh\libconsh.opt

rmdir %to_dir%cs\libkbm\Debug /s /q
del %to_dir%cs\libkbm\libkbm.ncb
del %to_dir%cs\libkbm\libkbm.opt

rmdir %to_dir%cs\libprim\Debug /s /q
del %to_dir%cs\libprim\libprim.ncb
del %to_dir%cs\libprim\libprim.opt

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy c:\lite %to_dir%lite\ /e /h

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy c:\lib %to_dir%lib\ /e /h

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem Remove excess garbage.

rmdir %to_dir%apps\webco\Debug /s /q
del %to_dir%apps\webco\webco.ncb
del %to_dir%apps\webco\webco.opt
del %to_dir%apps\webco\output\*.* /q
del %to_dir%apps\webco\data\doom.words

rem Remove User area also.
rmdir %to_dir%apps\webco\user\Debug /s /q
del %to_dir%apps\webco\user\user.ncb
del %to_dir%apps\webco\user\user.opt

rem Remove the same stuff from APP area
rmdir %to_dir%apps\App\Debug /s /q
del %to_dir%apps\App\App.ncb
del %to_dir%apps\App\App.opt
del %to_dir%apps\App\output\*.* /q
del %to_dir%apps\App\data\doom.words

rem Remove User area also.
rmdir %to_dir%apps\App\user\Debug /s /q
del %to_dir%apps\App\user\user.ncb
del %to_dir%apps\App\user\user.opt

rem Remove the same stuff from Resume area
rmdir %to_dir%apps\Resume\Debug /s /q
del %to_dir%apps\Resume\Resume.ncb
del %to_dir%apps\Resume\Resume.opt
del %to_dir%apps\Resume\output\*.* /q
del %to_dir%apps\Resume\data\doom.words

rem Remove User area also.
rmdir %to_dir%apps\Resume\user\Debug /s /q
del %to_dir%apps\Resume\user\user.ncb
del %to_dir%apps\Resume\user\user.opt

rmdir %to_dir%apps\spider\Debug /s /q
del %to_dir%apps\spider\spider.ncb
del %to_dir%apps\spider\spider.opt

rmdir %to_dir%apps\spidern\Debug /s /q
del %to_dir%apps\spidern\spidern.ncb
del %to_dir%apps\spidern\spidern.opt

rmdir %to_dir%lite\Debug /s /q
del %to_dir%lite\lite.ncb
del %to_dir%lite\lite.opt

rem Remove the same stuff from lib\web area
rmdir %to_dir%lib\web\Debug /s /q
del %to_dir%lib\web\web.ncb
del %to_dir%lib\web\web.opt

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

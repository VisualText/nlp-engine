@echo off
rem  Batch file for copying the include files from LITE to APPS and NLPLITE.
rem  01/18/99 AM.

set drv=d:

pushd %drv%\lite
copy global.h %drv%\apps\include
copy algo.h %drv%\apps\include
copy Agram.h %drv%\apps\include
rem copy Arug.h %drv%\apps\include
copy Aseq.h %drv%\apps\include
copy Avar.h %drv%\apps\include
copy code.h %drv%\apps\include
copy delt.h %drv%\apps\include
copy dir.h  %drv%\apps\include
copy dlist.h %drv%\apps\include
copy Gui.h  %drv%\apps\include
rem copy iarg.h %drv%\apps\include
copy lite.h %drv%\apps\include
copy mach.h %drv%\apps\include
copy nlp.h  %drv%\apps\include
rem copy pre.h  %drv%\apps\include

copy rug.h  %drv%\apps\include

rem Got rid of grunk.  12/11/99 AM.

rem copy Eana.h %drv%\apps\include
rem copy mode.h %drv%\apps\include

rem For apps\*\user projects	01/05/00 AM.
copy pre.h %drv%\apps\include

rem 02/22/00 AM. Removing some app project dependencies.
pushd %drv%\cs\include\consh
rem copy libconsh.h %drv%\apps\include
rem copy cg.h %drv%\apps\include


rem 10/08/99 AM. As part of synchronization, no longer need this stuff.
rem xcopy %drv%\apps\\include %drv%\dev\include\
rem if errorlevel 4 goto lowmemory
rem if errorlevel 2 goto abort

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
popd
popd

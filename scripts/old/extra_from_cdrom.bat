@echo off
rem  FILE:  extra_from_cdrom
rem  CR:    08/13/99 AM.
rem  SUBJ:  Copy extra stuff from a CDROM to D: drive.
rem         To have a copy of everything on Amnon's PC on here (exc programs).


if "%1" == "" goto ok


:bad
echo.
echo Usage: extra_from_cdrom
goto exit

:ok

echo.
echo Copy from cdrom.  OVERWRITES FILES, makes sure not to copy readonly.
echo (Control-C aborts)
echo.

set to_dir=d:

xcopy e:\ad     %to_dir%\ad\     /e /w /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"business data" %to_dir%\"business data"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\dict %to_dir%\dict\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\domain %to_dir%\domain\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\download %to_dir%\download\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\flexlm %to_dir%\flexlm\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\flicnet %to_dir%\flicnet\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"flicnet TAI 990717" %to_dir%\"flicnet TAI 990717"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"flicnet texanalysis" %to_dir%\"flicnet texanalysis"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\internic %to_dir%\internic\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\muc %to_dir%\muc\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"My Installations" %to_dir%\"My Installations"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\netowl %to_dir%\netowl\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"nlp web" %to_dir%\"nlp web"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\samples %to_dir%\samples\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\screens %to_dir%\screens\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\Tai %to_dir%\Tai\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\Taii %to_dir%\Taii\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\videos %to_dir%\videos\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"webco safe" %to_dir%\"webco safe"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\whois %to_dir%\whois\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"whois indiv" %to_dir%\"whois indiv"\ /e /i /h /r
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

xcopy e:\"wn dict" %to_dir%\"wn dict"\ /e /i /h /r
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

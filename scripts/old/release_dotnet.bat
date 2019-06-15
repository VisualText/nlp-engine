@echo off
rem  SUBJ:  Batch file for creating a release distribution.
rem  CR:    04/07/00 AM.
rem  CALL:  release
rem            will create the directory <drive>\rel.
rem		To save elsewhere, copy the REL directory manually.
rem	12/12/00 AM. Updating to reflect overhauled DEV area.

if "%1" == "" goto ok

:bad
echo.
echo Usage: release
echo        Creates <drive>\REL directory.
goto exit

:ok

echo.
echo Copying from development directories to release directory.
echo (Control-C aborts)
echo.

set to_dir=c:\rel\
set fm_dir=d:\dev

echo here
rem Keeping this hardwired so there will be NO MISTAKE!
rmdir /s /q c:\rel
echo here1

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle first guy.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
echo xcopy %fm_dir%\bin\web.dll %to_dir%anadll\ /w /e /h /i

xcopy %fm_dir%\bin\web.dll %to_dir%anadll\ /w /e /h /i

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle libraries.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\bin\words.dll    %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\lite.dll     %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\libprim.dll  %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\libkbm.dll   %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\libconsh.dll %to_dir%anadll\ /e /h /i

xcopy %fm_dir%\bin\DotNetAPI.dll %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\DotNetBridge.dll %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\DotNetTest.exe.manifest %to_dir%anadll\ /e /h /i
xcopy %fm_dir%\bin\DotNetTest.exe %to_dir%anadll\ /e /h /i


xcopy %fm_dir%\dll\ics.dll %to_dir%anadll\ /e /h /i

xcopy %fm_dir%\dll\*.dll  %to_dir%vtdll\ /e /h /i
xcopy "c:\program files\codejock software\mfc\toolkit\bin_vc71\XT4100Lib.dll"  %to_dir%vtdll\ /e /h /i


rem Getting a RoboHelp dll to avoid auto dialup. 4/5/01 AM.
rem xcopy "E:\Program Files\RoboHELP Office\RoboHTML\HHActiveX.dll" %to_dir%vtdll\ /h
xcopy %fm_dir%\bin\HHActiveX.dll %to_dir%vtdll\ /h

rem Need VC++ libraries....
rem Putting in the MFC library.  06/23/00 AM.
xcopy C:\WINDOWS\SYSTEM32\Shell32.DLL %to_dir%anadll\ /h
xcopy C:\WINDOWS\SYSTEM32\MFC71.DLL %to_dir%anadll\ /h
xcopy C:\WINDOWS\SYSTEM32\MFC71D.DLL %to_dir%anadll\ /h
xcopy C:\WINDOWS\SYSTEM32\MSVCP71.DLL %to_dir%anadll\ /h
xcopy C:\WINDOWS\SYSTEM32\MSVCP71D.DLL %to_dir%anadll\ /h
xcopy C:\WINDOWS\SYSTEM32\MSVCRT.DLL %to_dir%anadll\ /h
xcopy C:\WINDOWS\SYSTEM32\MSVCRTD.DLL %to_dir%anadll\ /h
xcopy "c:\program files\microsoft visual studio .net 2003\common7\ide\msvcr71.dll"  %to_dir%anadll\ /h
rem 01/08/02 AM.
rem xcopy C:\I386\MSVCP60.DLL %to_dir%anadll\ /h

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle compilable libs.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

xcopy %fm_dir%\lib\lite.exp     %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\libprim.exp  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\libkbm.exp   %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\libconsh.exp %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\web.exp      %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\words.exp    %to_dir%lib\ /e /h /i

xcopy %fm_dir%\lib\lite.lib     %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\libprim.lib  %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\libkbm.lib   %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\libconsh.lib %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\web.lib      %to_dir%lib\ /e /h /i
xcopy %fm_dir%\lib\words.lib    %to_dir%lib\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle TextAI data.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem DO NOT TAKE VISUALTEXT HEADERS.  04/09/01 AM.
xcopy %fm_dir%\include\Api %to_dir%include\Api /e /h /i
xcopy %fm_dir%\data %to_dir%data\ /e /h /i
del %to_dir%data\autoexec.bat
del %to_dir%data\doom.words
del %to_dir%data\readme.txt

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle VisualText
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
xcopy %fm_dir%\VisualText\release\VisualText.exe  %to_dir%exe\ /e /h /i

xcopy %fm_dir%\VisualText\*.ico  %to_dir%vt\ /h /i
xcopy %fm_dir%\VisualText\*.bmp  %to_dir%vt\ /h /i

rem Moved App up a level, calling it Templates.
xcopy %fm_dir%\Templates  %to_dir%Templates\ /e /h /i
rem xcopy %fm_dir%\VisualText\App  %to_dir%vt\App\ /e /h /i
rem xcopy %fm_dir%\VisualText\res  %to_dir%vt\res\ /e /h /i

rem Moved Help up a level.
xcopy %fm_dir%\Help\Help.chm  %to_dir%Help\ /h /i
rem xcopy %fm_dir%\VisualText\Help\Help.chm  %to_dir%vt\Help\ /h /i
rem xcopy %fm_dir%\VisualText\Help\*.gif  %to_dir%vt\Help\ /h /i
rem xcopy %fm_dir%\VisualText\Help\*.bmp  %to_dir%vt\Help\ /h /i
rem xcopy %fm_dir%\VisualText\Help\"Audio Files"  %to_dir%vt\Help\"Audio Files" /e /h /i
rem xcopy %fm_dir%\VisualText\Help\"Image Files"  %to_dir%vt\Help\"Image Files" /e /h /i
rem xcopy %fm_dir%\VisualText\Help\Videos  %to_dir%vt\Help\Videos /e /h /i
rem rem xcopy %fm_dir%\VisualText\Help\WebHelp  %to_dir%vt\Help\WebHelp /e /h /i

rem monolithic visualtext
mkdir %to_dir%exeT
rem xcopy %fm_dir%\VisualTextM\VisualText___Win32_Release_Mono\VisualText.exe %to_dir%exeT\ /e /h /i

mkdir %to_dir%exeTX
rem xcopy %fm_dir%\VisualTextM\VisualText___Win32_Release_Mono_Noweb\VisualText.exe %to_dir%exeTX\ /e /h /i

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle tutorials. 06/22/00 AM.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
xcopy %fm_dir%\docs  %to_dir%docs\ /e /h /i

rem REMOVE tutorial7 tutorial8
rem rmdir /s /q %to_dir%docs\tutorial7
rem rmdir /s /q %to_dir%docs\tutorial8

rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
rem Handle sample applications.
rem ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

rem Need to make apps folder in every case.
mkdir %to_dir%apps
echo here6
rem COPY SAMPLE ANALYZERS HERE.
rem dev-samples to dev-apps  12/11/03 AM.
xcopy %fm_dir%\apps\* %to_dir%apps /e /h /i

rem Method: Copy everything, then remove standard garbage.
rem xcopy %fm_dir%\apps\chatter  %to_dir%apps\chatter\ /e /h /i
rem Keeping the chatter exe
rem rmdir /s /q %to_dir%apps\chatter\Debug\
rem rmdir /s /q %to_dir%apps\chatter\Release\

rem rmdir /s /q %to_dir%apps\chatter\kb\Debug\
rem rmdir /s /q %to_dir%apps\chatter\kb\Release\
rem rmdir /s /q %to_dir%apps\chatter\user\Debug\
rem rmdir /s /q %to_dir%apps\chatter\user\Release\
rem rmdir /s /q %to_dir%apps\chatter\run\Debug\
rem rmdir /s /q %to_dir%apps\chatter\run\Release\

rem xcopy %fm_dir%\apps\resume  %to_dir%apps\resume\ /e /h /i
rem rmdir /s /q %to_dir%apps\resume\Debug\
rem rmdir /s /q %to_dir%apps\resume\user\Debug\
echo here7

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
rem del /f /s /q %to_dir%*.txt_log

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

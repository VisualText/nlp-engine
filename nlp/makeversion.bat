@Echo off
:: Check WMIC is available
WMIC.EXE Alias /? >NUL 2>&1 || GOTO s_error

set _arg0=%0
set _arg1=%1
set _arg2=%2
set _arg3=%3


if "%_arg1%"=="-o" (
    call %_arg0% | %_arg3% -o "%_arg2%"
)

:: Use WMIC to retrieve date and time
FOR /F "skip=1 tokens=1-6" %%G IN ('WMIC Path Win32_LocalTime Get Day^,Hour^,Minute^,Month^,Second^,Year /Format:table') DO (
   IF "%%~L"=="" goto s_done
      Set _yyyy=%%L
      Set _mm=00%%J
      Set _dd=00%%G
      Set _hour=00%%H
      SET _minute=00%%I
)
:s_done

:: Pad digits with leading zeros
      Set _mm=%_mm:~-2%
      Set _dd=%_dd:~-2%
      Set _hour=%_hour:~-2%
      Set _minute=%_minute:~-2%

:: Display the date/time in ISO 8601 format:
Set _isodate=%_yyyy%-%_mm%-%_dd% %_hour%:%_minute%

FOR /F "tokens=* USEBACKQ" %%F IN (`git describe --tags`) DO (
SET _repo=%%F
)

FOR /F "tokens=* USEBACKQ" %%F IN (`ver`) DO (
SET _wversion=%%F
)

Set _version=%_repo% (built %_isodate% with %_wversion%)

echo // This file was created by makeversion.bat on %_isodate%
echo // Do not edit - do not add to git
echo char const *Version() {
echo     return "@(#)%_version%" + 4;
echo }
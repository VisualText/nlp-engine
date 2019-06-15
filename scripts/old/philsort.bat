@echo off
rem  FILE:  philsort
rem  CR:    11/23/99 AM (copied from Phil email).
rem  SUBJ:  Partial sort of text lines.


if "%1" == "" goto bad
goto ok

:bad
echo.
echo Usage: extra_from_cdrom
goto exit

:ok

echo.
echo Sorts given file .txt to file.srt. Don't specify .TXT!
echo.

set to_dir=d:

sort /+1   /R <%1.txt >%1.tmp1
sort /+2   /R <%1.tmp1 >%1.tmp2
sort /+3   /R <%1.tmp2 >%1.tmp1
sort /+4   /R <%1.tmp1 >%1.tmp2
sort /+5   /R <%1.tmp2 >%1.tmp1
sort /+6   /R <%1.tmp1 >%1.tmp2
sort /+7   /R <%1.tmp2 >%1.tmp1
sort /+8   /R <%1.tmp1 >%1.tmp2
sort /+9   /R <%1.tmp2 >%1.tmp1
sort /+10  /R <%1.tmp1 >%1.tmp2
sort /+11  /R <%1.tmp2 >%1.tmp1
sort /+12  /R <%1.tmp1 >%1.tmp2
sort /+13  /R <%1.tmp2 >%1.tmp1
sort /+14  /R <%1.tmp1 >%1.tmp2
sort /+15  /R <%1.tmp2 >%1.tmp1
sort /+16  /R <%1.tmp1 >%1.tmp2
sort /+17  /R <%1.tmp2 >%1.tmp1
sort /+18  /R <%1.tmp1 >%1.tmp2
sort /+19  /R <%1.tmp2 >%1.tmp1
sort /+20  /R <%1.tmp1 >%1.tmp2
sort /+21  /R <%1.tmp2 >%1.tmp1
sort /+22  /R <%1.tmp1 >%1.tmp2
sort /+23  /R <%1.tmp2 >%1.tmp1
sort /+24  /R <%1.tmp1 >%1.tmp2
sort /+25  /R <%1.tmp2 >%1.tmp1
sort /+26  /R <%1.tmp1 >%1.tmp2
sort /+27  /R <%1.tmp2 >%1.tmp1
sort /+28  /R <%1.tmp1 >%1.tmp2
sort /+29  /R <%1.tmp2 >%1.tmp1
sort /+30  /R <%1.tmp1 >%1.tmp2
sort /+31  /R <%1.tmp2 >%1.tmp1
sort /+32  /R <%1.tmp1 >%1.tmp2
sort /+33  /R <%1.tmp2 >%1.tmp1
sort /+34  /R <%1.tmp1 >%1.tmp2
sort /+35  /R <%1.tmp2 >%1.tmp1
sort /+36  /R <%1.tmp1 >%1.tmp2
sort /+37  /R <%1.tmp2 >%1.tmp1
sort /+38  /R <%1.tmp1 >%1.tmp2
sort /+39  /R <%1.tmp2 >%1.tmp1
sort /+40  /R <%1.tmp1 >%1.tmp2
sort /+41  /R <%1.tmp2 >%1.tmp1
sort /+42  /R <%1.tmp1 >%1.tmp2
sort /+43  /R <%1.tmp2 >%1.tmp1
sort /+44  /R <%1.tmp1 >%1.tmp2
sort /+45  /R <%1.tmp2 >%1.tmp1
sort /+46  /R <%1.tmp1 >%1.tmp2
sort /+47  /R <%1.tmp2 >%1.tmp1
sort /+48  /R <%1.tmp1 >%1.tmp2
sort /+49  /R <%1.tmp2 >%1.tmp1
sort /+50  /R <%1.tmp1 >%1.srt
del %1.tmp1
del %1.tmp2

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

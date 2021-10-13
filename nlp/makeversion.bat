@echo off
for /f "tokens=1-4 delims=/ " %%i in ("%date%") do (
     set dow=%%i
     set month=%%j
     set day=%%k
     set year=%%l
)
set datestr=%month%/%day%/%year%
FOR /F "tokens=*" %%F IN ('git.exe tag ^| sort -V ^| tail -1') DO (
     set version=%%F
)
echo // This file was created on %datestr%
echo // Do not edit - do not add to git
echo char const *Version() {
echo     return "%version%";
echo }
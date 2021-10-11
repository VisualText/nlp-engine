@echo off
for /f "tokens=1-4 delims=/ " %%i in ("%date%") do (
     set dow=%%i
     set month=%%j
     set day=%%k
     set year=%%l
)
set datestr=%month%/%day%/%year%
FOR /F "tokens=*" %%F IN ('git.exe describe --tags') DO (
SET version=%%F
)
SET version=v1.8.4
echo // This file was created on %datestr%
echo // Do not edit - do not add to git
echo char const *Version() {
echo     return "%version%";
echo }
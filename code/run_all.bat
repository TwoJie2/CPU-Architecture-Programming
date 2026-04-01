@echo off
setlocal
cd /d "%~dp0"
if not exist results mkdir results
if not exist build mkdir build

echo [1/2] compiling with -O2...
gcc -O2 -std=c11 -Wall -Wextra src\main.c src\dot.c src\reduce.c src\timer.c -o build\lab1_O2.exe
if errorlevel 1 goto :fail

echo [2/2] running...
build\lab1_O2.exe --tag O2_default
if errorlevel 1 goto :fail

echo.
echo finished. csv files are in the results folder.
exit /b 0

:fail
echo.
echo build or run failed.
exit /b 1

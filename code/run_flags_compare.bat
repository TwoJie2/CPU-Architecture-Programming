@echo off
setlocal
cd /d "%~dp0"
if not exist results mkdir results
if not exist build mkdir build

for %%O in (0 2 3) do (
  echo ==========================================
  echo compiling with -O%%O
  gcc -O%%O -std=c11 -Wall -Wextra src\main.c src\dot.c src\reduce.c src\timer.c -o build\lab1_O%%O.exe
  if errorlevel 1 goto :fail
  echo running build\lab1_O%%O.exe
  build\lab1_O%%O.exe --tag O%%O
  if errorlevel 1 goto :fail
)

echo.
echo flag comparison finished. Please inspect results\*.csv .
exit /b 0

:fail
echo.
echo flag comparison failed.
exit /b 1

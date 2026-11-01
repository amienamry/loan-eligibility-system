@echo off
setlocal

where gcc >nul 2>nul
if errorlevel 1 (
    echo Error: GCC was not found. Install MinGW-w64 and add gcc to PATH.
    exit /b 1
)

gcc -std=c99 -Wall -Wextra -Wpedantic main.c -o loan-eligibility.exe
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build succeeded. Starting the application...
.\loan-eligibility.exe

@echo off
setlocal
cd /d "%~dp0"
if not exist "%~dp0Roadrash.exe" (
  echo Please extract the entire ZIP before starting the game.
  pause
  exit /b 1
)
start "" /wait "%~dp0Roadrash.exe"

@echo off
setlocal
cd /d "%~dp0"
set "RR_REG=%SystemRoot%\System32\reg.exe"
if exist "%SystemRoot%\SysWOW64\reg.exe" set "RR_REG=%SystemRoot%\SysWOW64\reg.exe"
"%RR_REG%" import "%~dp0Settings-32.reg"
if errorlevel 1 (
  echo.
  echo Setup failed. Right-click Setup.cmd and choose Run as administrator.
  pause
  exit /b 1
)
echo.
echo Setup completed. Close this window, then double-click Start.cmd.
echo Run Setup.cmd only once. It initializes the game's controls and settings.
pause
exit /b 0

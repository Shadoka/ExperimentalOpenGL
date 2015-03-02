@echo off

call ..\misc\shell.bat
set LIB=%CD%\lib;%LIB%
mkdir ..\build
pushd ..\build
ver | findstr /i "6\.1\." > nul
IF %ERRORLEVEL% EQU 0 goto win7
ver | findstr /i "6\.2\." > nul
IF %ERRORLEVEL% EQU 0 goto win8
ver | findstr /i "6\.3\." > nul
IF %ERRORLEVEL% EQU 0 goto win8
goto warning

:win7
cl /Zi ..\code\SimpleCube.cpp glew32.lib Xinput9_1_0.lib
goto exit

:win8
cl /Zi ..\code\SimpleCube.cpp glew32.lib Xinput.lib
goto exit

:warning
echo Your OS seems to be neither Win7 nor Win8. Too bad.

:exit
popd


@echo off

call ..\misc\shell.bat
mkdir ..\build
pushd ..\build
set LIB=%CD%\lib;%LIB%
cl ..\code\ExperimentalOpenGL.cpp glew32.lib
popd
pause


@echo off

call ..\misc\shell.bat
set LIB=%CD%\lib;%LIB%
mkdir ..\build
pushd ..\build
cl /Zi ..\code\ExperimentalOpenGL.cpp glew32.lib
popd


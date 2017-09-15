@echo off
subst v: c:/Dev/Programs/C++
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=v:/devprojectbuilder;%path%
code -n v:/devprojectbuilder
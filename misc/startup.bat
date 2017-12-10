@echo off
subst v: c:/Dev/Programs/C++
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path=v:/devprojectbuilder/misc;%path%
cd /D v:/devprojectbuilder
"C:\Program Files\emacs\bin\runemacs.exe" -q -l v:\devprojectbuilder\misc\.emacs
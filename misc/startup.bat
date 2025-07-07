@echo off
subst x: D:\Dev\Programs\C++
call "F:\Development\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set PATH=%PATH%;x:\DevProjectBuilder\misc
set PATH=%PATH%;F:\Development\remedybg_0_3_7_1
cd ..
start /b cmd.exe
start /b remedybg.exe D:\Dev\Programs\C++\DevProjectBuilder\build\devprojectbuilder.exe
F:\Development\4coder\4ed.exe -f 12 -W
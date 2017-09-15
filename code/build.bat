@echo off

set PathRoot=v:\devprojectbuilder
set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -FC -Z7 -wd4100
set CommonLinkerFlags= -incremental:no -opt:ref
set LinkFiles=Ole32.lib Shell32.lib

IF NOT EXIST %PathRoot%\build mkdir %PathRoot%\build
pushd %PathRoot%\build
REM 32bit build
REM cl %CommonCompilerFlags% %PathRoot%\code\devprojectbuilder.cpp /link -subsystem:windows,5.10 %CommonLinkerFlags%

REM 64bit build
del *.pdb > NUL 2> NUL
REM cl %CommonCompilerFlags% %PathRoot%\code\devprojectbuilder.cpp -Fmtest.map -Fd -LD /link -incremental:no -PDB:test_%date:~-4,4%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
cl %CommonCompilerFlags% %PathRoot%\code\devprojectbuilder.cpp /link %CommonLinkerFlags% %LinkFiles%
popd
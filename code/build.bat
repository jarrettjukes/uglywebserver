@echo off
pushd ..\build\
del *.pdb
cl ..\code\win32_uglywebserver.cpp -Z7 -Od -Gm- -EHa- -nologo /link /incremental:no "Ws2_32.lib"
popd

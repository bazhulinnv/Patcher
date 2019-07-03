@echo off
set config=RelWithDebInfo
set blddir=build

if exist %blddir% rmdir /q /s %blddir%
mkdir %blddir%
cd    %blddir%

cmake ..\ -G "Visual Studio 15 2017 Win64" -Wno-dev -DCMAKE_CONFIGURATION_TYPES=%config% || echo CMake project generation error! && exit %ERRORLEVEL%
cmake --build . --config %config%							 || echo CMake project building   error! && exit %ERRORLEVEL%

cd ..
for %%X in (%blddir%\bin\%config%\*.exe) do (@echo Running %%X ... && %%X)

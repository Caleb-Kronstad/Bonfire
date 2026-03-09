@echo off

call Windows-Config.bat

pushd ..

set BUILD_SYSTEM=%1
if "%BUILD_SYSTEM%"=="" set BUILD_SYSTEM=premake

if "%BUILD_SYSTEM%"=="cmake" (
    echo === Setting up CMake build system ===
    cmake -S . -B Build -G "Visual Studio 17 2022" -A x64
    echo === CMake setup complete ===
) else (
    echo === Setting up Premake build system ===
    Premake\Windows\premake5.exe --file=Build.lua vs2022
    echo === Premake setup complete ===
)

popd
pause

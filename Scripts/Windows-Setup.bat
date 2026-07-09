@echo off

call Windows-Config.bat

pushd ..

echo === Setting up Premake build system ===
Premake\Windows\premake5.exe --file=Build.lua vs2022
echo === Premake setup complete ===

popd
pause

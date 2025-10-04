@echo off

call Windows-Config.bat

pushd ..

echo === Setting up build ===
Premake\Windows\premake5.exe --file=Build.lua vs2022

echo === Setup complete ===

popd
pause
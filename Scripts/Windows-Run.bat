@echo off

call Windows-Config.bat

echo === Running build ===
pushd ..\Build\Binaries\windows-x86_64\%BUILD_CONFIG%\Project
Project.exe
popd
pause
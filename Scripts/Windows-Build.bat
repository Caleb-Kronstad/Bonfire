@echo off

pushd ..

set MSBUILD= "C:\Program Files\Microsoft Visual Studio\2022\Preview\MSBuild\Current\Bin\MSBuild.exe"

echo === Cleaning build ===
%MSBUILD% Bonfire.sln /t:Clean /p:Configuration=Debug /v:minimal

echo === Building project (debug) ===
%MSBUILD% Bonfire.sln /t:Build /p:Configuration=Debug /v:minimal

echo === Build complete ===

popd
pause
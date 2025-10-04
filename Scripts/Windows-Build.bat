@echo off

call Windows-Config.bat

pushd ..

echo === Cleaning build ===
%MSBUILD% Build/Bonfire.sln /t:Clean /p:Configuration=%BUILD_CONFIG% /v:minimal

echo === Building project (debug) ===
%MSBUILD% Build/Bonfire.sln /t:Build /p:Configuration=%BUILD_CONFIG% /v:minimal

echo === Build complete ===

popd
pause
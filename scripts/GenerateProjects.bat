@echo off
pushd %~dp0\..\
cmake -B build -G "Visual Studio 17 2022"
popd
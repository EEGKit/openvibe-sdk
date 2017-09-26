REM Install OV SDK dependencies
REM This script can be copied, renamed as `windows-install-dependencies-custom.cmd`
REM and updated with the right credentials in PROXYPASS, such as username:password
REM 
@echo off

echo Install all dependencies

REM In order to download dependencies, PROXYPASS must be changed with appropriate credentials
REM set PROXYPASS=XXX:XXX
set URL=https://extranet.mensiatech.com/dependencies

if not exist "..\dependencies_x64\arch\build\windows" ( mkdir "..\dependencies_x64\arch\build\windows" )
if not exist "..\dependencies\arch\data" ( mkdir "..\dependencies\arch\data" )

REM Install build tools, CMake, Ninja, comment line if you already have an instance of CMake installed
powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-get-dependencies.ps1" -manifest_file .\windows-build-tools.txt -dest_dir .\..\dependencies_x64
REM Install OV SDK dependencies
powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-get-dependencies.ps1" -manifest_file .\windows-dependencies-x64.txt -dest_dir .\..\dependencies_x64
REM Retrieve test data
powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-get-dependencies.ps1" -manifest_file .\tests-data.txt

pause

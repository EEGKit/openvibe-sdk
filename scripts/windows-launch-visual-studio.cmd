@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 

call "windows-initialize-environment.cmd"

set BuildType=Release

for %%A in (%*) DO (
	if /i "%%A"=="--debug" (
		set BuildType=Debug
	)
)

SET "OV_PATH_ROOT=%CD%\..\..\openvibe-sdk-build\dist\%BuildType%"
SET "PATH=%OV_PATH_ROOT%\bin;%PATH%"

if not defined USE_EXPRESS (
	SET USE_EXPRESS=1
)

set SolutionPath=%CD%\..\..\openvibe-sdk-build\vs-project\OpenVIBE.sln

if %USE_EXPRESS% == 1 (
	echo Use %VSCMake% Express Edition
	
	if "%VSCMake%"=="Visual Studio 12"  (
		start /b "%VSINSTALLDIR%\Common7\IDE\WDExpress.exe" %SolutionPath%
	) else (
		"%VSINSTALLDIR%\Common7\IDE\VCExpress.exe" %SolutionPath%
	)
) else (
	echo Use %VSCMake%
	"%VSINSTALLDIR%\Common7\IDE\devenv.exe" %SolutionPath%
)

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

SET "OV_PATH_ROOT=%CD%\..\..\certivibe-build\dist-%BuildType%"
SET "OV_PATH_BIN=%OV_PATH_ROOT%\bin"
SET "OV_PATH_DATA=%OV_PATH_ROOT%\share\openvibe"
SET "OV_PATH_LIB=%OV_PATH_ROOT%\bin"
SET "PATH=%OV_PATH_ROOT%\bin;%PATH%"

if not defined USE_EXPRESS (
	SET USE_EXPRESS=0
)

set SolutionPath=%CD%\..\..\certivibe-build\vs-project-%BuildType%\OpenVIBE.sln

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

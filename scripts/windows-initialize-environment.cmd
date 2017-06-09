@echo off

set PATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\bin;%PATH%
set "SCRIPT_PATH=%~dp0"

if "%1"=="" (
	set "PATH_DEPENDENCIES=%SCRIPT_PATH%\..\dependencies"
) else (
	set "PATH_DEPENDENCIES=%1"
)
set PATH=%PATH_DEPENDENCIES%\boost\bin;%PATH%
set PATH=%PATH_DEPENDENCIES%\cmake\bin;%PATH%
set PATH=%PATH_DEPENDENCIES%\expat\bin;%PATH%
set PATH=%PATH_DEPENDENCIES%\ninja;%PATH%
set PATH=%PATH_DEPENDENCIES%\tvicport\bin;%PATH%
set PATH=%PATH_DEPENDENCIES%\vcredist;%PATH%
set PATH=%PATH_DEPENDENCIES%\zip;%PATH%
set PATH=%PATH_DEPENDENCIES%\xerces-c\lib;%PATH%

REM # Set to 1 to skip new compilers.
if not defined SKIP_VS2017 (
	SET SKIP_VS2017=1
)
if not defined SKIP_VS2015 (
	SET SKIP_VS2015=1
)
if not defined SKIP_VS2013 (
	SET SKIP_VS2013=0
)

set VSTOOLS=
set VSCMake=

if %SKIP_VS2017% == 1 (
	echo Visual Studio 2017 detection skipped as requested
) else (
	if exist "%VS150COMNTOOLS%vsvars32.bat" (
		echo Found VS150 tools at "%VS150COMNTOOLS%" ...
		CALL "%VS150COMNTOOLS%vsvars32.bat"
		SET VSCMake=Visual Studio 15
		goto terminate
	)
)

if %SKIP_VS2015% == 1 (
	echo Visual Studio 2015 detection skipped as requested
) else (
	if exist "%VS140COMNTOOLS%vsvars32.bat" (
		echo Found VS140 tools at "%VS140COMNTOOLS%" ...
		CALL "%VS140COMNTOOLS%vsvars32.bat"
		SET VSCMake=Visual Studio 14
		goto terminate
	)
)

if %SKIP_VS2013% == 1 (
	echo Visual Studio 2013 detection skipped as requested
) else (
	if exist "%VS120COMNTOOLS%vsvars32.bat" (
		echo Found VS120 tools at "%VS120COMNTOOLS%" ...
		CALL "%VS120COMNTOOLS%vsvars32.bat"
		SET VSCMake=Visual Studio 12
		goto terminate
	)
)

:terminate

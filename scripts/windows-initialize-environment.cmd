@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 
 
set PATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\bin;%PATH%
set "SCRIPT_PATH=%~dp0"

:parameter_parse

if /i "%1" == "--dependencies-dir" (
	set "PATH_DEPENDENCIES=%2"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--platform-target" (
	set PlatformTarget=%2
	if "%2"=="x64" (
		set PLATFORM=x64
		set VSPLATFORMGENERATOR=Win64
	) else if "%2"=="x86" (
		set PLATFORM=x86
		set VSPLATFORMGENERATOR=
	) else (
		echo Unknown platform %2 target
		Goto terminate
	)
	SHIFT
	SHIFT

	Goto parameter_parse
) else if not "%1" == "" (
	echo unrecognized option [%1]
	Goto terminate_error
)

if not defined PATH_DEPENDENCIES (
	if %PLATFORM%==x64 (
		SET "PATH_DEPENDENCIES=%SCRIPT_PATH%../dependencies_x64"
	) else (
		SET "PATH_DEPENDENCIES=%SCRIPT_PATH%../dependencies"
	)
)

set PATH=%PATH_DEPENDENCIES%/boost/bin;%PATH%
set PATH=%PATH_DEPENDENCIES%/cmake/bin;%PATH%
set PATH=%PATH_DEPENDENCIES%/expat/bin;%PATH%
set PATH=%PATH_DEPENDENCIES%/ninja;%PATH%
set PATH=%PATH_DEPENDENCIES%/tvicport/bin;%PATH%
set PATH=%PATH_DEPENDENCIES%/vcredist;%PATH%
set PATH=%PATH_DEPENDENCIES%/zip;%PATH%
set PATH=%PATH_DEPENDENCIES%/xerces-c/lib;%PATH%
set PATH=%PATH_DEPENDENCIES%/gtk/bin;%PATH%

REM ########################################################################################################################

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

SET VSTOOLS=
SET VSCMake=
set VCVARSALLPATH=../../VC/vcvarsall.bat

if %SKIP_VS2017% == 0 (
	set "VSTOOLS=%VS150COMNTOOLS%"
	set VSCMake=Visual Studio 15 2017
) else if %SKIP_VS2015% == 0 (
	echo Visual Studio 2017 detection skipped as requested
	set "VSTOOLS=%VS140COMNTOOLS%"
	set VSCMake=Visual Studio 14 2015
) else (
	echo Visual Studio 2017 detection skipped as requested
	echo Visual Studio 2015 detection skipped as requested
	set "VSTOOLS=%VS120COMNTOOLS%"
	set VSCMake=Visual Studio 12 2013
)

echo VStools: %VSTOOLS%, VSCMake: %VSCMake%.
if exist "!VSTOOLS!%VCVARSALLPATH%" (
	if %PlatformTarget% == x64 (
		if exist "!VSTOOLS!../../VC/bin/x64" (
			echo Found %VSCMake% tools: !VSTOOLS!%VCVARSALLPATH% %PlatformTarget%
			call "!VSTOOLS!%VCVARSALLPATH%" %PlatformTarget%
		) else (
			echo Found %VSCMake% tools: !VSTOOLS!%VCVARSALLPATH% x86_amd64
			call "!VSTOOLS!%VCVARSALLPATH%" x86_amd64
		)
	) else (
		echo Found %VSCMake% tools: !VSTOOLS!vsvars32.bat
		call "!VSTOOLS!vsvars32.bat"
	)
	goto terminate
)

set VSCMake=!VSCMake! %VSPLATFORMGENERATOR%

goto terminate_success

:terminate_error

echo An error occured during environment initializing !

pause
exit 1

REM #######################################################################################
:terminate_success

goto terminate

REM #######################################################################################
:terminate

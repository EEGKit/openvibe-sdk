@echo off

set PATH=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\bin;%PATH%
set "SCRIPT_PATH=%~dp0"
set "PATH_DEPENDENCIES=%SCRIPT_PATH%\..\dependencies"

if /i "%1" == "--dependencies-dir" (
	set "PATH_DEPENDENCIES=%1"
) else if /i "%1" == "--platform-target" (
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

set VCVARSALLPATH=../../VC/vcvarsall.bat

if %SKIP_VS2017% == 1 (
	echo Visual Studio 2017 detection skipped as requested
) else (
	if exist "%VS150COMNTOOLS%%VCVARSALLPATH%" (
		echo Found VS150 tools at "%VS150COMNTOOLS%%VCVARSALLPATH%" ...
		call "%VS150COMNTOOLS%%VCVARSALLPATH%" %PLATFORM%
		set VSCMake=Visual Studio 15
		if %PLATFORM% == x64 (
			set VSCMake=!VSCMake! %VSPLATFORMGENERATOR%
		)
		goto terminate
	)
)

if %SKIP_VS2015% == 1 (
	echo Visual Studio 2015 detection skipped as requested
) else (
	if exist "%VS140COMNTOOLS%%VCVARSALLPATH%" (
		echo Found VS140 tools at "%VS140COMNTOOLS%%VCVARSALLPATH%" ...
		call "%VS140COMNTOOLS%%VCVARSALLPATH%" %PLATFORM%
		set VSCMake=Visual Studio 14
		if %PLATFORM% == x64 (
			set VSCMake=!VSCMake! %VSPLATFORMGENERATOR%
		)
		goto terminate
	)
)

if %SKIP_VS2013% == 1 (
	echo Visual Studio 2013 detection skipped as requested
) else (
	if exist "%VS120COMNTOOLS%%VCVARSALLPATH%" (
		echo Found VS120 tools at "%VS120COMNTOOLS%%VCVARSALLPATH%" ...
		call "%VS120COMNTOOLS%%VCVARSALLPATH%" %PLATFORM%
		set VSCMake=Visual Studio 12
		if %PLATFORM% == x64 (
			set VSCMake=!VSCMake! %VSPLATFORMGENERATOR%
		)
		goto terminate
	)
)

:terminate

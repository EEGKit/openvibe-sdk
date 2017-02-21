@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 

set BuildType=Release
set PauseCommand=pause
set RerunCmake=false
set PackageOption=FALSE

goto parameter_parse

:print_help
	echo Usage: win32-build.cmd [-h ^| --help] [--no-pause] [-d^|--debug] [-r^|--release] [--make-package] [--rerun-cmake]
	echo -- Build Type option can be : --release (-r) or --debug (-d). Default is Release.
	exit /b

:parameter_parse
for %%A in (%*) DO (
	if /i "%%A"=="-h" (
		goto print_help
	) else if /i "%%A"=="--help" (
		goto print_help
	) else if /i "%%A"=="--no-pause" (
		set PauseCommand=echo ""
	) else if /i "%%A"=="-d" (
		set BuildType=Debug
	) else if /i "%%A"=="--debug" (
		set BuildType=Debug
	) else if /i "%%A"=="-r" (
		set BuildType=Release
	) else if /i "%%A"=="--release" (
		set BuildType=Release
	) else if /i "%%A"=="--make-package" (
		set PackageOption=TRUE
	) else if /i "%%A"=="--rerun-cmake" (
		set RerunCmake="true"
	)
)

echo Build type is set to: %BuildType%.

setlocal

call "windows-initialize-environment.cmd"

set script_dir=%CD%
set build_dir=%script_dir%\..\..\certivibe-build\build-%BuildType%
set install_dir=%script_dir%\..\..\certivibe-build\dist-%BuildType%

mkdir %build_dir% 2>NUL
pushd %build_dir%

set CallCmake=false
if not exist "%build_dir%\CMakeCache.txt" set CallCmake="true"
if %RerunCmake%=="true" set CallCmake="true"
if %CallCmake%=="true" (
	cmake %script_dir%\.. -G"Ninja" -DCMAKE_BUILD_TYPE=%BuildType% -DCMAKE_INSTALL_PREFIX=%install_dir% -DOV_PACKAGE=%PackageOption%
)

if not "!ERRORLEVEL!" == "0" goto terminate_error

ninja install

if not "!ERRORLEVEL!" == "0" goto terminate_error


if %PackageOption% == TRUE (
	cmake --build . --target package
)


if not "!ERRORLEVEL!" == "0" goto terminate_error

goto terminate_success

:terminate_error

echo An error occured during building process !

%PauseCommand%

goto terminate

:terminate_success

%PauseCommand%

goto terminate

:terminate

popd

endlocal

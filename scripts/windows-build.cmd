@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 

set BuildType=Release
set PauseCommand=pause
set RerunCmake=false
set PackageOption=FALSE
set UserDataSubdir=OpenVIBE

goto parameter_parse

:print_help
	echo Usage: %0 [options]
    echo.
	echo -h ^|--help usage
	echo --no-pause will not pause during script execution
	echo -d^|--debug build in debug mode
	echo -r^|--release build in release mode
	echo --make-package make packages at the end
	echo --rerun-cmake force cmake rerun
	echo --userdata-subdir [dirname] name of the userdata sub directory
	echo --build-unit build unit tests
	echo --build-validation build validation tests

	echo --test-data-dir [dirname] test data directory
	echo --test-output-dir [dirname] test output files directory
	echo.
	exit /b
	
	

:parameter_parse
for %%A in (%*) DO (
	if defined next_is_test_data_dir (
		set ov_cmake_test_data="-DOVT_TEST_DATA_DIR=%%A"
		set next_is_test_data_dir=
	) else if defined next_is_test_output_dir (
		set ov_cmake_test_output="%%A"
		set next_is_test_output_dir=
	) else if /i "%%A"=="-h" (
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
	) else if /i "%%A" == "--build-unit" (
		set ov_build_unit=true
	) else if /i "%%A" == "--build-validation" (
		set ov_build_validation=true
	) else if /i "%%A" == "--test-data-dir" (
		set next_is_test_data_dir=1
	) else if /i "%%A" == "--test-output-dir" (
		set next_is_test_output_dir=1
	) else if /i "%%A"=="--userdata-subdir" (
		set next=USERDATA_SUBDIR
	) else if "!next!"=="USERDATA_SUBDIR" (
		set UserDataSubdir=%%A
		set next=
	)
)

echo Build type is set to: %BuildType%.

setlocal

call "windows-initialize-environment.cmd"

set script_dir=%CD%
set build_dir=%script_dir%\..\..\certivibe-build\build-%BuildType%
set install_dir=%script_dir%\..\..\certivibe-build\dist-%BuildType%

if not defined ov_cmake_test_output (
	set ov_cmake_test_output=%build_dir%\validation-test-output\
)


mkdir %build_dir% 2>NUL
pushd %build_dir%

set CallCmake=false
if not exist "%build_dir%\CMakeCache.txt" set CallCmake="true"
if %RerunCmake%=="true" set CallCmake="true"
if %CallCmake%=="true" (
	cmake %script_dir%\.. -G"Ninja" ^
		-DCMAKE_BUILD_TYPE=%BuildType% ^
		-DCMAKE_INSTALL_PREFIX=%install_dir% ^
		-DOV_PACKAGE=%PackageOption% ^
		-DBUILD_UNIT_TEST=%ov_build_unit% ^
		-DBUILD_VALIDATION_TEST=%ov_build_validation% ^
		%ov_cmake_test_data% ^
		-DOVT_VALIDATION_TEST_OUTPUT_DIR=%ov_cmake_test_output% ^
		-DOV_CONFIG_SUBDIR=%UserDataSubdir%
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

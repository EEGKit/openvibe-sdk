@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 

set SKIP_VS2017=1
set SKIP_VS2015=1
set BuildType=Release
set PauseCommand=pause
set RerunCmake=false
set PackageOption=FALSE
set UserDataSubdir=OpenVIBE
set BrandName=OpenViBE
set DisplayErrorLocation=ON

goto parameter_parse

:print_help
	echo Usage: %0 [options]
    echo.
	echo -h ^|--help usage
	echo --no-pause will not pause during script execution
	echo -d^|--debug build in debug mode
	echo -r^|--release build in release mode
	echo --make-package make packages at the end
	echo --hide-error-location do not display complete error locations
	echo --rerun-cmake force cmake rerun
	echo --userdata-subdir [dirname] name of the userdata sub directory
	echo --brand-name [brand name] name of the brand to prefix titles and documentation
	echo --build-unit build unit tests
	echo --build-validation build validation tests

	echo --test-data-dir [dirname] test data directory
	echo --test-output-dir [dirname] test output files directory
	echo --python-exec [path] path to the python executable to use

	echo.
	exit /b
	

:parameter_parse

if /i "%1" == "-h" (
	goto print_help
) else if /i "%1" == "--help" (
	goto print_help
) else if /i "%1" == "--no-pause" (
	set PauseCommand=echo ""
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "-d" (
	set BuildType=Debug
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--debug" (
	set BuildType=Debug
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "-r" (
	set BuildType=Release
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--release" (
	set BuildType=Release
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--make-package" (
	set PackageOption=TRUE
	set DisplayErrorLocation=OFF
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--hide-error-location" (
	set DisplayErrorLocation=OFF
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--rerun-cmake" (
	set RerunCmake="true"
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--build-unit" (
	set ov_build_unit=true
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--build-validation" (
	set ov_build_validation=true
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--test-data-dir" (
	set ov_cmake_test_data="-DOVT_TEST_DATA_DIR=%2"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--test-output-dir" (
	set ov_cmake_test_output="%2"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--python-exec" (
	set python_exec="-DPYTHON_EXECUTABLE=%2"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--userdata-subdir" (
	set UserDataSubdir="%2"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1" == "--brand-name" (
	set BrandName="%2"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if not "%1" == "" (
	echo unrecognized option [%1]
	Goto terminate_error
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
		-DOV_DISPLAY_ERROR_LOCATION=%DisplayErrorLocation% ^
		-DBUILD_UNIT_TEST=%ov_build_unit% ^
		-DBUILD_VALIDATION_TEST=%ov_build_validation% ^
		%ov_cmake_test_data% ^
		-DBRAND_NAME=%BrandName% ^
		-DOV_CONFIG_SUBDIR=%UserDataSubdir% ^
		-DOVT_VALIDATION_TEST_OUTPUT_DIR=%ov_cmake_test_output% ^
		%python_exec%
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

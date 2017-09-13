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
set DependenciesPath=
set init_env_cmd=windows-initialize-environment.cmd
set generator=-G"Ninja"
set builder=Ninja
set script_dir=%~dp0

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

	echo --build-dir [dirname] build directory
	echo --install-dir [dirname] binaries deployment directory
	echo --dependencies-dir [dirname] directory where dependencies are located
	echo --test-data-dir [dirname] test data directory
	echo --test-output-dir [dirname] test output files directory
	echo --python-exec [path] path to the python executable to use
	echo --vsproject Create visual studio project (.sln)
	echo --vsbuild Create visual studio project (.sln) and compiles it

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
) else if /i "%1"=="--build-dir" (
	set build_dir=%2
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1"=="--install-dir" (
	set install_dir=%2
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1"=="--dependencies-dir" (
	set DependenciesPath="-DOV_CUSTOM_DEPENDENCIES_PATH=%2"
	set init_env_cmd=windows-initialize-environment.cmd %2
	REM -DOV_SOURCE_DEPENDENCIES_PATH=%2\dependencies-source"
	SHIFT
	SHIFT
	Goto parameter_parse
) else if /i "%1"=="--vsproject" (
	set vsgenerate=TRUE
	set builder=None
	SHIFT
	Goto parameter_parse
) else if /i "%1"=="--vsbuild" (
	set vsgenerate=TRUE
	set builder=Visual
	SHIFT
	Goto parameter_parse
) else if not "%1" == "" (
	echo unrecognized option [%1]
	Goto terminate_error
)

if defined vsgenerate (
	echo Build type is set to: MultiType.
) else (
	echo Build type is set to: %BuildType%.
)


setlocal

call %init_env_cmd%

if defined vsgenerate (
	set generator=-G"%VSCMake%" -T "v120"
	if not defined build_dir (
		set build_dir=%script_dir%\..\..\openvibe-sdk-build\vs-project
	)
	if not defined install_dir (
		set install_dir=%script_dir%\..\..\openvibe-sdk-build\dist
	)
) else (
	set build_type="-DCMAKE_BUILD_TYPE=%BuildType%"
	if not defined build_dir (
		set build_dir=%script_dir%\..\..\openvibe-sdk-build\build-%BuildType%
	)
	if not defined install_dir (
		set install_dir=%script_dir%\..\..\openvibe-sdk-build\dist-%BuildType%
	)
)
if not defined ov_cmake_test_output (
	set ov_cmake_test_output=%build_dir%\validation-test-output\
)

mkdir %build_dir% 2>NUL
pushd %build_dir%

set CallCmake=false

if not exist "%build_dir%\CMakeCache.txt" set CallCmake="true"
if %RerunCmake%=="true" set CallCmake="true"
if %CallCmake%=="true" (
	cmake %script_dir%\.. %generator% ^
		%build_type% ^
		-DCMAKE_INSTALL_PREFIX=%install_dir% ^
		-DOV_PACKAGE=%PackageOption% ^
		-DOV_DISPLAY_ERROR_LOCATION=%DisplayErrorLocation% ^
		-DBUILD_UNIT_TEST=%ov_build_unit% ^
		-DBUILD_VALIDATION_TEST=%ov_build_validation% ^
		%ov_cmake_test_data% ^
		-DBRAND_NAME=%BrandName% ^
		-DOV_CONFIG_SUBDIR=%UserDataSubdir% ^
		-DOVT_VALIDATION_TEST_OUTPUT_DIR=%ov_cmake_test_output% ^
		%python_exec% ^
		%DependenciesPath%
)

if not "!ERRORLEVEL!" == "0" goto terminate_error
if !builder! == None (
	goto terminate_success
) else if !builder! == Ninja (
	ninja install
	if not "!ERRORLEVEL!" == "0" goto terminate_error
) else if !builder! == Visual (
	msbuild OpenVIBE.sln /p:Configuration=%BuildType%
	if not "!ERRORLEVEL!" == "0" goto terminate_error

	cmake --build . --config %BuildType% --target install
	if not "!ERRORLEVEL!" == "0" goto terminate_error
)
if %PackageOption% == TRUE (
	cmake --build . --target package
	if not "!ERRORLEVEL!" == "0" goto terminate_error
)

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

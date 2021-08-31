project(OpenViBESDKTest VERSION ${OPENVIBE_MAJOR_VERSION}.${OPENVIBE_MINOR_VERSION}.${OPENVIBE_PATCH_VERSION})

# ------------------
# Project variables
# ------------------

if(NOT DEFINED OVT_TEST_DATA_DIR)
	message(FATAL_ERROR "Path to tests input data files is not set")
endif()

# Set the test ouptut directory for output tests files storage
set(OVT_VALIDATION_TEST_OUTPUT_DIR ${PROJECT_BINARY_DIR}/validation-test-output/ CACHE PATH "Path for the output result and configuration file create during test")

# ----------------------
# Project configuration
# ----------------------

# Add python tools path
set(OVT_PYTHON_TOOL_DIR  ${PROJECT_SOURCE_DIR}/python-toolkit/)

# ---------------------
# Project dependencies
# ---------------------

# Add some search directory to module path
set(OVT_CMAKE_DIR ${OV_BASE_DIR}/cmake-modules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OVT_CMAKE_DIR})

# Modify library prefixes and suffixes to comply to Windows or Linux naming
if(WIN32)
	set(CMAKE_FIND_LIBRARY_PREFIXES "")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
elseif(APPLE)
	set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".a")
else()
	set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endif()

# Find main dependency
if(DEFINED Python3_EXECUTABLE AND NOT DEFINED Python3_Interpreter_FOUND)
	# Python exec was supplied through command line, just set it in the files and don't ask questions
	set(Python3_Interpreter_FOUND TRUE)
else()
	set(Python_ADDITIONAL_VERSIONS 3.7)
	find_package(Python3 COMPONENTS Interpreter Development)
endif()

if(NOT Python3_Interpreter_FOUND)
	# This is likely a messed-up PYTHONPATH/HOME ISSUE
	message(WARNING "Current PYTHONPATH likely not suitable for a Python 3 executable, \
please check that the following values are plausible :\n\
	\$ENV{PYTHONPATH} = $ENV{PYTHONPATH}\n\
	\$ENV{PYTHONHOME} = $ENV{PYTHONHOME}\n\
If the previous environement is inconsistent, please set it correctly \
or directly give Python3 executable path at build time using \"--python-exec\"\n\
These tests will be generated with a generic \"python\" call, please use the \"--python-dir\" \
option of the launcher to set the path to a working python3 directory.")
else()
	message(STATUS "Found Python3 at ${Python3_EXECUTABLE}")
endif()

# --------------------
# Test configuration
# --------------------

# On Windows and Linux, some environment setting scripts are needed to ease the testing process
set(OVT_SCRIPT_DIR ${OV_BASE_BIN_DIR}/scripts)
if(WIN32)
	# Wrapper script that set the environment and launch ctest
	# It is located at build directory root to be as close as possible as standard ctest use
	configure_file(${OV_BASE_DIR}/scripts/windows-ctest-launcher.cmd-base
				   ${OV_BASE_BIN_DIR}/ctest-launcher.cmd)
elseif(UNIX)
	# Wrapper script that set the environment and launch ctest
	# It is located at build directory root to be as close as possible as standard ctest use
	configure_file(${OV_BASE_DIR}/scripts/unix-ctest-launcher.sh-base
				   ${OV_BASE_BIN_DIR}/ctest-launcher.sh)

endif()

# ${PROJECT_BINARY_DIR}/Testing/Temporary is automatically created by CTest for log files
# We create a temporary subdir for our test that will be create before each test is run
# and removed afterwards (see CTestCustom.cmake.in)
set(OVT_TEST_TEMPORARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing/Temporary/OVT/)

# Create test data directory for big or binary files
file(MAKE_DIRECTORY ${OVT_TEST_DATA_DIR})

# Ctest custom options
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake @ONLY)

# Timeout must be set first
set (DART_TESTING_TIMEOUT 72000 CACHE STRING "Maximum time allowed before CTest will kill the test." FORCE)
include(CTest)
enable_testing()

# It might be useful to set some configuration token from cmake variable
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/openvibe/openvibe.conf.in ${CMAKE_CURRENT_BINARY_DIR}/data/openvibe/openvibe.conf @ONLY)

# Utility variable for subproject
set(OVT_OPENVIBE_DATA ${CMAKE_CURRENT_BINARY_DIR}/data/openvibe)
if(DEFINED DIST_BINDIR_ABSOLUTE)
	set(OVT_OPENVIBE_PLAYER ${DIST_BINDIR_ABSOLUTE}/openvibe-scenario-player${CMAKE_EXECUTABLE_SUFFIX})
else()
	set(OVT_OPENVIBE_PLAYER ${DIST_BINDIR}/openvibe-scenario-player${CMAKE_EXECUTABLE_SUFFIX})
endif()

if(UNIX)
	SET(OVT_OPENVIBE_PLAYER_LOG_FILE "$ENV{HOME}/.config/${OV_CONFIG_SUBDIR}/log/openvibe-scenario-player.log")
elseif(WIN32)
	SET(OVT_OPENVIBE_PLAYER_LOG_FILE "\"$ENV{APPDATA}/${OV_CONFIG_SUBDIR}/log/openvibe-scenario-player.log\"")
endif()

file(TO_CMAKE_PATH ${OVT_OPENVIBE_PLAYER_LOG_FILE} OVT_OPENVIBE_PLAYER_LOG_FILE)

set(OVT_VALIDATION_TOOLKIT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/python-toolkit)
# ----------------------
# CMake tree processing
# ----------------------

# Process validation tests
# When adding a new set of validation tests, test directory must be added here
add_subdirectory(openvibe-file-io-csv)
add_subdirectory(openvibe-file-io-openvibe)
add_subdirectory(openvibe-stimulation-timeout)
add_subdirectory(openvibe-regularized-csp)
add_subdirectory(openvibe-classification-lda)

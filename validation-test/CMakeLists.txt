PROJECT(CertiViBETest)

# ------------------
# Project variables
# ------------------

SET(OVT_GLOBAL_VERSION_MAJOR 0)
SET(OVT_GLOBAL_VERSION_MINOR 1)
SET(OVT_GLOBAL_VERSION_PATCH 0)
SET(OVT_GLOBAL_VERSION_STRING "${OVT_GLOBAL_VERSION_MAJOR}.${OVT_GLOBAL_VERSION_MINOR}.${OVT_GLOBAL_VERSION_PATCH}")

IF(NOT DEFINED OVT_TEST_DATA_DIR)
	MESSAGE(FATAL_ERROR "Path to tests input data files is not set")
ENDIF()

# Set the test ouptut directory for output tests files storage
SET(OVT_VALIDATION_TEST_OUTPUT_DIR ${PROJECT_BINARY_DIR}/validation-test-output/ CACHE PATH "Path for the output result and configuration file create during test")

# ----------------------
# Project configuration
# ----------------------

# Sort target into directories for better visualization in IDE
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET(OVT_VALIDATION_TESTS_FOLDER validation-tests)
SET(OVT_TOOLS_FOLDER tools)

# Add python tools path
SET(OVT_PYTHON_TOOL_DIR  ${PROJECT_SOURCE_DIR}/python-toolkit/)

# ---------------------
# Project dependencies
# ---------------------

# Add some search directory to module path
SET(OVT_CMAKE_DIR ${OV_BASE_DIR}/cmake-modules)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OVT_CMAKE_DIR})

# Modify library prefixes and suffixes to comply to Windows or Linux naming
IF(WIN32)
	SET(CMAKE_FIND_LIBRARY_PREFIXES "")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
ELSEIF(APPLE)
	SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dylib" ".a")
ELSE()
	SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
ENDIF()

# Find main dependency
IF(BUILD_VALIDATION_TEST)
	SET(Python_ADDITIONAL_VERSIONS 3.4)
	FIND_PACKAGE(PythonInterp 3.4 REQUIRED)
ENDIF()

# --------------------
# Test configuration
# --------------------

# On Windows and Linux, some environment setting scripts are needed to ease the testing process
SET(OVT_SCRIPT_DIR ${OV_BASE_BIN_DIR}/scripts)
IF(WIN32)
	# Wrapper script that set the environment and launch ctest
	# It is located at build directory root to be as close as possible as standard ctest use
	CONFIGURE_FILE(${OV_BASE_DIR}/scripts/windows-ctest-launcher.cmd-base
				   ${OV_BASE_BIN_DIR}/ctest-launcher.cmd)
ELSEIF(UNIX)
	# Wrapper script that set the environment and launch ctest
	# It is located at build directory root to be as close as possible as standard ctest use
	CONFIGURE_FILE(${OV_BASE_DIR}/scripts/unix-ctest-launcher.sh-base
				   ${OV_BASE_BIN_DIR}/ctest-launcher.sh)

ENDIF()

# ${PROJECT_BINARY_DIR}/Testing/Temporary is automatically created by CTest for log files
# We create a temporary subdir for our test that will be create before each test is run
# and removed afterwards (see CTestCustom.cmake.in)
SET(OVT_TEST_TEMPORARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Testing/Temporary/OVT/)

# Create test data directory for big or binary files
FILE(MAKE_DIRECTORY ${OVT_TEST_DATA_DIR})

# Ctest custom options
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake @ONLY)

# Timeout must be set first
SET (DART_TESTING_TIMEOUT 72000 CACHE STRING "Maximum time allowed before CTest will kill the test." FORCE)
INCLUDE(CTest)
ENABLE_TESTING()

# It might be useful to set some configuration token from cmake variable
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/data/openvibe/openvibe.conf.in ${CMAKE_CURRENT_BINARY_DIR}/data/openvibe/openvibe.conf @ONLY)

# Utility variable for subproject
SET(OVT_OPENVIBE_DATA ${CMAKE_CURRENT_BINARY_DIR}/data/openvibe)
SET(OVT_OPENVIBE_PLAYER ${DIST_BINDIR}/openvibe-scenario-player${CMAKE_EXECUTABLE_SUFFIX})

IF(UNIX)
	SET(OVT_OPENVIBE_PLAYER_LOG_FILE "$ENV{HOME}/.config/${OV_CONFIG_SUBDIR}/log/openvibe-scenario-player.log")
ELSEIF(WIN32)
	SET(OVT_OPENVIBE_PLAYER_LOG_FILE "$ENV{APPDATA}/${OV_CONFIG_SUBDIR}/log/openvibe-scenario-player.log")
ENDIF()

FILE(TO_CMAKE_PATH ${OVT_OPENVIBE_PLAYER_LOG_FILE} OVT_OPENVIBE_PLAYER_LOG_FILE)

SET(OVT_VALIDATION_TOOLKIT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/python-toolkit)
# ----------------------
# CMake tree processing
# ----------------------

# Process validation tests
# When adding a new set of validation tests, test directory must be added here
#ADD_SUBDIRECTORY(openvibe-file-io-csv)
ADD_SUBDIRECTORY(openvibe-file-io-openvibe)
ADD_SUBDIRECTORY(openvibe-stimulation-timeout)
ADD_SUBDIRECTORY(openvibe-regularized-csp)
ADD_SUBDIRECTORY(openvibe-classification-lda)

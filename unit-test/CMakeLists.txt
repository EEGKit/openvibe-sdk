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

# ----------------------
# Project configuration
# ----------------------

# Sort target into directories for better visualization in IDE
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET(OVT_UNIT_TESTS_FOLDER unit-tests)
SET(OVT_TOOLS_FOLDER tools)

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

SET(GTEST_ROOT ${OV_CUSTOM_DEPENDENCIES_PATH}/gtest)
FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})

# --------------------
# Test configuration
# --------------------

# On Windows and Linux, some environment setting scripts are needed to ease the testing process
SET(OVT_SCRIPT_DIR ${OV_BASE_BIN_DIR}/scripts)
IF(WIN32)
	# Script set the environment variable needed by the tests
	CONFIGURE_FILE(${OV_BASE_DIR}/scripts/windows-set-test-environment.cmd-base
				   ${OVT_SCRIPT_DIR}/openvibe-test-set-environment.cmd)
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
SET(OVT_TEST_TEMPORARY_DIR ${OV_BASE_BIN_DIR}/Testing/Temporary/OVT/)

# Create test data directory for big or binary files
FILE(MAKE_DIRECTORY ${OVT_TEST_DATA_DIR})

# Ctest custom options
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake @ONLY)

# Memcheck configuration
IF(UNIX)
	FIND_PROGRAM(MEMORYCHECK_COMMAND valgrind /usr/bin/valgrind)
	FILE(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/data/resources/valgrind.supp" MEMORYCHECK_SUPPRESSIONS_FILE)
	SET(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full --run-libc-freeres=no --show-possibly-lost=no" )
ELSEIF(WIN32)
	FIND_PROGRAM(MEMORYCHECK_COMMAND purify "C:/Program Files (x86)/")
ENDIF()

# Timeout must be set first
SET (DART_TESTING_TIMEOUT 72000 CACHE STRING "Maximum time allowed before CTest will kill the test." FORCE)
INCLUDE(CTest)
ENABLE_TESTING()

# Utility variable for subproject
SET(OVT_OPENVIBE_DATA ${CMAKE_CURRENT_BINARY_DIR}/data/openvibe)

# ----------------------
# CMake tree processing
# ----------------------

# Process tools first.
# Some important variables are defined within tools projects for their use by other
# projects as:
# - OVT_UNIT_TOOLKIT_INCLUDE_DIR
# - OVT_UNIT_TOOLKIT_TARGET_FILE
# - OVT_ACCURACY_TARGET_FILE
ADD_SUBDIRECTORY(unit-toolkit)

INCLUDE_DIRECTORIES(${OVT_UNIT_TOOLKIT_INCLUDE_DIR})

# Process unit tests
# When adding a new test driver, driver directory must be added here
ADD_SUBDIRECTORY(openvibe-module-csv)
ADD_SUBDIRECTORY(openvibe-module-fs)
ADD_SUBDIRECTORY(openvibe-module-ebml)
ADD_SUBDIRECTORY(openvibe-module-xml)
ADD_SUBDIRECTORY(openvibe-module-socket)
ADD_SUBDIRECTORY(openvibe-module-system)
ADD_SUBDIRECTORY(openvibe-base)
ADD_SUBDIRECTORY(openvibe-toolkit)
ADD_SUBDIRECTORY(openvibe-kernel)


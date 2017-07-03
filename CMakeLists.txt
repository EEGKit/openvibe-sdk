CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
cmake_policy(SET CMP0026 OLD)
cmake_policy(SET CMP0042 OLD)
cmake_policy(SET CMP0043 OLD)
cmake_policy(SET CMP0045 OLD)
cmake_policy(SET CMP0048 OLD)

PROJECT(OpenVIBE)

SET(OV_GLOBAL_VERSION_MAJOR 0)
SET(OV_GLOBAL_VERSION_MINOR 1)
SET(OV_GLOBAL_VERSION_PATCH 1)
SET(OV_GLOBAL_VERSION_STRING "${OV_GLOBAL_VERSION_MAJOR}.${OV_GLOBAL_VERSION_MINOR}.${OV_GLOBAL_VERSION_PATCH}")

# Default is to build to dist/. If you wish a custom install, set your own MAKE_INSTALL_PREFIX when you call CMake. Safest to do under a fakeroot.
MESSAGE(STATUS "Appending compilation flags...")

SET(OV_CONFIG_SUBDIR "${PROJECT_NAME}" CACHE STRING "Subdirectory under user directory when configuration and logs will be saved")
SET(OV_CUSTOM_DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/dependencies")
SET(OV_SOURCE_DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/dependencies-source")

SET(BRAND_NAME "OpenViBE" CACHE STRING "Brand with which titles and documentation will be prefixed")
OPTION(OV_DISPLAY_ERROR_LOCATION ON)
OPTION(PUBLISH_DOC_ASSETS "Publish plugin documentation assets" ON)

# If OpenMP is available, it will allow Eigen to use multiple cores in matrix math.
IF(NOT(APPLE))
	INCLUDE("FindOpenMP")
	IF(OPENMP_FOUND)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	ENDIF(OPENMP_FOUND)
ENDIF()


ADD_DEFINITIONS("-DOV_CONFIG_SUBDIR=\"${OV_CONFIG_SUBDIR}\"")
ADD_DEFINITIONS("-DOV_PROJECT_NAME=\"${PROJECT_NAME}\"")
ADD_DEFINITIONS("-DOV_VERSION_MAJOR=\"${OV_GLOBAL_VERSION_MAJOR}\"")
ADD_DEFINITIONS("-DOV_VERSION_MINOR=\"${OV_GLOBAL_VERSION_MINOR}\"")
ADD_DEFINITIONS("-DOV_VERSION_PATCH=\"${OV_GLOBAL_VERSION_PATCH}\"")

IF(NOT DEFINED OV_PACKAGE)
	SET(OV_PACKAGE FALSE)
ENDIF(NOT DEFINED OV_PACKAGE)

IF(WIN32)
	ADD_DEFINITIONS("-DNOMINMAX -DBOOST_ALL_NO_LIB")
	# Switch /arch:SSE2 enables vectorization. Remove if your CPU/compiler doesn't support it.
	SET(OV_EIGEN_FLAGS "/arch:SSE2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4355 /MP ${OV_EIGEN_FLAGS}")
	SET(OV_WIN32_BOOST_VERSION "1_55")
ELSEIF(APPLE)
	SET(OV_EIGEN_FLAGS "-msse2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall ${OV_EIGEN_FLAGS}")
ELSEIF(UNIX)
	# Switch -msse2 enables vectorization. Remove if your CPU/compiler doesn't support it.
	SET(OV_EIGEN_FLAGS "-msse2")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall ${OV_EIGEN_FLAGS}")
ELSE()
	MESSAGE(WARNING "Warning: unknown platform")
ENDIF()

IF(NOT CMAKE_BUILD_TYPE)
	MESSAGE(STATUS "Setting default build type to Release")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF()

IF(NOT CMAKE_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../certivibe-build/dist-${CMAKE_BUILD_TYPE}")
ENDIF(NOT CMAKE_INSTALL_PREFIX)

IF(OV_PACKAGE)
  SET(CMAKE_INSTALL_PREFIX .)
ENDIF()

SET(DOCUMENTATION_TEMP_DIRECTORY "${CMAKE_INSTALL_PREFIX}/doc-tmp")


# Print the used compilation parameters (for transparency)
GET_DIRECTORY_PROPERTY(TMP_DEFINITIONS COMPILE_DEFINITIONS)
MESSAGE(STATUS "Compilation flags used at source root: ")
MESSAGE(STATUS "  COMPILE_DEFINITIONS = '${TMP_DEFINITIONS}'")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS = '${CMAKE_CXX_FLAGS}'")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS_RELEASE = '${CMAKE_CXX_FLAGS_RELEASE}'")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS_DEBUG = '${CMAKE_CXX_FLAGS_DEBUG}'")

FUNCTION(SET_BUILD_PLATFORM)
	IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_x64)
	ELSEIF("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_i386)
	ELSE()
		ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_Unknown)
	ENDIF()

	IF(WIN32)
		ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
		ADD_DEFINITIONS(-DTARGET_OS_Windows)
		ADD_DEFINITIONS(-DTARGET_COMPILER_VisualStudio)
	ELSEIF(APPLE)
		ADD_DEFINITIONS(-fnon-call-exceptions)
		ADD_DEFINITIONS(-DTARGET_OS_MacOS)
		# ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_x64)
		ADD_DEFINITIONS(-DTARGET_COMPILER_LLVM)
	ELSEIF(UNIX)
		# ADD_DEFINITIONS(-fvisibility=hidden) # This flag should be present... man gcc
		ADD_DEFINITIONS(-fnon-call-exceptions)
		ADD_DEFINITIONS(-DTARGET_OS_Linux)
		ADD_DEFINITIONS(-DTARGET_COMPILER_GCC)
	ENDIF()

ENDFUNCTION()

# Custom cmakelist can be used to overwrite the default compilation & packaging parameters
# e.g. OEM_DISTRIBUTION
IF(EXISTS "${CMAKE_SOURCE_DIR}/CustomCMakeLists.txt")
	MESSAGE(STATUS "Found custom build settings")
	INCLUDE("${CMAKE_SOURCE_DIR}/CustomCMakeLists.txt")
ENDIF()

#################################################################
string(TOLOWER ${BRAND_NAME} LOWER_BRAND_NAME)
ADD_DEFINITIONS(-DBRAND_NAME="${BRAND_NAME}")

IF(OV_DISPLAY_ERROR_LOCATION)
	ADD_DEFINITIONS(-DOV_DISPLAY_ERROR_LOCATION)
ENDIF()
#################################################################

SET(OV_LAUNCHER_SOURCE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules/launchers")

# By setting SKIP[_FOLDER]* you can skip a subtree (example: SKIP_A_B_C skips folder a/b/c and all its subfolders if any)

# Skip building documentation
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

# a list of all project which will be filled by the directory traversal. This is needed to generate the documentation.
SET_PROPERTY(GLOBAL PROPERTY OV_PROP_CURRENT_PROJECTS "")
SET_PROPERTY(GLOBAL PROPERTY OV_PROP_CURRENT_PROJECTS_BUILD_DIR "")

# Used by the various Find* scripts to locate OpenViBE modules
SET(OV_BASE_DIR ${CMAKE_SOURCE_DIR})
SET(OV_BASE_BIN_DIR ${CMAKE_BINARY_DIR})
SET(OV_LIBRARY_DIR  ${CMAKE_BINARY_DIR}/lib)
# needed for making visual studio projects when this script is called without CMAKE_BUILD_TYPE
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG "TARGET_BUILDTYPE_Debug")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE "TARGET_BUILDTYPE_Release")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO "TARGET_BUILDTYPE_Release")

# OpenViBE-specific helper functions that are used by the CMakeLists of the subprojects.
INCLUDE("OvAddProjects")
INCLUDE("OvInstallLaunchScript")
INCLUDE("OvLinkBoostLib")
INCLUDE("OvDelayLoad")
INCLUDE("SetProjectVersion")

# Setup knowledge of GNU-style install path variables
INCLUDE("GNUInstallDirs")
SET(CMAKE_INSTALL_FULL_DOCDIR "${CMAKE_INSTALL_FULL_BINDIR}/../doc/html")

# Add directories that OpenViBE will use to look for its components runtime, unless overridden by environment variables in the launch scripts.
# These defines should only be used in "openvibe/ov_directories.h".
ADD_DEFINITIONS(-DOV_CMAKE_PATH_ROOT="${CMAKE_INSTALL_PREFIX}")
ADD_DEFINITIONS(-DOV_CMAKE_PATH_BIN="${CMAKE_INSTALL_FULL_BINDIR}")
ADD_DEFINITIONS(-DOV_CMAKE_PATH_LIB="${CMAKE_INSTALL_FULL_LIBDIR}")
ADD_DEFINITIONS(-DOV_CMAKE_PATH_DATA="${CMAKE_INSTALL_FULL_DATADIR}/openvibe")

# Sets the PROJECT_VERSION variable to something, depending on overriding OvSetProjectVersion.cmake files
OV_SET_PROJECT_VERSION()

SET(DIST_ROOT ${CMAKE_INSTALL_PREFIX})
IF(OV_PACKAGE)
	SET(DIST_BINDIR ${CMAKE_INSTALL_BINDIR})
	SET(DIST_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	SET(DIST_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
	SET(DIST_DATADIR ${CMAKE_INSTALL_DATADIR})
	SET(DIST_DOCDIR ${CMAKE_INSTALL_BINDIR}/../doc)
	SET(CPACK_GENERATOR "ZIP")
	SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
	SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CertiViBE")
	SET(CPACK_PACKAGE_VENDOR "Inria & Mensia Technologies")
	SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.md")
	SET(CPACK_PACKAGE_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
	SET(CPACK_PACKAGE_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
	SET(CPACK_PACKAGE_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})

	if(UNIX)
		execute_process(
			COMMAND bash -c "echo -n $(lsb_release -i -s)$(lsb_release -r -s)"
			OUTPUT_VARIABLE CPACK_SYSTEM_NAME)
	endif()

	SET(CPACK_PACKAGE_NAME "OpenViBE-${CMAKE_BUILD_TYPE}")
	INCLUDE(CPack)
ELSE()
	SET(DIST_BINDIR ${CMAKE_INSTALL_FULL_BINDIR})
	SET(DIST_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR})
	SET(DIST_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
	SET(DIST_DATADIR ${CMAKE_INSTALL_FULL_DATADIR})
	SET(DIST_DOCDIR ${CMAKE_INSTALL_FULL_BINDIR}/../doc)
ENDIF()

# Traverse these directories and build their components
ADD_SUBDIRECTORY("build-tool/")

ADD_SUBDIRECTORY("documentation/")
ADD_SUBDIRECTORY("common/")

IF(NOT(SKIP_OPENVIBE))
    ADD_SUBDIRECTORY("openvibe/")
ENDIF()

IF(NOT(SKIP_KERNEL))
    ADD_SUBDIRECTORY("kernel/")
ENDIF()

ADD_SUBDIRECTORY("modules/")

IF(NOT(SKIP_TOOLKIT))
    ADD_SUBDIRECTORY("toolkit/")
ENDIF()

IF(NOT(SKIP_PLUGINS))
    ADD_SUBDIRECTORY("plugins/")
ENDIF()

IF(NOT(SKIP_APPLICATIONS))
    ADD_SUBDIRECTORY("applications/")
ENDIF()


OPTION(BUILD_TESTING "Include and build unit tests" ON)

IF(BUILD_TESTING)
	SET(OVT_LOG_DIR "${CMAKE_BINARY_DIR}/test/log" CACHE PATH "Path to directory containing test logs")
	SET(OVT_TEST_DATA_DIR "${CMAKE_SOURCE_DIR}/dependencies/test-input" CACHE PATH "Path to test input data files")
ENDIF()

# make a copy of the dependency script, this is done so we can use the same launch scripts both in win install and win devel situations.
#IF(WIN32)
#	SET(ABSOLUTE_PATH_DEPENDENCIES ${OV_CUSTOM_DEPENDENCIES_PATH})
#	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/scripts/windows-set-environment.cmd-base "${DIST_BINDIR}/openvibe-set-env.cmd")
#ENDIF(WIN32)

# add the scripts to the project so IDEs using the CMake file are aware of them
FILE(GLOB_RECURSE script_files scripts/*.cmd scripts/*.sh scripts/*.nsi scripts/*.cmake)
INSTALL(FILES scripts/AddCertivibeComponents.cmake DESTINATION ${DIST_DATADIR})
ADD_CUSTOM_TARGET(openvibe-scripts SOURCES ${script_files})

# Enable/Disable unit test
SET(BUILD_UNIT_TEST true CACHE BOOL "Set this flag to true/false to enable/disable unit test building")
SET(BUILD_VALIDATION_TEST true CACHE BOOL "Set this flag to true/false to enable/disable validation test building")

IF(BUILD_UNIT_TEST) 
	ADD_SUBDIRECTORY("unit-test/")
ENDIF()
IF(BUILD_VALIDATION_TEST) 
	ADD_SUBDIRECTORY("validation-test/")
ENDIF()

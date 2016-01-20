CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
cmake_policy(SET CMP0026 OLD)
cmake_policy(SET CMP0042 OLD)
cmake_policy(SET CMP0043 OLD)
cmake_policy(SET CMP0045 OLD)
cmake_policy(SET CMP0048 OLD)

PROJECT(OpenVIBE)

# These versions are used by the subprojects by default.
# If you wish to maintain specific version numbers for a subproject, please do so in the projects CMakeLists.txt
SET(MENSIA_GLOBAL_VERSION_MAJOR 2)
SET(MENSIA_GLOBAL_VERSION_MINOR 4)
SET(MENSIA_GLOBAL_VERSION_TWEAK 0)
SET(MENSIA_GLOBAL_VERSION_PATCH 1)
SET(MENSIA_GLOBAL_VERSION_STRING "${MENSIA_GLOBAL_VERSION_MAJOR}.${MENSIA_GLOBAL_VERSION_MINOR}.${MENSIA_GLOBAL_VERSION_TWEAK}.${MENSIA_GLOBAL_VERSION_PATCH}")

SET(OV_GLOBAL_VERSION_MAJOR 0)
SET(OV_GLOBAL_VERSION_MINOR 17)
SET(OV_GLOBAL_VERSION_PATCH 0)
SET(OV_GLOBAL_VERSION_STRING "${OV_GLOBAL_VERSION_MAJOR}.${OV_GLOBAL_VERSION_MINOR}.${OV_GLOBAL_VERSION_PATCH}")

###################################################################################
# We check the local version of the dependencies
# The installer should install local manifest, if a mismatch is detected CMake will cancel the build
# and ask to reinstall the missing/update dependencies
MESSAGE(STATUS "Checking local dependencies manifest...")

SET(OV_CUSTOM_DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/dependencies")
SET(OV_SOURCE_DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/dependencies-source")
SET(OV_CUSTOM_THIRD_PARTIES_DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/dependencies/third-parties-dependencies")
SET(MENSIA_DEPENDENCIES "${CMAKE_SOURCE_DIR}/externals/mensia/dependencies")
SET(OV_ICON_PATH "${CMAKE_SOURCE_DIR}/scripts/components/icons")

# update this section when updating the dependencies.
SET(DEPENDENCY_COUNT 27)
SET(DEPENDENCY_BOOST_VERSION        "1.55")
SET(DEPENDENCY_DOC_VERSION          "1.0.0")
SET(DEPENDENCY_CMAKE_VERSION        "2.8.12.1")
SET(DEPENDENCY_CURL_VERSION         "7.19.3")
SET(DEPENDENCY_EIGEN_VERSION        "3.2.0")
SET(DEPENDENCY_ELECTRON_VERSION     "0.29.2")
SET(DEPENDENCY_EXPAT_VERSION        "2.0.1")
SET(DEPENDENCY_FREEALUT_VERSION     "1.1")
SET(DEPENDENCY_GTK_VERSION          "2.22.1")
SET(DEPENDENCY_LIBOGG_VERSION       "1.2.1")
SET(DEPENDENCY_LIBVORBIS_VERSION    "1.3.2")
SET(DEPENDENCY_LOVE2D_VERSION       "0.9.1")
SET(DEPENDENCY_LUA_VERSION          "5.1.4")
SET(DEPENDENCY_LIBMYSQL_VERSION     "10.0.13")
SET(DEPENDENCY_NINJA_VERSION        "1.4.0")
SET(DEPENDENCY_NSIS_VERSION         "3.0b1")
SET(DEPENDENCY_OPENAL_VERSION       "1.1")
SET(DEPENDENCY_PLINK_VERSION        "0.62.0.0")
SET(DEPENDENCY_PORTQRY_VERSION      "1.0")
SET(DEPENDENCY_PTHREADS_VERSION     "2.8.0")
SET(DEPENDENCY_PYTHON_VERSION       "3.4.1")
SET(DEPENDENCY_SFML_VERSION         "2.1")
SET(DEPENDENCY_SFML.NET_VERSION     "2.1")
SET(DEPENDENCY_THIRDPARTIES_VERSION "2.5")
SET(DEPENDENCY_TVICPORT_VERSION     "5.0.2.1")
SET(DEPENDENCY_VRPN_VERSION         "7.26")
SET(DEPENDENCY_ZIP_VERSION          "3.0")

IF(WIN32)
  IF(NOT EXISTS "${OV_CUSTOM_DEPENDENCIES_PATH}/manifest.txt")
    MESSAGE(WARNING "Your local dependency folder do not have any manifest. Please run the Mensia Dependencies installer, and retry a new build from scratch (cleanup and rebuild)")
  ELSE()
    SET(LOCAL_DEPENDENCY_COUNT 0)

    # the manifest is a list of NAME=VERSION, 1 per line
    FILE(READ "${OV_CUSTOM_DEPENDENCIES_PATH}/manifest.txt" DEPENDENCIES_LOCAL_VERSION)
    # Convert file contents into a CMake list (where each element in the list
    # is one line of the file)
    STRING(REGEX REPLACE ";" "\\\\;" DEPENDENCIES_LOCAL_VERSION "${DEPENDENCIES_LOCAL_VERSION}")
    STRING(REGEX REPLACE "\n" ";" DEPENDENCIES_LOCAL_VERSION "${DEPENDENCIES_LOCAL_VERSION}")

    SET(DEPENDENCY_CHECK 1)
    FOREACH(dependency IN LISTS DEPENDENCIES_LOCAL_VERSION)
      STRING(FIND "${dependency}" "=" EQU_POS)
      IF(NOT ${EQU_POS} EQUAL -1)
        MATH(EXPR VER_POS "${EQU_POS}+1")
        STRING(SUBSTRING "${dependency}" 0 ${EQU_POS} DEP_NAME)
        STRING(SUBSTRING "${dependency}" ${VER_POS} -1 DEP_VERSION)
        IF(NOT ${DEPENDENCY_${DEP_NAME}_VERSION} STREQUAL "")
          MATH(EXPR LOCAL_DEPENDENCY_COUNT "${LOCAL_DEPENDENCY_COUNT}+1")
          IF(${DEP_VERSION} STREQUAL ${DEPENDENCY_${DEP_NAME}_VERSION})
            MESSAGE(STATUS "Local Dependency ${DEP_NAME} has version ${DEP_VERSION} -- OK")
          ELSE()
            MESSAGE(WARNING "Local Dependency ${DEP_NAME} has version ${DEP_VERSION} -- MISMATCH (expected ${DEPENDENCY_${DEP_NAME}_VERSION})!")
            SET(DEPENDENCY_CHECK 0)
          ENDIF()
        ELSE()
          MESSAGE(WARNING "Local Dependency ${DEP_NAME} with version ${DEP_VERSION} was not expected and probably won't be used !")
          # we do not fail for that
        ENDIF()
      ENDIF()
    ENDFOREACH()

    IF(NOT ${LOCAL_DEPENDENCY_COUNT} EQUAL ${DEPENDENCY_COUNT})
      MESSAGE(WARNING "At least one dependency is missing. Please run the Mensia Dependency installer before retrying.")
    ENDIF()

    IF(NOT ${DEPENDENCY_CHECK})
      MESSAGE(WARNING "At least one dependency is out-of-date. Please run the Mensia Dependency installer before retrying. ")
    ELSE()
      MESSAGE(STATUS "All dependencies are up-to-date. Configuration continues...")
    ENDIF()

  ENDIF()
ENDIF()

###################################################################################

SET(OEM_DISTRIBUTION "mensia")
SET(INSTALL_EXE ON)

# The user files will be stored in .../mensia/OV_PROJECT_NAME folder
ADD_DEFINITIONS("-DOV_PROJECT_NAME=\"openvibe\"")

# Default is to build to dist/. If you wish a custom install, set your own MAKE_INSTALL_PREFIX when you call CMake. Safest to do under a fakeroot.
MESSAGE(STATUS "Appending compilation flags...")

IF(WIN32)
	ADD_DEFINITIONS("-DNOMINMAX -DBOOST_ALL_NO_LIB")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4355")
	SET(OV_WIN32_BOOST_VERSION "1_55")
ELSEIF(APPLE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
ELSEIF(UNIX)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")
	# This ENV var is needed to locate our custom IT++ on Linux
	SET(ENV{PKG_CONFIG_PATH} "${OV_CUSTOM_DEPENDENCIES_PATH}/lib/pkgconfig")
ELSE(WIN32)
	MESSAGE(WARNING "Warning: unknown platform")
ENDIF(WIN32)

IF(NOT CMAKE_BUILD_TYPE)
	MESSAGE(STATUS "Setting default build type to Release")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(NOT CMAKE_INSTALL_PREFIX)
	SET(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../certivibe-build/dist-${CMAKE_BUILD_TYPE}")
ENDIF(NOT CMAKE_INSTALL_PREFIX)

SET(DOCUMENTATION_TEMP_DIRECTORY "${CMAKE_INSTALL_PREFIX}/doc-tmp")


# Print the used compilation parameters (for transparency)
GET_DIRECTORY_PROPERTY(TMP_DEFINITIONS COMPILE_DEFINITIONS)
MESSAGE(STATUS "Compilation flags used at source root: ")
MESSAGE(STATUS "  COMPILE_DEFINITIONS = '${TMP_DEFINITIONS}'")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS = '${CMAKE_CXX_FLAGS}'")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS_RELEASE = '${CMAKE_CXX_FLAGS_RELEASE}'")
MESSAGE(STATUS "  CMAKE_CXX_FLAGS_DEBUG = '${CMAKE_CXX_FLAGS_DEBUG}'")

# if no specific branch has been given for an openvibe component, default is to compile its trunk folder
## SET(OV_TRUNK "trunc")

# SET(OV_COMPILE_TESTS "true")
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

# By setting SKIP[_FOLDER]* you can skip a subtree (example: SKIP_A_B_C skips folder a/b/c and all its subfolders if any)

# By default we skip some builds we rarely use to speed up the compilation

# Compile lua code in the Love Demos using luac on windows
SET(MENSIA_LOVE_DEMOS_LUAC_COMPILATION ON)

# Custom cmakelist can be used to overwrite the default compilation & packaging parameters
# e.g. OEM_DISTRIBUTION
IF(EXISTS "${CMAKE_SOURCE_DIR}/CustomCMakeLists.txt")
	MESSAGE(STATUS "Found custom build settings")
	INCLUDE("${CMAKE_SOURCE_DIR}/CustomCMakeLists.txt")
ENDIF()

#################################################################
SET(BRAND_NAME "CertiViBE")
string(TOLOWER ${BRAND_NAME} LOWER_BRAND_NAME)
ADD_DEFINITIONS(-DBRAND_NAME="${BRAND_NAME}")
#################################################################

SET(OV_LAUNCHER_SOURCE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules/launchers")
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/mensia/cmake-modules;${CMAKE_SOURCE_DIR}/cmake-modules")

# If you wish you can uncomment the next line and add a file with additional information called OVBranches.cmake to
# the cmake-modules folder. It makes modifying this very CMake file easier.
#INCLUDE("OVCustomSettings")

# a list of all project which will be filled by the directory traversal. This is needed to generate the documentation.
SET_PROPERTY(GLOBAL PROPERTY OV_PROP_CURRENT_PROJECTS "")
SET_PROPERTY(GLOBAL PROPERTY OV_PROP_CURRENT_PROJECTS_BUILD_DIR "")

# Used by the various Find* scripts to locate OpenViBE modules
SET(OV_BASE_DIR ${CMAKE_SOURCE_DIR})

# needed for making visual studio projects when this script is called without CMAKE_BUILD_TYPE
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG "TARGET_BUILDTYPE_Debug")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE "TARGET_BUILDTYPE_Release")
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO "TARGET_BUILDTYPE_Release")

# OpenViBE-specific helper functions that are used by the CMakeLists of the subprojects.
#INCLUDE("OvAddSingleProject")
INCLUDE("OvAddProjects")
INCLUDE("OvInstallLaunchScript")
INCLUDE("OvLinkBoostLib")
INCLUDE("OvDelayLoad")
INCLUDE("SetProjectVersion")


# Setup knowledge of GNU-style install path variables
INCLUDE("GNUInstallDirs")
SET(CMAKE_INSTALL_FULL_DOCDIR "${CMAKE_INSTALL_FULL_BINDIR}/../doc/html")
SET(CMAKE_INSTALL_FULL_WEBDIR "${CMAKE_INSTALL_FULL_BINDIR}/../web-applications")

# Add directories that OpenViBE will use to look for its components runtime, unless overridden by environment variables in the launch scripts.
# These defines should only be used in "openvibe/ov_directories.h".
ADD_DEFINITIONS(-DOV_CMAKE_PATH_ROOT="${CMAKE_INSTALL_PREFIX}")
ADD_DEFINITIONS(-DOV_CMAKE_PATH_BIN="${CMAKE_INSTALL_FULL_BINDIR}")
ADD_DEFINITIONS(-DOV_CMAKE_PATH_LIB="${CMAKE_INSTALL_FULL_LIBDIR}")
ADD_DEFINITIONS(-DOV_CMAKE_PATH_DATA="${CMAKE_INSTALL_FULL_DATADIR}/openvibe")

# Add Mensia definitions
SET(MENSIA_BASE_DIR "${CMAKE_SOURCE_DIR}/externals/mensia")
ADD_DEFINITIONS(-DMENSIA_CMAKE_PATH_ROOT="${CMAKE_INSTALL_PREFIX}")
ADD_DEFINITIONS(-DMENSIA_CMAKE_PATH_BIN="${CMAKE_INSTALL_FULL_BINDIR}")
ADD_DEFINITIONS(-DMENSIA_CMAKE_PATH_DATA="${CMAKE_INSTALL_FULL_DATADIR}/mensia")
ADD_DEFINITIONS(-DMENSIA_CMAKE_PATH_LIB="${CMAKE_INSTALL_FULL_LIBDIR}")

# Sets the PROJECT_VERSION variable to something, depending on overriding OvSetProjectVersion.cmake files
OV_SET_PROJECT_VERSION()

MESSAGE(STATUS "BUILDING VERSION : ${PROJECT_VERSION} OpenViBE : ${OV_PROJECT_BRANCH}~${OV_PROJECT_COMMITHASH} Mensia : ${PROJECT_BRANCH}~${PROJECT_COMMITHASH}")

# Add vendor specific installation instructions
# Traverse these directories and build their components
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

IF(NOT(SKIP_EXTERNALS))
	ADD_SUBDIRECTORY("externals/")
ENDIF()

# make a copy of the dependency script, this is done so we can use the same launch scripts both in win install and win devel situations.
IF(WIN32)
MESSAGE("STUFF")
	SET(ABSOLUTE_PATH_DEPENDENCIES ${OV_CUSTOM_DEPENDENCIES_PATH})
	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/scripts/windows-set-environment.cmd-base "${CMAKE_INSTALL_FULL_BINDIR}/openvibe-set-env.cmd")
ENDIF(WIN32)


OV_INSTALL_LAUNCH_SCRIPT(SCRIPT_PREFIX "neurort-set-dev-env" EXECUTABLE_NAME "cmd" PAUSE)

# ----------------------
# Generate 'external application launcher' script
# ----------------------
IF(WIN32)
	SET(OV_EXT_CMD_FILE "neurort-external-application-launcher.cmd")
ENDIF(WIN32)
IF(UNIX)
	SET(OV_EXT_CMD_FILE "neurort-external-application-launcher")
ENDIF(UNIX)

CONFIGURE_FILE("${OV_LAUNCHER_SOURCE_PATH}/${OV_EXT_CMD_FILE}-base" "${CMAKE_CURRENT_BINARY_DIR}/${OV_EXT_CMD_FILE}")
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${OV_EXT_CMD_FILE} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

# add the scripts to the project so IDEs using the CMake file are aware of them
FILE(GLOB_RECURSE script_files scripts/*.cmd scripts/*.sh scripts/*.nsi)
ADD_CUSTOM_TARGET(openvibe-scripts SOURCES ${script_files})


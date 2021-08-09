project(OpenVIBE_SDK VERSION ${OPENVIBE_MAJOR_VERSION}.${OPENVIBE_MINOR_VERSION}.${OPENVIBE_PATCH_VERSION})
# Add cmake-modules to path
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH_BASE}
	${CMAKE_SOURCE_DIR}/sdk/cmake-modules
	# ${CMAKE_MODULE_PATH}
	)
include("SetProjectVersion")

set_version()
OV_SET_PROJECT_VERSION()
message(STATUS "BUILDING VERSION OpenViBE SDK: ${OV_GLOBAL_VERSION_STRING} Branch : ${OV_PROJECT_BRANCH}~${OV_PROJECT_COMMITHASH}")

# Default is to build to dist/. If you wish a custom install, set your own MAKE_INSTALL_PREFIX when you call CMake. Safest to do under a fakeroot.
message(STATUS "Appending compilation flags...")

set(OV_CONFIG_SUBDIR "${PROJECT_NAME}" CACHE STRING "Subdirectory under user directory when configuration and logs will be saved")


set(OV_SOURCE_DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/sdk/dependencies-source")

# ----------------------
# Build CMake flags
# ----------------------
set(BRAND_NAME "OpenViBE" CACHE STRING "Brand with which titles and documentation will be prefixed")
string(TIMESTAMP COPYRIGHT_DATE "2012-%Y")
if(BRAND_NAME STREQUAL "NeuroRT")
	set(COMPANY_NAME "Mensia Technologies SA")
else()
	set(COMPANY_NAME "Inria")
endif()

# Options and cahce variables

option(PUBLISH_DOC_ASSETS "Publish plugin documentation assets" ON)
option(BUILD_VALIDATION_TEST "Build the validation tests" ON)
option(BUILD_UNIT_TEST "Build the unit tests" ON)

set(CMAKE_INSTALL_LIBDIR lib)  # Enforce as all distros may not have the same default (e.g. lib, lib32/lib64)
include("GNUInstallDirs")  # Setup knowledge of GNU-style install path variables

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

if(NOT(${Flag_VerboseOutput}))
	set(CMAKE_INSTALL_MESSAGE LAZY)
endif()

# If OpenMP is available, using it will allow Eigen to use multiple cores in matrix math.
if(NOT(APPLE))
	include("FindOpenMP")
	if(OPENMP_FOUND)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	endif(OPENMP_FOUND)
endif()

add_definitions("-DOV_CONFIG_SUBDIR=\"${OV_CONFIG_SUBDIR}\"")
add_definitions("-DOV_PROJECT_NAME=\"${PROJECT_NAME}\"")
add_definitions("-DOV_VERSION_MAJOR=\"${OV_GLOBAL_VERSION_MAJOR}\"")
add_definitions("-DOV_VERSION_MINOR=\"${OV_GLOBAL_VERSION_MINOR}\"")
add_definitions("-DOV_VERSION_PATCH=\"${OV_GLOBAL_VERSION_PATCH}\"")

if(WIN32)
	add_definitions("-DNOMINMAX -DBOOST_ALL_NO_LIB")
	# The following is needed to keep OpenMP from hogging all the cores. Note that this works only for VS2013+. On VS2010, it may be better to disable OpenMP.
	set(OV_OMP_WAIT_POLICY "PASSIVE")
	# Disable /MP if you don't like VS using all the cores for compilation
	
	# Switch /arch:SSE2 enables vectorization. Remove if your CPU/compiler doesn't support it.
	# x64 build do not provide SSE2 option because 64bit processors always supports SSE vectorization.
	# So the /arch:SSE2 is not recognized for x64 build we have to only activate it for x86.
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		set(OV_EIGEN_FLAGS "/arch:SSE2")
	endif()
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4355 /MP ${OV_EIGEN_FLAGS}")
	set(OV_WIN32_BOOST_VERSION "1_58")
elseif(UNIX)
	# Switch -msse2 enables vectorization. Remove if your CPU/compiler doesn't support it.
	set(OV_EIGEN_FLAGS "-msse2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall ${OV_EIGEN_FLAGS}")
else()
	message(WARNING "Warning: unknown platform")
endif()


get_property (GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(${GENERATOR_IS_MULTI_CONFIG})
	set(MULTI_BUILD TRUE)
	set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin" )
	set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/lib" )
	# set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/archive" )
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
		# set_target_properties( ${target} PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIGU} "${CMAKE_INSTALL_PREFIX}/${OUTPUTCONFIG}/bin")
		set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIGU} "${CMAKE_INSTALL_PREFIX}/${OUTPUTCONFIG}/bin" )
		set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIGU} "${CMAKE_INSTALL_PREFIX}/${OUTPUTCONFIG}/lib" )
		# set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_INSTALL_PREFIX}/${OUTPUTCONFIG}/archive" )
		string(CONCAT DIST_ROOT ${DIST_ROOT} $<$<CONFIG:${OUTPUTCONFIGU}>:${CMAKE_INSTALL_PREFIX}/${OUTPUTCONFIG}>)
	endforeach()
	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "OV_CMAKE_PATH_ROOT=\"${DIST_ROOT}\"")
	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "OV_CMAKE_PATH_BIN=\"${DIST_ROOT}/bin\"")
	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "OV_CMAKE_PATH_LIB=\"${DIST_ROOT}/lib\"")
	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS "OV_CMAKE_PATH_DATA=\"${DIST_ROOT}/share/openvibe\"")
	set(DIST_BINDIR ${DIST_ROOT}/bin)
	set(DIST_LIBDIR ${DIST_ROOT}/lib)
	set(DIST_INCLUDEDIR ${DIST_ROOT}/include)
	set(DIST_DATADIR ${DIST_ROOT}/share)
	set(DIST_DOCDIR ${DIST_ROOT}/doc)
	set(DIST_DOCTMP ${DIST_ROOT}/doc-tmp)
	
	set(DIST_BINDIR_ABSOLUTE ${DIST_BINDIR})
	# Also install it on the base dir for convenience
	
	# Install the .install file inside the dists folders
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/.install DESTINATION ${DIST_ROOT})
elseif(CMAKE_BUILD_TYPE AND OV_PACKAGE)
	# Add directories that OpenViBE will use to look for its components runtime, unless overridden by environment variables in the launch scripts.
	# These defines should only be used in "openvibe/ov_directories.h".

	# We use absolute paths to avoid issue for unit and validation tests
	get_filename_component(DIST_ROOT_ABSOLUTE "." REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
	set(DIST_BINDIR_ABSOLUTE ${DIST_ROOT_ABSOLUTE}/bin)

	set(CMAKE_INSTALL_PREFIX .)

	add_definitions(-DOV_CMAKE_PATH_ROOT="${DIST_ROOT_ABSOLUTE}")
	add_definitions(-DOV_CMAKE_PATH_BIN="${DIST_ROOT_ABSOLUTE}/bin")
	add_definitions(-DOV_CMAKE_PATH_LIB="${DIST_ROOT_ABSOLUTE}/lib")
	add_definitions(-DOV_CMAKE_PATH_DATA="${DIST_ROOT_ABSOLUTE}/share/openvibe")

	set(DIST_ROOT ${CMAKE_INSTALL_PREFIX})
	set(DIST_BINDIR ${CMAKE_INSTALL_BINDIR})
	set(DIST_LIBDIR ${CMAKE_INSTALL_LIBDIR})
	set(DIST_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
	set(DIST_DATADIR ${CMAKE_INSTALL_DATADIR})
	set(DIST_DOCDIR doc)
	set(DIST_DOCTMP doc-tmp)
	set(CPACK_GENERATOR "ZIP")
	set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${BRAND_NAME})
	set(CPACK_PACKAGE_VENDOR "Inria & Mensia Technologies")
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.md")
	set(CPACK_PACKAGE_VERSION ${OV_GLOBAL_VERSION_MAJOR}.${OV_GLOBAL_VERSION_MINOR}.${OV_GLOBAL_VERSION_PATCH}.${OV_GLOBAL_VERSION_BUILD})
	set(CPACK_PACKAGE_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})

	if(UNIX)
		execute_process(
			COMMAND bash -c "echo -n $(lsb_release -i -s)$(lsb_release -r -s)"
			OUTPUT_VARIABLE CPACK_SYSTEM_NAME)
	endif()

	set(CPACK_PACKAGE_NAME "OpenViBE-${CMAKE_BUILD_TYPE}")
	include(CPack)
elseif(CMAKE_BUILD_TYPE)
	# Add directories that OpenViBE will use to look for its components runtime, unless overridden by environment variables in the launch scripts.
	# These defines should only be used in "openvibe/ov_directories.h".
	add_definitions(-DOV_CMAKE_PATH_ROOT="${CMAKE_INSTALL_PREFIX}")
	add_definitions(-DOV_CMAKE_PATH_BIN="${CMAKE_INSTALL_PREFIX}/bin")
	add_definitions(-DOV_CMAKE_PATH_LIB="${CMAKE_INSTALL_PREFIX}/lib")
	add_definitions(-DOV_CMAKE_PATH_DATA="${CMAKE_INSTALL_PREFIX}/share/openvibe")
	
	set(DIST_ROOT ${CMAKE_INSTALL_PREFIX})
	set(DIST_BINDIR ${CMAKE_INSTALL_FULL_BINDIR})
	set(DIST_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR})
	set(DIST_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
	set(DIST_DATADIR ${CMAKE_INSTALL_FULL_DATADIR})
	set(DIST_DOCDIR ${DIST_ROOT}/doc)
	set(DIST_DOCTMP ${DIST_ROOT}/doc-tmp)
	
	set(DIST_BINDIR_ABSOLUTE ${DIST_BINDIR})
else()
	message(FATAL_ERROR "Build should specify a type or use a multi-type generator (like Visual Studio)")
endif()

# Print the used compilation parameters (for transparency)
get_directory_property(TMP_DEFINITIONS COMPILE_DEFINITIONS)
message(STATUS "Compilation flags used at source root: ")
message(STATUS "  COMPILE_DEFINITIONS = '${TMP_DEFINITIONS}'")
message(STATUS "  CMAKE_CXX_FLAGS = '${CMAKE_CXX_FLAGS}'")
message(STATUS "  CMAKE_CXX_FLAGS_RELEASE = '${CMAKE_CXX_FLAGS_RELEASE}'")
message(STATUS "  CMAKE_CXX_FLAGS_DEBUG = '${CMAKE_CXX_FLAGS_DEBUG}'")

function(SET_BUILD_PLATFORM)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		add_definitions(-DTARGET_ARCHITECTURE_x64)
	elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		add_definitions(-DTARGET_ARCHITECTURE_i386)
		SET(PLATFORM_TARGET "x86")
	else()
		add_definitions(-DTARGET_ARCHITECTURE_Unknown)
	endif()

	if(WIN32)
		add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
		add_definitions(-DTARGET_OS_Windows)
		add_definitions(-DTARGET_COMPILER_VisualStudio)
	elseif(APPLE)
		add_definitions(-fnon-call-exceptions)
		add_definitions(-DTARGET_OS_MacOS)
		# ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_x64)
		add_definitions(-DTARGET_COMPILER_LLVM)
	elseif(UNIX)
		# ADD_DEFINITIONS(-fvisibility=hidden) # This flag should be present... man gcc
		add_definitions(-fnon-call-exceptions)
		add_definitions(-DTARGET_OS_Linux)
		add_definitions(-DTARGET_COMPILER_GCC)
	endif()
endfunction()

# Custom cmakelist can be used to overwrite the default compilation & packaging parameters
# e.g. OEM_DISTRIBUTION
if(EXISTS "${CMAKE_SOURCE_DIR}/sdk/CustomCMakeLists.txt")
	message(STATUS "Found custom build settings")
	include("${CMAKE_SOURCE_DIR}/sdk/CustomCMakeLists.txt")
endif()

#################################################################
string(TOLOWER ${BRAND_NAME} LOWER_BRAND_NAME)
add_definitions(-DBRAND_NAME="${BRAND_NAME}")
if(${BRAND_NAME} STREQUAL "NeuroRT")
	add_definitions(-DOV_LOCAL_SYMBOLS)
endif()

if(OV_DISPLAY_ERROR_LOCATION)
	add_definitions(-DOV_DISPLAY_ERROR_LOCATION)
endif()
#################################################################

set(OV_LAUNCHER_SOURCE_PATH "${CMAKE_SOURCE_DIR}/sdk/cmake-modules/launchers")

# By setting SKIP[_FOLDER]* you can skip a subtree (example: SKIP_A_B_C skips folder a/b/c and all its subfolders if any)
# Skip building documentation
# a list of all project which will be filled by the directory traversal. This is needed to generate the documentation.
set_property(GLOBAL PROPERTY OV_PROP_CURRENT_PROJECTS "")
set_property(GLOBAL PROPERTY OV_PROP_CURRENT_PROJECTS_BUILD_DIR "")

# Used by the various Find* scripts to locate OpenViBE modules
set(OV_BASE_DIR ${CMAKE_SOURCE_DIR}/sdk)
set(OV_BASE_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(OV_LIBRARY_DIR  ${CMAKE_CURRENT_BINARY_DIR}/lib)

# needed for making visual studio projects when this script is called without CMAKE_BUILD_TYPE
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG "TARGET_BUILDTYPE_Debug")
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE "TARGET_BUILDTYPE_Release")
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELWITHDEBINFO "TARGET_BUILDTYPE_Release")

# OpenViBE-specific helper functions that are used by the CMakeLists of the subprojects.
include("OvAddProjects")
include("OvLinkBoostLib")
include("OvDelayLoad")

# Sort target into directories for better visualization in IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(APP_FOLDER Applications)
set(KERNEL_FOLDER Kernel)
set(MODULES_FOLDER Modules)
set(PLUGINS_FOLDER Plugins)
set(TESTS_FOLDER Unit-Tests)
set(VALIDATION_FOLDER Validation-Tests)

# Traverse these directories and build their components
add_subdirectory("build-tool/")

add_subdirectory("documentation/")
add_subdirectory("common/")

if(NOT(SKIP_OPENVIBE))
    add_subdirectory("openvibe/")
endif()

if(NOT(SKIP_KERNEL))
    add_subdirectory("kernel/")
endif()

add_subdirectory("modules/")

if(NOT(SKIP_TOOLKIT))
    add_subdirectory("toolkit/")
endif()

if(NOT(SKIP_PLUGINS))
    add_subdirectory("plugins/")
endif()

if(NOT(SKIP_APPLICATIONS))
    add_subdirectory("applications/")
endif()

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/.install" DESTINATION ${CMAKE_INSTALL_PREFIX})


option(BUILD_TESTING "Include and build unit tests" ON)

if(BUILD_TESTING)
	set(OVT_LOG_DIR "${CMAKE_CURRENT_BINARY_DIR}/test/log" CACHE PATH "Path to directory containing test logs")
	set(OVT_TEST_DATA_DIR "${OV_CUSTOM_DEPENDENCIES_PATH}/test-input" CACHE PATH "Path to test input data files")
endif()

# add the scripts to the project so IDEs using the CMake file are aware of them
file(GLOB_RECURSE script_files scripts/*.cmd scripts/*.sh scripts/*.nsi scripts/*.cmake)
add_custom_target(openvibe-scripts SOURCES ${script_files})

if(BUILD_UNIT_TEST) 
	add_subdirectory("unit-test/")
endif()
if(BUILD_VALIDATION_TEST) 
	add_subdirectory("validation-test/")
endif()

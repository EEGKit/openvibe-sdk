PROJECT(openvibe-module-system)

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

<<<<<<< HEAD
FILE(GLOB_RECURSE source_files src/*.cpp src/*.h src/*.hpp src/*.inl include/*.h)
INCLUDE_DIRECTORIES(include)
INCLUDE("FindSourceRCProperties")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${source_files})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DSystem_Shared -DSystem_Exports")

ADD_LIBRARY(${PROJECT_NAME}-static STATIC ${source_files})
SET_TARGET_PROPERTIES(${PROJECT_NAME}-static PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DSystem_Static -DSystem_Exports")

IF(UNIX)
	SET_TARGET_PROPERTIES(${PROJECT_NAME}-static PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(UNIX)

INCLUDE("FindOpenViBECommon")

INCLUDE("FindThirdPartyBoost")
=======
FILE(GLOB_RECURSE source_files src/*.cpp src/*.h src/*.inl include/*.h)
INCLUDE_DIRECTORIES(include/system)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${source_files})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DOV_Shared -DOV_Exports")

INCLUDE("FindOpenViBECommon")
INCLUDE("FindThirdPartyBoost")
INCLUDE("FindThirdPartyBoost_Chrono")
>>>>>>> openvibe-starting-point
INCLUDE("FindThirdPartyBoost_System")
INCLUDE("FindThirdPartyBoost_Thread")

# ---------------------------------
# Finds standard library winmm
# Adds library to target
# Adds include path
# ---------------------------------
IF(WIN32)
	INCLUDE("OvSetWindowsSDKPath")
	FIND_LIBRARY(LIB_STANDARD_MODULE_WINMM winmm ${OV_MS_SDK_PATH}/lib)
	IF(LIB_STANDARD_MODULE_WINMM)
		MESSAGE(STATUS "  Found winmm...")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIB_STANDARD_MODULE_WINMM})
<<<<<<< HEAD
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-static ${LIB_STANDARD_MODULE_WINMM})
=======
>>>>>>> openvibe-starting-point
	ELSE(LIB_STANDARD_MODULE_WINMM)
		MESSAGE(STATUS "  FAILED to find winmm...")
	ENDIF(LIB_STANDARD_MODULE_WINMM)
ENDIF(WIN32)

# ---------------------------------
# Test applications
# ---------------------------------
OPTION(BUILD_OPENVIBE_MODULE_System_EXAMPLES "Build OpenViBE module System examples" OFF)
<<<<<<< HEAD
IF(BUILD_OPENVIBE_MODULE_System_EXAMPLES)
	SUBDIRS(test)
ENDIF(BUILD_OPENVIBE_MODULE_System_EXAMPLES)
=======
IF(OV_COMPILE_TESTS)
	SUBDIRS(test)
ENDIF(OV_COMPILE_TESTS)
>>>>>>> openvibe-starting-point

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------
<<<<<<< HEAD
SET_BUILD_PLATFORM()

=======
IF(WIN32)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
	ADD_DEFINITIONS(-DTARGET_OS_Windows)
	ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_i386)
	ADD_DEFINITIONS(-DTARGET_COMPILER_VisualStudio)
ENDIF(WIN32)
IF(UNIX)
	# ADD_DEFINITIONS(-fvisibility=hidden) # This flag should be present... man gcc
	ADD_DEFINITIONS(-g)
	ADD_DEFINITIONS(-fnon-call-exceptions)
	ADD_DEFINITIONS(-DTARGET_OS_Linux)
	ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_i386)
	ADD_DEFINITIONS(-DTARGET_COMPILER_GCC)
ENDIF(UNIX)
>>>>>>> openvibe-starting-point

# -----------------------------
# Install files
# -----------------------------
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

<<<<<<< HEAD
INSTALL(TARGETS ${PROJECT_NAME}-static
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

=======
>>>>>>> openvibe-starting-point
INSTALL(DIRECTORY include/   DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

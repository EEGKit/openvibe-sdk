PROJECT(openvibe-module-system)

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

SET(source_files
	include/system/defines.h
	include/system/ovCChrono.h
	include/system/ovCMath.h
	include/system/ovCMemory.h
	include/system/ovCTime.h
	include/system/ovCDynamicModule.h
	src/ovCChrono.cpp
	src/ovCMath.cpp
	src/ovCMemory.cpp
	src/ovCTime.cpp
	src/ovCDynamicModule.cpp
	)

IF(WIN32)
	LIST(APPEND source_files
		"include/system/WindowsUtilities.h"
		"src/WindowsUtilities.cpp"
		)
ENDIF()

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
INCLUDE("FindThirdPartyBoost_Chrono")

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
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-static ${LIB_STANDARD_MODULE_WINMM})
	ELSE(LIB_STANDARD_MODULE_WINMM)
		MESSAGE(STATUS "  FAILED to find winmm...")
	ENDIF(LIB_STANDARD_MODULE_WINMM)

	FIND_LIBRARY(LIB_STANDARD_MODULE_DBGHELP dbghelp ${OV_MS_SDK_PATH}/lib)

	IF(LIB_STANDARD_MODULE_DBGHELP)
		MESSAGE(STATUS "  Found dbghelp...")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIB_STANDARD_MODULE_DBGHELP})
		TARGET_LINK_LIBRARIES(${PROJECT_NAME}-static ${LIB_STANDARD_MODULE_DBGHELP})
	ELSE(LIB_STANDARD_MODULE_DBGHELP)
		MESSAGE(STATUS "  FAILED to find dbghelp...")
	ENDIF(LIB_STANDARD_MODULE_DBGHELP)
ELSE(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_DL_LIBS})
ENDIF(WIN32)

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})
INSTALL(TARGETS ${PROJECT_NAME}-static
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})
INSTALL(DIRECTORY include/   DESTINATION ${DIST_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

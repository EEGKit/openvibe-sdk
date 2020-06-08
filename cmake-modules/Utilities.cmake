#==================================================
FUNCTION(DEBUG_MESSAGE)
	IF(${Flag_VerboseOutput})
		MESSAGE(STATUS "${ARGV}")
	ENDIF()
ENDFUNCTION()
#==================================================

# Set version based on git tag.
#  If current commit is tagged, use the tag as it is, and add build number based on content of .build file, written by Jenkins
#  Else use last tag major and minor number and set patch number to 99
#
# This function should remain generic to be usable in every projects.
#==================================================
FUNCTION(SET_VERSION)
	FIND_PACKAGE(Git)
	IF(EXISTS ${CMAKE_SOURCE_DIR}/.version)

		# These versions are used by the subprojects by default.
		# If you wish to maintain specific version numbers for a subproject, please do so in the projects CMakeLists.txt
		FILE(READ ${CMAKE_SOURCE_DIR}/.version PROJECT_VERSION)
		STRING(STRIP ${PROJECT_VERSION} PROJECT_VERSION)
		STRING(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
		LIST(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
		LIST(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
		LIST(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

	ELSEIF(NOT GIT_FOUND)
		MESSAGE(WARNING "Git not found, set version to 0.0.0")
		SET(PROJECT_VERSION "0.0.0")
		SET(PROJECT_BRANCH_STRING "unknown")
		SET(PROJECT_COMMITHASH_STRING "0")
	ELSE()
		DEBUG_MESSAGE("Found Git: ${GIT_EXECUTABLE}")
		EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} describe
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			OUTPUT_VARIABLE  PROJECT_VERSION
			ERROR_VARIABLE  ERROR)
		IF(ERROR)
			SET(PROJECT_VERSION "0.0.0")
			MESSAGE(WARNING "No tags found, set version to 0.0.0")
		ELSE()
			# codename = the name of the current branch
			EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				OUTPUT_VARIABLE  PROJECT_BRANCH_STRING)
			# command output may contain carriage return
			STRING(STRIP ${PROJECT_BRANCH_STRING} PROJECT_BRANCH_STRING)

			# commithash = short hash of latest revision
			EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				OUTPUT_VARIABLE  PROJECT_COMMITHASH_STRING)
			# command output may contain carriage return
			STRING(STRIP ${PROJECT_COMMITHASH_STRING} PROJECT_COMMITHASH_STRING)
		ENDIF()
	ENDIF()

	# if current commit is not tagged result is formed as: "major.minor.patch-number of commits since last tag-hash"
	STRING(STRIP ${PROJECT_VERSION} PROJECT_VERSION)
	STRING(REPLACE "-" ";" version_list ${PROJECT_VERSION})
	LIST(LENGTH version_list version_list_length)
	IF(${version_list_length} EQUAL 3) # if result is formed as "major.minor.patch-number of commits since last tag-hash" set patch as 99
		LIST(GET version_list 0 PROJECT_VERSION)
		SET(PROJECT_VERSION_PATCH 99)
	ENDIF()

	STRING(REPLACE "." ";" version_list ${PROJECT_VERSION})
	LIST(GET version_list 0 PROJECT_VERSION_MAJOR)
	LIST(GET version_list 1 PROJECT_VERSION_MINOR)
	IF(NOT PROJECT_VERSION_PATCH)
		LIST(GET version_list 2 PROJECT_VERSION_PATCH)
	ENDIF()

	# These versions are used by the subprojects by default.
	# If you wish to maintain specific version numbers for a subproject, please do so in the projects CMakeLists.txt
	IF(EXISTS ${CMAKE_SOURCE_DIR}/.build)
		FILE(READ ${CMAKE_SOURCE_DIR}/.build PROJECT_VERSION_BUILD)
		STRING(STRIP ${PROJECT_VERSION_BUILD} PROJECT_VERSION_BUILD)
	ELSE()
		SET(PROJECT_VERSION_BUILD 0)
	ENDIF()
	
	SET(PROJECT_BRANCH ${PROJECT_BRANCH_STRING} PARENT_SCOPE)
	SET(PROJECT_COMMITHASH ${PROJECT_COMMITHASH_STRING} PARENT_SCOPE)

	SET(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
	SET(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
	SET(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)
	SET(PROJECT_VERSION_BUILD ${PROJECT_VERSION_BUILD} PARENT_SCOPE)
	SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} PARENT_SCOPE)
ENDFUNCTION()
#==================================================

#==================================================
FUNCTION(SET_BUILD_PLATFORM)
	IF("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_x64)
	ELSEIF("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		ADD_DEFINITIONS(-DTARGET_ARCHITECTURE_i386)
		SET(PLATFORM_TARGET "x86")
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
#==================================================

# Set OpenViBE specific versions.
#==================================================
FUNCTION(SET_PROJECT_VERSION)
	SET(OV_GLOBAL_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
	SET(OV_GLOBAL_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
	SET(OV_GLOBAL_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)
	SET(OV_GLOBAL_VERSION_BUILD ${PROJECT_VERSION_BUILD} PARENT_SCOPE)
	SET(OV_GLOBAL_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_BUILD} PARENT_SCOPE)
	
	SET(OV_PROJECT_BRANCH ${PROJECT_BRANCH} PARENT_SCOPE)
	SET(OV_PROJECT_COMMITHASH ${PROJECT_COMMITHASH} PARENT_SCOPE)
	
	# Write version to .install file
	STRING(TIMESTAMP BUILD_DATE "%d/%m/%y %H:%M:%S")
	SET(INSTALL_MANIFEST_STR "[Package]=${PROJECT_NAME}\
\n[Version]=${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_BUILD}\
\n[BuildHash]=${PROJECT_COMMITHASH}\
\n[BuildBranch]=${PROJECT_BRANCH}\
\n[BuildDate]=${BUILD_DATE}")
	FILE(WRITE ${CMAKE_BINARY_DIR}/.install "${INSTALL_MANIFEST_STR}")
ENDFUNCTION()
#==================================================

#==================================================
FUNCTION(SET_UNIT_TEST_PROJECT_CONFIG)
	SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${TESTS_FOLDER})				# Place project in folder Unit-Tests (for some IDE)

	SET(GTEST_ROOT ${OV_CUSTOM_DEPENDENCIES_PATH}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest)	# Set link to Gtest root
	FIND_PACKAGE(GTest REQUIRED)														# Find Package
	INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})											# Add gtest dir to root include dir
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})						# Add google Test to project
ENDFUNCTION()
#==================================================
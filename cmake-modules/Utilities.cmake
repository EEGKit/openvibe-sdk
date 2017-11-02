function(debug_message)
	if(${Flag_VerboseOutput})
		message(STATUS "${ARGV}")
	endif()
endfunction()

# Set version based on git tag.
#  If current commit is tagged, use the tag as it is, and add build number based on content of .build file, written by Jenkins
#  Else use last tag major and minor number and set patch number to 99
#
# This function should remain generic to be usable in every projects.
function(set_version)
	find_package(Git)
	if(NOT GIT_FOUND)
		message(WARNING "Git not found, set version to 0.0.0")
		set(PROJECT_VERSION "0.0.0")
		set(PROJECT_BRANCH_STRING "unknown")
		set(PROJECT_COMMITHASH_STRING "0")
	else()
		debug_message("Found Git: ${GIT_EXECUTABLE}")
		execute_process(COMMAND ${GIT_EXECUTABLE} describe
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			OUTPUT_VARIABLE  PROJECT_VERSION
			ERROR_VARIABLE  ERROR)
		if(ERROR)
			message(WARNING "No tags found, set version to 0.0.0")
			set(PROJECT_VERSION "0.0.0")
			set(PROJECT_BRANCH_STRING "unknown")
			set(PROJECT_COMMITHASH_STRING "0")
		else()
			# codename = the name of the current branch
			execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				OUTPUT_VARIABLE  PROJECT_BRANCH_STRING)
			# command output may contain carriage return
			string(STRIP ${PROJECT_BRANCH_STRING} PROJECT_BRANCH_STRING)

			# commithash = short hash of latest revision
			execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				OUTPUT_VARIABLE  PROJECT_COMMITHASH_STRING)
			# command output may contain carriage return
			string(STRIP ${PROJECT_COMMITHASH_STRING} PROJECT_COMMITHASH_STRING)
		endif()
	endif()

	# if current commit is not tagged result is formed as: "major.minor.patch-number of commits since last tag-hash"
	string(STRIP ${PROJECT_VERSION} PROJECT_VERSION)
	string(REPLACE "-" ";" version_list ${PROJECT_VERSION})
	list(LENGTH version_list version_list_length)
	if(${version_list_length} EQUAL 3) # if result is formed as "major.minor.patch-number of commits since last tag-hash" set patch as 99
		list(GET version_list 0 PROJECT_VERSION)
		set(PROJECT_VERSION_PATCH 99)
	endif()

	string(REPLACE "." ";" version_list ${PROJECT_VERSION})
	list(GET version_list 0 PROJECT_VERSION_MAJOR)
	list(GET version_list 1 PROJECT_VERSION_MINOR)
	if(NOT PROJECT_VERSION_PATCH)
		list(GET version_list 2 PROJECT_VERSION_PATCH)
	endif()

	# These versions are used by the subprojects by default.
	# If you wish to maintain specific version numbers for a subproject, please do so in the projects CMakeLists.txt
	if(EXISTS ${CMAKE_SOURCE_DIR}/.build)
		file(READ ${CMAKE_SOURCE_DIR}/.build PROJECT_VERSION_BUILD)
		string(STRIP ${PROJECT_VERSION_BUILD} PROJECT_VERSION_BUILD)
	else()
		set(PROJECT_VERSION_BUILD 0)
	endif()
	
	set(PROJECT_BRANCH ${PROJECT_BRANCH_STRING} PARENT_SCOPE)
	set(PROJECT_COMMITHASH ${PROJECT_COMMITHASH_STRING} PARENT_SCOPE)

	set(PROJECT_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
	set(PROJECT_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
	set(PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)
	set(PROJECT_VERSION_BUILD ${PROJECT_VERSION_BUILD} PARENT_SCOPE)
	set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} PARENT_SCOPE)
endfunction()

project(openvibe-test)

if(WIN32)
	message(FATAL "Install & run deprecated")
	#add_custom_target(${PROJECT_NAME}
	#	COMMAND ctest-launcher.cmd -T Test
	#	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test")
else()
	add_custom_target(${PROJECT_NAME}-env
		COMMAND ./unix-dependencies
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test/scripts"
		COMMENT "Install test project dependencies"
	)

	add_custom_target(${PROJECT_NAME}-build
		COMMAND ./unix-build --disable-robot
			"--build-unit=${CMAKE_BUILD_TYPE}"
			"--root-dir=${CMAKE_INSTALL_PREFIX}"
			"--test-build-dir=${CMAKE_BINARY_DIR}/test"
			"--test-data-dir=${OVT_TEST_DATA_DIR}"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test/scripts"
		COMMENT "Build test project"
	)

	add_custom_target(${PROJECT_NAME}
		COMMAND bash ctest-launcher.sh -T Test
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test")
endif()

add_dependencies(${PROJECT_NAME}-build ${PROJECT_NAME}-env)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-build)

make_directory(${CMAKE_BINARY_DIR}/test)

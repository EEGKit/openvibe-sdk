project(test-run)

add_custom_target(${PROJECT_NAME}-cmake
	COMMAND cmake "${CMAKE_SOURCE_DIR}/test" "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}" "-DOV_ROOT_DIR=${CMAKE_INSTALL_PREFIX}" "-GNinja"
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test"
	COMMENT "Configuring the test project"
	)

add_custom_target(${PROJECT_NAME}-build
	COMMAND ninja
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test"
	COMMENT "Building the test project"
	)

if(WIN32)
	add_custom_target(${PROJECT_NAME}
		COMMAND "ctest-launcher.cmd"
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test")
else()
	add_custom_target(${PROJECT_NAME}
		COMMAND sh ctest-launcher.sh
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test")
endif()

add_dependencies(${PROJECT_NAME}-build ${PROJECT_NAME}-cmake)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-build)

make_directory(${CMAKE_BINARY_DIR}/test)

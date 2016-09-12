project(openvibe-test)

if(WIN32)
	add_custom_target(${PROJECT_NAME}-cmake
		COMMAND cmake "${CMAKE_SOURCE_DIR}/test"
			"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DOV_ROOT_DIR=${CMAKE_INSTALL_PREFIX}"
			"-GNinja"
			"-DOVT_TEST_DATA_DIR=${OVT_TEST_DATA_DIR}"
			"-DBUILD_ROBOT_TEST=0"
			"-DBUILD_UNIT_TEST=1"
			"-DGTEST_INCLUDE_DIR=${OV_CUSTOM_DEPENDENCIES_PATH}/gtest/include"
			"-DGTEST_LIBRARY=${OV_CUSTOM_DEPENDENCIES_PATH}/gtest/lib/gtest.lib"
			"-DGTEST_LIBRARY_DEBUG=${OV_CUSTOM_DEPENDENCIES_PATH}/gtest/lib/gtestd.lib"
			"-DGTEST_MAIN_LIBRARY=${OV_CUSTOM_DEPENDENCIES_PATH}/gtest/lib/gtest_main.lib"
			"-DGTEST_MAIN_LIBRARY_DEBUG=${OV_CUSTOM_DEPENDENCIES_PATH}/gtest/lib/gtest_maind.lib"
			WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test"
		COMMENT "Configuring the test project")

	add_custom_target(${PROJECT_NAME}-build
		COMMAND ninja
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test"
		COMMENT "Building the test project")
	
	add_custom_target(${PROJECT_NAME}
		COMMAND ctest-launcher.cmd -T Test
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/test")
	
	add_dependencies(${PROJECT_NAME}-build ${PROJECT_NAME}-cmake)
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
		
	add_dependencies(${PROJECT_NAME}-build ${PROJECT_NAME}-env)

endif()

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-build)

make_directory(${CMAKE_BINARY_DIR}/test)

project(module-fs-test VERSION ${OPENVIBE_MAJOR_VERSION}.${OPENVIBE_MINOR_VERSION}.${OPENVIBE_PATCH_VERSION})

file(GLOB_RECURSE TESTS_SRC_FILES *.cpp *.hpp)

add_executable(${PROJECT_NAME} ${TESTS_SRC_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${TESTS_FOLDER})	# Place project in folder unit-test (for some IDE)

target_link_libraries(${PROJECT_NAME}
					  openvibe-module-fs
					  Boost::boost
					  Boost::filesystem
					  GTest::GTest
					  GTest::Main)

# ---------------------------------
# Target macros
# Defines target operating system, architecture and compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------
add_definitions("-DDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
add_definitions("-DTMP_DIR=\"${OVT_TEST_TEMPORARY_DIR}\"")

ADD_TEST(NAME test_Module_FS COMMAND ${PROJECT_NAME})

OV_INSTALL_LAUNCH_SCRIPT(SCRIPT_PREFIX "${PROJECT_NAME}" EXECUTABLE_NAME  "${PROJECT_NAME}")
install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})
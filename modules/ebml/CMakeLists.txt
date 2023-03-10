project(openvibe-module-ebml VERSION ${OPENVIBE_MAJOR_VERSION}.${OPENVIBE_MINOR_VERSION}.${OPENVIBE_PATCH_VERSION})

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h src/*.hpp src/*.inl include/*.h)

include("FindSourceRCProperties")

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} openvibe-common)

target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES
					  VERSION ${PROJECT_VERSION}
					  SOVERSION ${PROJECT_VERSION_MAJOR}
					  FOLDER ${MODULES_FOLDER})

target_compile_options(${PROJECT_NAME}
					   PRIVATE -DEBML_Exports
					   PUBLIC -DEBML_Shared)

					   # ---------------------------------
# Target macros
# Defines target operating system, architecture and compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------
install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION ${DIST_BINDIR}
		LIBRARY DESTINATION ${DIST_LIBDIR}
		ARCHIVE DESTINATION ${DIST_LIBDIR})

INSTALL(DIRECTORY include/ DESTINATION ${DIST_INCLUDEDIR} COMPONENT headers FILES_MATCHING PATTERN "*.h")
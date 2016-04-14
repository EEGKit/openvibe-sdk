PROJECT(openvibe-toolkit)
OV_ADD_THIS_TO_PROJECT_LIST()

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})


#We need to generate the stimulation files
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/ovtk_stimulations.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ovtk_stimulations.cpp
    DEPENDS openvibe-stimulation-generator ${CMAKE_CURRENT_SOURCE_DIR}/share/stimulation_list.txt
    COMMAND openvibe-stimulation-generator
    --cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/share/stimulation_list.txt
    ${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/ovtk_stimulations.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ovtk_stimulations.cpp
    COMMENT "Generating stimulation sources..." )
    
add_custom_target(generate_stimulations_file DEPENDS include/toolkit/ovtk_stimulations.h src/ovtk_stimulations.cpp)

FILE(GLOB_RECURSE source_files src/*.cpp src/*.h src/*.inl src/*.hpp include/*.h)
INCLUDE("FindSourceRCProperties")

INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/toolkit/algorithms/scenario-io)
INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/toolkit/algorithms/classification)
INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/toolkit/deprecated/writer)
INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/toolkit/deprecated/reader)
INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/toolkit/training)
INCLUDE_DIRECTORIES(${openvibe-toolkit_SOURCE_DIR}/include/toolkit/tools)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${source_files} ${CMAKE_CURRENT_SOURCE_DIR}/src/ovtk_stimulations.cpp)
add_dependencies(openvibe-toolkit generate_stimulations_file)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DOVTK_Exports -DOVTK_Shared")

INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEModuleEBML")
INCLUDE("FindOpenViBEModuleXML")
INCLUDE("FindOpenViBEModuleSystem")

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
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

INSTALL(DIRECTORY include/   DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

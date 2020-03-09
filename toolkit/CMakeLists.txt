PROJECT(openvibe-toolkit)
OV_ADD_THIS_TO_PROJECT_LIST()

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})


# Command to generate cpp compliant stimulation files from text file
# Creating output directories is mandatory as it is not handled by ADD_CUSTOM_COMMAND
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/toolkit)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)
ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/toolkit/ovtk_stimulations.h ${CMAKE_CURRENT_BINARY_DIR}/src/ovtk_stimulations.cpp
    DEPENDS openvibe-stimulation-generator ${CMAKE_CURRENT_SOURCE_DIR}/share/stimulation_list.txt
    COMMAND openvibe-stimulation-generator
    --cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/share/stimulation_list.txt
    ${CMAKE_CURRENT_BINARY_DIR}/include/toolkit/ovtk_stimulations.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/ovtk_stimulations.cpp
    COMMENT "Generating stimulation sources..." )

# ADD_CUSTOM_TARGET will automatically trigger the command defined above
# as it depends on its outputs
ADD_CUSTOM_TARGET(openvibe-generate-stimulation-file
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/toolkit/ovtk_stimulations.h ${CMAKE_CURRENT_BINARY_DIR}/src/ovtk_stimulations.cpp)

FILE(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h src/*.inl src/*.hpp include/*.h)
INCLUDE("FindSourceRCProperties")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/algorithms/scenario-io)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/algorithms/classification)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/deprecated/writer)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/deprecated/reader)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/training)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include/toolkit/tools)

# add binary directory to includes as it contains generated stimulation header
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/include/)

ADD_LIBRARY(${PROJECT_NAME} SHARED
	${SRC_FILES}
	${CMAKE_CURRENT_BINARY_DIR}/include/toolkit/ovtk_stimulations.h
	${CMAKE_CURRENT_BINARY_DIR}/src/ovtk_stimulations.cpp
	)
ADD_DEPENDENCIES(${PROJECT_NAME} openvibe-generate-stimulation-file)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DOVTK_Exports -DOVTK_Shared")

# ---------------------------------
INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEModuleEBML")
INCLUDE("FindOpenViBEModuleXML")
INCLUDE("FindOpenViBEModuleFS")
INCLUDE("FindThirdPartyBoost_FileSystem")

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

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ ${CMAKE_CURRENT_BINARY_DIR}/include/   DESTINATION  ${DIST_INCLUDEDIR} FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY share/ DESTINATION ${DIST_DATADIR}/openvibe/toolkit PATTERN "*-base" EXCLUDE)


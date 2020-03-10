PROJECT(openvibe-plugins-sdk-file-io)

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

FILE(GLOB_RECURSE SRC_FILES src/*.cpp src/*.c src/*.h src/*.hpp src/*.inl)
LIST(REMOVE_ITEM SRC_FILES
	src/box-algorithms/csv/ovpCBoxAlgorithmCSVFileReader.cpp
	src/box-algorithms/csv/ovpCBoxAlgorithmCSVFileWriter.cpp
	src/box-algorithms/csv/ovpCBoxAlgorithmCSVFileReader.h
	src/box-algorithms/csv/ovpCBoxAlgorithmCSVFileWriter.h)
INCLUDE("FindSourceRCProperties")
ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRC_FILES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
	COMPILE_FLAGS "-DOVP_Exports -DOVP_Shared -D_LARGEFILE64_SOURCE -D_LARGEFILE_SOURCE")

# ---------------------------------
INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEToolkit")
INCLUDE("FindOpenViBEModuleCSV")
INCLUDE("FindOpenViBEModuleEBML")
INCLUDE("FindOpenViBEModuleXML")
INCLUDE("FindOpenViBEModuleFS")
INCLUDE("FindThirdPartyBoost")
INCLUDE("FindThirdPartyXerces")

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

INSTALL(DIRECTORY box-tutorials  DESTINATION ${DIST_DATADIR}/openvibe/scenarios/)
INSTALL(DIRECTORY signals DESTINATION ${DIST_DATADIR}/openvibe/scenarios/)
INSTALL(DIRECTORY electrode_sets DESTINATION ${DIST_DATADIR}/openvibe/)


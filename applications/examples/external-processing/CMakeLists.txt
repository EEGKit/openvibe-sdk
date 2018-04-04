PROJECT(sdk-examples-communication-client-generator)

SET(source_files src/main-generator.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${source_files})

INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEModuleEBML")
INCLUDE("FindOpenViBEModuleSocket")
INCLUDE("FindOpenViBEModuleCommunication")

SET_BUILD_PLATFORM()

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})

PROJECT(sdk-examples-communication-client-filter)

SET(source_files src/main-filter.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${source_files})

INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEModuleSocket")
INCLUDE("FindOpenViBEModuleCommunication")
INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBEToolkit")

SET_BUILD_PLATFORM()

INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})

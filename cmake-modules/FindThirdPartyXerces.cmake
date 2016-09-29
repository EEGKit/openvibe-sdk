# ---------------------------------
# Finds Xerces dependency
# ---------------------------------

IF(WIN32)
	SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${OV_CUSTOM_DEPENDENCIES_PATH}/xerces-c/include")
	SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${OV_CUSTOM_DEPENDENCIES_PATH}/xerces-c/lib")
ENDIF()

FIND_PACKAGE(XercesC REQUIRED)

IF(XercesC_FOUND)
	MESSAGE(STATUS "  Found xerces...")

	INCLUDE_DIRECTORIES(${XercesC_INCLUDE_DIRS})

	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${XercesC_LIBRARIES})
	ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyXERCES)

ELSE()
	MESSAGE(STATUS "  FAILED to find xerces...")
ENDIF()



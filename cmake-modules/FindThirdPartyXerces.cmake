# ---------------------------------
# Finds Xerces dependency
# ---------------------------------

FIND_PACKAGE(XercesC REQUIRED)

IF(XercesC_FOUND)
	MESSAGE(STATUS "  Found xerces...")

	INCLUDE_DIRECTORIES(${XercesC_INCLUDE_DIRS})

	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${XercesC_LIBRARIES})
	ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyXERCES)

ELSE()
	MESSAGE(STATUS "  FAILED to find xerces...")
ENDIF()



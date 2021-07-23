# ---------------------------------
# Finds third party boost
# Adds library to target
# Adds include path
# ---------------------------------
IF(APPLE)
find_package(Boost 1.60.0 COMPONENTS regex)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
ELSEIF(UNIX)
	find_package(Boost 1.54.0 COMPONENTS regex)
	if(Boost_FOUND)
	  include_directories(${Boost_INCLUDE_DIRS})
	  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
	endif()
ENDIF()

IF(WIN32)
	OV_LINK_BOOST_LIB("regex" ${OV_WIN32_BOOST_VERSION} )
ENDIF(WIN32)

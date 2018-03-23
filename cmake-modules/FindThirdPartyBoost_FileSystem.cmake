# ---------------------------------
# Finds third party boost
# Adds library to target
# Adds include path
# ---------------------------------
IF(APPLE)
find_package(Boost 1.60.0 COMPONENTS filesystem system)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()
ELSEIF(UNIX)
	find_package(Boost 1.54.0 COMPONENTS filesystem system)
	if(Boost_FOUND)
	  include_directories(${Boost_INCLUDE_DIRS})
	  target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
	endif()

	# For Fedora
	FIND_LIBRARY(LIB_STANDARD_MODULE_PTHREAD pthread)
	IF(LIB_STANDARD_MODULE_PTHREAD)
		debug_message( "  Found pthread...")
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIB_STANDARD_MODULE_PTHREAD})
	ELSE(LIB_STANDARD_MODULE_PTHREAD)
		MESSAGE(WARNING "  FAILED to find pthread...")
	ENDIF(LIB_STANDARD_MODULE_PTHREAD)
ENDIF()

IF(WIN32)
	OV_LINK_BOOST_LIB("filesystem" ${OV_WIN32_BOOST_VERSION} )
	OV_LINK_BOOST_LIB("system" ${OV_WIN32_BOOST_VERSION} )			# filesystem depends on system
ENDIF(WIN32)

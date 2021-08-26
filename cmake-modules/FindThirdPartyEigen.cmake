# ---------------------------------
# Finds Eigen headers
#
# Adds include path
# ---------------------------------

IF(WIN32)
	SET(PATH_EIGEN "PATH_EIGEN-NOTFOUND")
	FIND_PATH(PATH_EIGEN Eigen/Eigen PATHS ${LIST_DEPENDENCIES_PATH}/eigen NO_DEFAULT_PATH)
	IF(PATH_EIGEN)
		SET(EIGEN_FOUND TRUE)
		SET(EIGEN_INCLUDE_DIRS ${PATH_EIGEN})
		SET(EIGEN_CFLAGS "")
		#SET(EIGEN_LIBRARIES_RELEASE EIGEN)
		#SET(EIGEN_LIBRARIES_DEBUG EIGENd)	
		#SET(EIGEN_LIBRARY_DIRS ${PATH_EIGEN}/lib )
	ENDIF(PATH_EIGEN)
ENDIF(WIN32)

IF(UNIX)
    
	# @FIXME remove that when an Ubuntu package will be available
	# Find in priority local library for eigen even if linux package has been installed
	# reason: package on distri Ubuntu 16.04 is still bugged (eigen 3.2.92)
	INCLUDE("FindPkgConfig")		
	UNSET(PATH_EIGEN_LOCAL CACHE)		
	FIND_PATH(PATH_EIGEN_LOCAL eigen3 PATHS "/usr/local/include" NO_DEFAULT_PATH)
	SET(EIGEN_FOUND FALSE)
	IF (PATH_EIGEN_LOCAL)	
		SET(EIGEN_INCLUDE_DIRS "${PATH_EIGEN_LOCAL}/eigen3")				
		SET(EIGEN_CFLAGS "-I${EIGEN_INCLUDE_DIRS}")		
		SET(EIGEN_FOUND TRUE)
	ELSE()			
		pkg_check_modules(EIGEN eigen3)			
	ENDIF()
	
	# @FIXME uncomment that when an Ubuntu package will be available 
	#SET(EIGEN_FOUND FALSE)
	#INCLUDE("FindPkgConfig")
	#pkg_check_modules(EIGEN eigen3)

ENDIF(UNIX)

IF(EIGEN_FOUND)
	debug_message( "Found eigen3... [${EIGEN_INCLUDE_DIRS}]")
	INCLUDE_DIRECTORIES("${EIGEN_INCLUDE_DIRS}")
	ADD_DEFINITIONS(${EIGEN_CFLAGS})
	ADD_DEFINITIONS(-DTARGET_HAS_ThirdPartyEIGEN)
ELSE(EIGEN_FOUND)
	MESSAGE(WARNING "  FAILED to find eigen3...")
ENDIF(EIGEN_FOUND)




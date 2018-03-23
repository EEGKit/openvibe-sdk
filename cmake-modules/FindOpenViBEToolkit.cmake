# ---------------------------------
# Finds openvibe-toolkit
# Adds library to target
# Adds include path
# ---------------------------------
OPTION(LINK_OPENVIBE_TOOLKIT "By default, link openvibe-toolkit, otherwise only use the includes" ON)
OPTION(DYNAMIC_LINK_OPENVIBE_TOOLKIT "Dynamically link openvibe-toolkit" ON)

IF(DYNAMIC_LINK_OPENVIBE_TOOLKIT)
	SET(OPENVIBE_TOOLKIT_LINKING "")
	ADD_DEFINITIONS(-DOVTK_Shared)
ELSE()
	SET(OPENVIBE_TOOLKIT_LINKING "-static")
	ADD_DEFINITIONS(-DOVTK_Static)
ENDIF()

SET(SRC_DIR ${OV_BASE_DIR}/toolkit/include)

FIND_PATH(PATH_OPENVIBE_TOOLKIT toolkit/ovtk_all.h PATHS ${SRC_DIR} NO_DEFAULT_PATH)

IF(PATH_OPENVIBE_TOOLKIT)
	debug_message( "  Found openvibe-toolkit...  ${PATH_OPENVIBE_TOOLKIT} ${PATH_OPENVIBE_TOOLKIT_BIN}")
	INCLUDE_DIRECTORIES(${PATH_OPENVIBE_TOOLKIT})
	
	# No find path for this directory as content is not generated yet
	INCLUDE_DIRECTORIES(${OV_BASE_BIN_DIR}/toolkit/include)
		
	IF(LINK_OPENVIBE_TOOLKIT)
		TARGET_LINK_LIBRARIES(${PROJECT_NAME} openvibe-toolkit${OPENVIBE_TOOLKIT_LINKING})	
	ENDIF()

	ADD_DEFINITIONS(-DTARGET_HAS_OpenViBEToolkit)
ELSE()
	MESSAGE(FATAL "  FAILED to find openvibe-toolkit...")
ENDIF()


INCLUDE("FindOpenViBEModuleXML")

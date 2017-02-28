# ---------------------------------
# Finds Certivibe binary distribution
# Adds library to target
# Adds include path
# ---------------------------------
option(DYNAMIC_LINK_CERTIVIBE "Dynamically link Certivibe" ON)

set(CERTIVIBE_DIRECTORY ${OPENVIBE_SDK_PATH})

set(PATH_CERTIVIBE "PATH_CERTIVIBE-NOTFOUND")
find_path(PATH_CERTIVIBE include/openvibe/ov_all.h PATHS ${CERTIVIBE_DIRECTORY} NO_DEFAULT_PATH)
if(${PATH_CERTIVIBE} STREQUAL "PATH_CERTIVIBE-NOTFOUND")
	message(FATAL_ERROR "  FAILED to find Certivibe [${PATH_CERTIVIBE}]")
endif()

debug_message( "  Found Certivibe... [${PATH_CERTIVIBE}]")

include_directories(${PATH_CERTIVIBE}/include/)# TODO ?

if(DYNAMIC_LINK_CERTIVIBE)
	set(CERTIVIBE_LINKING "")
	add_definitions(-DOV_Shared)
	add_definitions(-DEBML_Shared)
	add_definitions(-DFS_Shared)
	add_definitions(-DSocket_Shared)
	add_definitions(-DSystem_Shared)
	add_definitions(-DXML_Shared)
	add_definitions(-DOVTK_Shared)
	add_definitions(-DCSV_Shared)
	add_definitions(-DDATE_Shared)
else()
	set(CERTIVIBE_LINKING "-static")
	add_definitions(-DEBML_Static)
	add_definitions(-DFS_Static)
	add_definitions(-DSocket_Static)
	add_definitions(-DSystem_Static)
	add_definitions(-DXML_Static)
	add_definitions(-DOVTK_Static)
	add_definitions(-DCSV_Static)
	add_definitions(-DDATE_Static)
endif()

if("${INCLUDED_CERTIVIBE_COMPONENTS}" STREQUAL "ALL")
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS "ALL")
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS BASE ALLPLUGINS ALLMODULES)
endif()

if(BASE IN_list INCLUDED_CERTIVIBE_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS BASE)
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS MAIN KERNEL)	
endif()

if(ALLPLUGINS IN_list INCLUDED_CERTIVIBE_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS ALLPLUGINS)
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS CLASSifICATION DATA_GENERATION FEATURE_EXTRACTION FILE_IO SIGNAL_PROCESSING STIMULATION STREAM_CODECS STREAMING TOOLS)
endif()

if(ALLMODULES IN_list INCLUDED_CERTIVIBE_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS ALLMODULES)
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS EBML SYSTEM FS SOCKET XML DATE CSV TOOLKIT)
endif()

if(WIN32)
	set(LIB_PREFIX "")
	set(ORIG_LIB_DIR bin)
	set(DEST_LIB_DIR  ${CMAKE_INSTALL_FULL_BINDIR})
	set(LIB_EXT lib)
	set(DLL_EXT dll)
	install(DIRECTORY ${PATH_CERTIVIBE}/bin/ DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} FILES_MATCHING PATTERN "*cmd")
elseif(UNIX)
	set(LIB_PREFIX lib)
	set(ORIG_LIB_DIR lib)
	set(DEST_LIB_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
	set(LIB_EXT "so")
	if(APPLE)
		set(LIB_EXT "dylib")
	endif()
	set(DLL_EXT "${LIB_EXT}*")
endif()

function(add_component TOKEN MODULE_NAME COMPILE_FLAG)
	set(COMPILE_FLAG ${ARGV2})
	if(${TOKEN} IN_list INCLUDED_CERTIVIBE_COMPONENTS)
		target_link_libraries(${PROJECT_NAME} "${PATH_CERTIVIBE}/lib/${LIB_PREFIX}${MODULE_NAME}${CERTIVIBE_LINKING}.${LIB_EXT}")
		install(DIRECTORY ${PATH_CERTIVIBE}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
		if(COMPILE_FLAG)
			add_definitions(-D${COMPILE_FLAG})
		endif()
	endif()
endfunction(add_component)

add_component(MAIN "openvibe" "TARGET_HAS_OpenViBE")
add_component(KERNEL "openvibe-kernel") #TODO TARGET_HAS_
add_component(TOOLKIT "openvibe-toolkit" "TARGET_HAS_OpenViBEToolkit")

#modules
add_component(EBML "openvibe-module-ebml" "TARGET_HAS_EBML")
add_component(SYSTEM "openvibe-module-system" "TARGET_HAS_System")
add_component(FS "openvibe-module-fs" "TARGET_HAS_FS")
add_component(SOCKET "openvibe-module-socket" "TARGET_HAS_Socket")
add_component(XML "openvibe-module-xml" "TARGET_HAS_XML")
add_component(CSV "openvibe-module-csv" "TARGET_HAS_CSV")
add_component(DATE "openvibe-module-date" "TARGET_HAS_DATE")

#plugins
add_component(CLASSIFICATION "certivibe-plugins-classification")
add_component(DATA_GENERATION "certivibe-plugins-data-generation")
add_component(FEATURE_EXTRACTION "certivibe-plugins-feature-extraction")
add_component(FILE_IO "certivibe-plugins-file-io")
add_component(SIGNAL_PROCESSING "certivibe-plugins-signal-processing")
add_component(STIMULATION "certivibe-plugins-stimulation")
add_component(STREAM_CODECS "certivibe-plugins-stream-codecs") 
add_component(STREAMING "certivibe-plugins-streaming")
add_component(TOOLS "certivibe-plugins-tools")

add_definitions(-DTARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines)

# if we link with the module socket in Static, we must link the project with the dependency on win32
if(WIN32 AND SOCKET IN_LIST INCLUDED_CERTIVIBE_COMPONENTS AND NOT DYNAMIC_LINK_CERTIVIBE)
	include("FindThirdPartyWinsock2")
	include("FindThirdPartyFTDI")
endif()

install(DIRECTORY ${PATH_CERTIVIBE}/share/ DESTINATION ${CMAKE_INSTALL_FULL_DATADIR})


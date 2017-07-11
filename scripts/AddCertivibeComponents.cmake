# ---------------------------------
# Finds OpenViBE SDK binary distribution
# Adds library to target, include path and execute install commands
# Also add library specific compiler flags
# This should be used with a defined INCLUDED_CERTIVIBE_COMPONENTS variable in scope
# Value should be a list of libraries to include
# They are organized into groups, and group name can also be used to add all members of said group
# This include :
# ALL : will include groups BASE, ALLPLUGINS and ALLMODULES
# BASE : MAIN KERNEL
# ALLPLUGINS : CLASSIFICATION DATA_GENERATION FEATURE_EXTRACTION FILE_IO SIGNAL_PROCESSING STIMULATION STREAM_CODECS STREAMING TOOLS
# ALLMODULES : EBML SYSTEM FS SOCKET XML DATE CSV TOOLKIT
# ---------------------------------
option(DYNAMIC_LINK_OPENVIBE_SDK "Dynamically link Certivibe" ON)

# set(OPENVIBE_SDK_PATH "OPENVIBE_SDK_PATH-NOTFOUND")
find_path(${OPENVIBE_SDK_PATH} include/openvibe/ov_all.h PATHS ${LIST_DEPENDENCIES_PATH} NO_DEFAULT_PATH)
if(${OPENVIBE_SDK_PATH} STREQUAL "OPENVIBE_SDK_PATH-NOTFOUND")
	message(FATAL_ERROR "  FAILED to find OpenViBE SDK [${OPENVIBE_SDK_PATH}]")
endif()

debug_message( "  Found OpenViBE SDK... [${OPENVIBE_SDK_PATH}]")

include_directories(${OPENVIBE_SDK_PATH}/include/)# TODO ?

if(DYNAMIC_LINK_OPENVIBE_SDK)
	set(OPENVIBE_SDK_LINKING "")
	set(LINKING_SUFFIX Shared)
	add_definitions(-DOV_Shared)
else()
	set(OPENVIBE_SDK_LINKING "-static")
	set(LINKING_SUFFIX Static)
endif()

if("${INCLUDED_CERTIVIBE_COMPONENTS}" STREQUAL "ALL")
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS "ALL")
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS BASE ALLPLUGINS ALLMODULES)
endif()

if(BASE IN_LIST INCLUDED_CERTIVIBE_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS BASE)
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS MAIN KERNEL)	
endif()

if(ALLPLUGINS IN_LIST INCLUDED_CERTIVIBE_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS ALLPLUGINS)
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS CLASSIFICATION DATA_GENERATION FEATURE_EXTRACTION FILE_IO SIGNAL_PROCESSING STIMULATION STREAM_CODECS STREAMING TOOLS)
endif()

if(ALLMODULES IN_LIST INCLUDED_CERTIVIBE_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_CERTIVIBE_COMPONENTS ALLMODULES)
	list(APPEND INCLUDED_CERTIVIBE_COMPONENTS EBML SYSTEM FS SOCKET XML DATE CSV TOOLKIT)
endif()

if(WIN32)
	set(LIB_PREFIX "")
	set(ORIG_LIB_DIR bin)
	set(DEST_LIB_DIR  ${CMAKE_INSTALL_FULL_BINDIR})
	set(LIB_EXT lib)
	set(DLL_EXT dll)
	install(DIRECTORY ${OPENVIBE_SDK_PATH}/bin/ DESTINATION ${CMAKE_INSTALL_FULL_BINDIR} FILES_MATCHING PATTERN "*cmd")
elseif(UNIX)
	set(LIB_PREFIX lib)
	set(ORIG_LIB_DIR lib)
	set(DEST_LIB_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
	if(DYNAMIC_LINK_OPENVIBE_SDK)
		set(LIB_EXT "so")
		if(APPLE)
			set(LIB_EXT "dylib")
		endif()
	else()
		set(LIB_EXT "a")
	endif()
	set(DLL_EXT "${LIB_EXT}*")
endif()

function(add_component TOKEN MODULE_NAME)
	if(${TOKEN} IN_LIST INCLUDED_CERTIVIBE_COMPONENTS)
		target_link_libraries(${PROJECT_NAME} "${OPENVIBE_SDK_PATH}/lib/${LIB_PREFIX}${MODULE_NAME}${CERTIVIBE_LINKING}.${LIB_EXT}")
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
		
		set(FLAGS_LIST ${ARGV})
		list(REMOVE_AT FLAGS_LIST 0 1) # Remove mandatory args to get only optional args
		foreach(COMPILE_FLAG IN LISTS FLAGS_LIST)
			add_definitions(-D${COMPILE_FLAG})
		endforeach(COMPILE_FLAG)
	endif()
endfunction(add_component)

function(add_plugin TOKEN MODULE_NAME)
	if(${TOKEN} IN_LIST INCLUDED_CERTIVIBE_COMPONENTS)
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
	endif()
endfunction(add_plugin)

add_component(MAIN "openvibe" "TARGET_HAS_OpenViBE")
add_component(KERNEL "openvibe-kernel") #TODO TARGET_HAS_
add_component(TOOLKIT "openvibe-toolkit" "TARGET_HAS_OpenViBEToolkit" "OVTK_${LINKING_SUFFIX}")

#modules
add_component(EBML "openvibe-module-ebml" "TARGET_HAS_EBML" "EBML_${LINKING_SUFFIX}")
add_component(SYSTEM "openvibe-module-system" "TARGET_HAS_System" "System_${LINKING_SUFFIX}")
add_component(FS "openvibe-module-fs" "TARGET_HAS_FS" "FS_${LINKING_SUFFIX}")
add_component(SOCKET "openvibe-module-socket" "TARGET_HAS_Socket" "Socket_${LINKING_SUFFIX}")
add_component(XML "openvibe-module-xml" "TARGET_HAS_XML" "XML_${LINKING_SUFFIX}")
add_component(CSV "openvibe-module-csv" "TARGET_HAS_CSV" "CSV_${LINKING_SUFFIX}")
add_component(DATE "openvibe-module-date" "TARGET_HAS_DATE" "DATE_${LINKING_SUFFIX}")

#plugins
add_plugin(CLASSIFICATION "certivibe-plugins-classification")
add_plugin(DATA_GENERATION "certivibe-plugins-data-generation")
add_plugin(FEATURE_EXTRACTION "certivibe-plugins-feature-extraction")
add_plugin(FILE_IO "certivibe-plugins-file-io")
add_plugin(SIGNAL_PROCESSING "certivibe-plugins-signal-processing")
add_plugin(STIMULATION "certivibe-plugins-stimulation")
add_plugin(STREAM_CODECS "certivibe-plugins-stream-codecs") 
add_plugin(STREAMING "certivibe-plugins-streaming")
add_plugin(TOOLS "certivibe-plugins-tools")

add_definitions(-DTARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines)

# Install binary dependencies if requested
if(DEPENDENCIES IN_LIST INCLUDED_CERTIVIBE_COMPONENTS)
	if(WIN32)
		install(
			FILES
			${OPENVIBE_SDK_PATH}/bin/libexpat.dll
			${OPENVIBE_SDK_PATH}/bin/libexpatw.dll
			${OPENVIBE_SDK_PATH}/bin/xerces-c_3_1.dll
			${OPENVIBE_SDK_PATH}/bin/xerces-c_3_1D.dll
			DESTINATION ${DEST_LIB_DIR})
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/include/ DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR})
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/lib/ DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})
	endif()
endif()

# if we link with the module socket in Static, we must link the project with the dependency on win32
if(WIN32 AND SOCKET IN_LIST INCLUDED_CERTIVIBE_COMPONENTS AND NOT DYNAMIC_LINK_OPENVIBE_SDK)
	include("FindThirdPartyWinsock2")
	include("FindThirdPartyFTDI")
endif()

install(DIRECTORY ${OPENVIBE_SDK_PATH}/share/ DESTINATION ${CMAKE_INSTALL_FULL_DATADIR})


# ---------------------------------
# Finds OpenViBE SDK binary distribution
# Adds library to target, include path and execute install commands
# Also add library specific compiler flags
# This should be used with a defined INCLUDED_OV_SDK_COMPONENTS variable in scope
# Value should be a list of libraries to include
# They are organized into groups, and group name can also be used to add all members of said group
# This include :
# ALL : will include groups BASE, ALLPLUGINS and ALLMODULES
# BASE : MAIN KERNEL
# ALLPLUGINS : CLASSIFICATION DATA_GENERATION FEATURE_EXTRACTION FILE_IO SIGNAL_PROCESSING STIMULATION STREAM_CODECS STREAMING TOOLS
# ALLMODULES : EBML SYSTEM FS SOCKET XML DATE CSV TOOLKIT
# ---------------------------------
option(DYNAMIC_LINK_OPENVIBE_SDK "Dynamically link OpenViBE SDK" ON)


if(NOT CMAKE_BUILD_TYPE AND CMAKE_GENERATOR MATCHES "Visual Studio*")
	set(MULTI_BUILD TRUE)
elseif(CMAKE_BUILD_TYPE AND OV_PACKAGE)
	set(SOLO_PACKAGE TRUE)
elseif(CMAKE_BUILD_TYPE)
	set(SOLO_BUILD TRUE)
else()
	message(FATAL_ERROR "Build should specify a type or use a multi-type generator (like Visual Studio)")
endif()

include_directories(${OPENVIBE_SDK_PATH}/include/)# TODO ?

if(DYNAMIC_LINK_OPENVIBE_SDK)
	set(OPENVIBE_SDK_LINKING "")
	set(LINKING_SUFFIX Shared)
	add_definitions(-DOV_Shared)
else()
	set(OPENVIBE_SDK_LINKING "-static")
	set(LINKING_SUFFIX Static)
endif()

if("${INCLUDED_OV_SDK_COMPONENTS}" STREQUAL "ALL")
	list(REMOVE_ITEM INCLUDED_OV_SDK_COMPONENTS "ALL")
	list(APPEND INCLUDED_OV_SDK_COMPONENTS BASE ALLPLUGINS ALLMODULES)
endif()

if(BASE IN_LIST INCLUDED_OV_SDK_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_OV_SDK_COMPONENTS BASE)
	list(APPEND INCLUDED_OV_SDK_COMPONENTS MAIN KERNEL)
endif()

if(ALLPLUGINS IN_LIST INCLUDED_OV_SDK_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_OV_SDK_COMPONENTS ALLPLUGINS)
	list(APPEND INCLUDED_OV_SDK_COMPONENTS CLASSIFICATION DATA_GENERATION FEATURE_EXTRACTION FILE_IO SIGNAL_PROCESSING STIMULATION STREAM_CODECS STREAMING TOOLS)
endif()

if(ALLMODULES IN_LIST INCLUDED_OV_SDK_COMPONENTS)
	list(REMOVE_ITEM INCLUDED_OV_SDK_COMPONENTS ALLMODULES)
	list(APPEND INCLUDED_OV_SDK_COMPONENTS EBML SYSTEM FS SOCKET XML DATE CSV TOOLKIT COMMUNICATION)
endif()

if(WIN32)
	set(LIB_PREFIX "")
	set(ORIG_LIB_DIR bin)
	set(DEST_LIB_DIR  ${DIST_BINDIR})
	set(LIB_EXT lib)
	set(DLL_EXT dll)
	set(EXE_EXT .exe)
	if(${GENERATOR_IS_MULTI_CONFIG})
		foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
			string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
			install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/ DESTINATION ${DIST_BINDIR} CONFIGURATIONS ${OUTPUTCONFIG} FILES_MATCHING PATTERN "*cmd")
		endforeach()
	else()
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/bin/ DESTINATION ${DIST_BINDIR} FILES_MATCHING PATTERN "*cmd")
	endif()
elseif(UNIX)
	set(LIB_PREFIX lib)
	set(ORIG_LIB_DIR lib)
	set(DEST_LIB_DIR ${DIST_LIBDIR})
	if(DYNAMIC_LINK_OPENVIBE_SDK)
		set(LIB_EXT "so")
		if(APPLE)
			set(LIB_EXT "dylib")
		endif()
	else()
		set(LIB_EXT "a")
	endif()
	set(DLL_EXT "${LIB_EXT}*")
	set(EXE_EXT "")
endif()

function(add_component TOKEN MODULE_NAME)
	if(${TOKEN} IN_LIST INCLUDED_OV_SDK_COMPONENTS)
		target_link_libraries(${PROJECT_NAME} "${OPENVIBE_SDK_PATH}/lib/${LIB_PREFIX}${MODULE_NAME}${OPENVIBE_SDK_LINKING}.${LIB_EXT}")
		if(${GENERATOR_IS_MULTI_CONFIG})
			foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
				string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
				install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} CONFIGURATIONS ${OUTPUTCONFIG} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
			endforeach()
		else()
			install(DIRECTORY ${OPENVIBE_SDK_PATH}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
		endif()

		set(FLAGS_LIST ${ARGV})
		list(REMOVE_AT FLAGS_LIST 0 1) # Remove mandatory args to get only optional args
		foreach(COMPILE_FLAG IN LISTS FLAGS_LIST)
			add_definitions(-D${COMPILE_FLAG})
		endforeach()
	endif()
endfunction(add_component)

function(add_plugin TOKEN MODULE_NAME)
	if(${TOKEN} IN_LIST INCLUDED_OV_SDK_COMPONENTS)
		if(${GENERATOR_IS_MULTI_CONFIG})
			foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
				string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
				install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} CONFIGURATIONS ${OUTPUTCONFIG} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
			endforeach()
		else()
			install(DIRECTORY ${OPENVIBE_SDK_PATH}/${ORIG_LIB_DIR}/ DESTINATION ${DEST_LIB_DIR} FILES_MATCHING PATTERN "*${MODULE_NAME}*${DLL_EXT}")
		endif()
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
add_component(COMMUNICATION "openvibe-module-communication" "TARGET_HAS_COMMUNICATION" "COMMUNICATION_${LINKING_SUFFIX}")

#plugins
add_plugin(CLASSIFICATION "openvibe-plugins-base-classification")
add_plugin(DATA_GENERATION "openvibe-plugins-base-data-generation")
add_plugin(FEATURE_EXTRACTION "openvibe-plugins-base-feature-extraction")
add_plugin(FILE_IO "openvibe-plugins-base-file-io")
add_plugin(SIGNAL_PROCESSING "openvibe-plugins-base-signal-processing")
add_plugin(STIMULATION "openvibe-plugins-base-stimulation")
add_plugin(STREAM_CODECS "openvibe-plugins-base-stream-codecs")
add_plugin(STREAMING "openvibe-plugins-base-streaming")
add_plugin(TOOLS "openvibe-plugins-base-tools")

add_definitions(-DTARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines)

# Install binary dependencies if requested
if(DEPENDENCIES IN_LIST INCLUDED_OV_SDK_COMPONENTS)
	if(WIN32)
		if(${GENERATOR_IS_MULTI_CONFIG}) # Replace with generator expression in CMake 3.5+
			foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
				string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
				install(FILES
					${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/libexpat.dll
					${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/xerces-c_3_1.dll
					${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/xerces-c_3_1D.dll
					DESTINATION ${DEST_LIB_DIR} CONFIGURATIONS ${OUTPUTCONFIG})
				install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/include/ DESTINATION ${DIST_INCLUDEDIR} CONFIGURATIONS ${OUTPUTCONFIG})
				install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/lib/ DESTINATION ${DIST_LIBDIR} CONFIGURATIONS ${OUTPUTCONFIG})
			endforeach()
		else()
			install(FILES
				${OPENVIBE_SDK_PATH}/bin/libexpat.dll
				${OPENVIBE_SDK_PATH}/bin/xerces-c_3_1.dll
				${OPENVIBE_SDK_PATH}/bin/xerces-c_3_1D.dll
				DESTINATION ${DEST_LIB_DIR})
			install(DIRECTORY ${OPENVIBE_SDK_PATH}/include/ DESTINATION ${DIST_INCLUDEDIR})
			install(DIRECTORY ${OPENVIBE_SDK_PATH}/lib/ DESTINATION ${DIST_LIBDIR})
		endif()
	endif()
endif()

# Install applications if required
if(DEVELOPER_TOOLS IN_LIST INCLUDED_OV_SDK_COMPONENTS)
	if(${GENERATOR_IS_MULTI_CONFIG}) # Replace with generator expression in CMake 3.5+
		foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
			string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
			install(FILES
				${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/openvibe-id-generator${EXE_EXT}
				${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/openvibe-plugin-inspector-sdk${EXE_EXT}
				DESTINATION ${DIST_BINDIR} CONFIGURATIONS ${OUTPUTCONFIG} FILE_PERMISSIONS OWNER_EXECUTE)
			install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/include/ DESTINATION ${DIST_INCLUDEDIR} CONFIGURATIONS ${OUTPUTCONFIG})
			install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/lib/ DESTINATION ${DIST_BINDIR} CONFIGURATIONS ${OUTPUTCONFIG})
		endforeach()
	else()
		install(FILES
			${OPENVIBE_SDK_PATH}/bin/openvibe-id-generator${EXE_EXT}
			${OPENVIBE_SDK_PATH}/bin/openvibe-plugin-inspector-sdk${EXE_EXT}
			DESTINATION ${DIST_BINDIR} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/include/ DESTINATION ${DIST_INCLUDEDIR})
		install(DIRECTORY ${OPENVIBE_SDK_PATH}/lib/ DESTINATION ${DIST_BINDIR})
	endif()
endif()

# if we link with the module socket in Static, we must link the project with the dependency on win32
if(WIN32 AND SOCKET IN_LIST INCLUDED_OV_SDK_COMPONENTS AND NOT DYNAMIC_LINK_OPENVIBE_SDK)
	include("FindThirdPartyWinsock2")
	include("FindThirdPartyFTDI")
endif()

if(${GENERATOR_IS_MULTI_CONFIG}) # Replace with generator expression in CMake 3.5+
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU )
		install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/share/ DESTINATION ${DIST_DATADIR} CONFIGURATIONS ${OUTPUTCONFIG})
		install(DIRECTORY ${OPENVIBE_SDK_PATH_${OUTPUTCONFIGU}}/bin/
			USE_SOURCE_PERMISSIONS
			DESTINATION ${DIST_BINDIR}
			CONFIGURATIONS ${OUTPUTCONFIG}
			FILES_MATCHING PATTERN "*example*")
	endforeach()
else()
	install(DIRECTORY ${OPENVIBE_SDK_PATH}/share/ DESTINATION ${DIST_DATADIR})
	install(DIRECTORY ${OPENVIBE_SDK_PATH}/bin/
		USE_SOURCE_PERMISSIONS
		DESTINATION ${DIST_BINDIR}
		FILES_MATCHING PATTERN "*example*")
endif()

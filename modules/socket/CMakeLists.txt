project(openvibe-module-socket VERSION ${OPENVIBE_MAJOR_VERSION}.${OPENVIBE_MINOR_VERSION}.${OPENVIBE_PATCH_VERSION})

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h src/*.hpp src/*.inl include/*.h include/*.hpp)

include("FindSourceRCProperties")

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
                      openvibe-common)

target_include_directories(${PROJECT_NAME}
                           PRIVATE include/socket
                           PUBLIC  include)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      VERSION ${PROJECT_VERSION}
                      SOVERSION ${PROJECT_VERSION_MAJOR}
                      FOLDER ${MODULES_FOLDER}
)

target_compile_options(${PROJECT_NAME}
                       PRIVATE -DSocket_Exports
                       PUBLIC -DSocket_Shared 
)

# ---------------------------------
# Finds standard library winsock
# Adds library to target
# Adds include path
# ---------------------------------
if(WIN32)
    include("OvSetWindowsSDKPath")
    find_library(LIB_STANDARD_MODULE_WINSOCK ws2_32 ${OV_MS_SDK_PATH}/lib)
    if(LIB_STANDARD_MODULE_WINSOCK)
        message(STATUS "  Found ws2_32...")
        target_link_libraries(${PROJECT_NAME} ${LIB_STANDARD_MODULE_WINSOCK})
    else(LIB_STANDARD_MODULE_WINSOCK)
        message(STATUS "  FAILED to find ws2_32...")
    endif(LIB_STANDARD_MODULE_WINSOCK)
endif(WIN32)

# ---------------------------------
# Target macros
# Defines target operating system, architecture and compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${DIST_BINDIR}
        LIBRARY DESTINATION ${DIST_LIBDIR}
        ARCHIVE DESTINATION ${DIST_LIBDIR})

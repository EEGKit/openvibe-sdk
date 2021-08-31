project(openvibe-module-date VERSION ${OPENVIBE_MAJOR_VERSION}.${OPENVIBE_MINOR_VERSION}.${OPENVIBE_PATCH_VERSION})

set(SRC_FILES
    include/date/defines.h
    include/date/DateParser.h
    src/DateParser.cpp
    )

if(WIN32)
    list(APPEND SRC_FILES
        "include/date/WindowsDateParser.h"
        "src/WindowsDateParser.cpp"
        )
endif()

include("FindSourceRCProperties")

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
                      openvibe-common)

target_include_directories(${PROJECT_NAME}
                           PRIVATE include/date
                           PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    FOLDER ${MODULES_FOLDER}
)

target_compile_options(${PROJECT_NAME}
                       PRIVATE -DDate_Exports
                       PUBLIC -DDate_Shared 
)

include("FindThirdPartyBoost")
include("FindThirdPartyExpat")

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

INSTALL(DIRECTORY include/ DESTINATION ${DIST_INCLUDEDIR} COMPONENT headers FILES_MATCHING PATTERN "*.h")

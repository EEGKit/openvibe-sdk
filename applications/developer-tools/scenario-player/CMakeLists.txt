PROJECT(certivibe-scenario-player)

# ----------------------
# Project variables
# ----------------------
SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# ----------------------
# External dependecies
# ----------------------
INCLUDE("FindThirdPartyBoost")

# ----------------------
# Add executable
# ----------------------
ADD_DEFINITIONS( -DPROJECT_VERSION="${PROJECT_VERSION}" )
FILE(GLOB_RECURSE source_files src/*.cpp src/*.h src/*.hpp)
ADD_EXECUTABLE(${PROJECT_NAME} ${source_files})

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# ----------------------
# Generate launch script
# ----------------------
OV_INSTALL_LAUNCH_SCRIPT(SCRIPT_PREFIX "cv-scenario-player" EXECUTABLE_NAME "${PROJECT_NAME}" PAUSE)

# -----------------------------
# Install files
# -----------------------------
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

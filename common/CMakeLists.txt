PROJECT(openvibe-common)

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

FILE(GLOB_RECURSE SRC_FILES src/*.h src/*.cpp include/*.h include/*.hpp)
INCLUDE_DIRECTORIES(include)

# A headers-only project, the executable is a NOP having some static asserts, not to be installed.
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})
INCLUDE("FindThirdPartyBoost")
	
# ---------------------------------
# Target macros
# Defines target operating system, architecture and compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------

INSTALL(DIRECTORY include/ DESTINATION ${DIST_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "doc" EXCLUDE)

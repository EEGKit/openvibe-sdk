PROJECT(openvibe-stimulation-generator)

SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

FILE(GLOB_RECURSE SRC_FILES src/*.cpp src/*.h src/*.inl)
ADD_EXECUTABLE(openvibe-stimulation-generator ${SRC_FILES})

# if(UNIX)
# SET(stim_generator_path  "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "stim_generator_path")
# else()
# get_property(path_to_target TARGET openvibe-stimulation-generator PROPERTY LOCATION)
# SET(stim_generator_path  "${path_to_target}" CACHE INTERNAL "stim_generator_path")
# MESSAGE(STATUS "MOO ${stim_generator_path}")
#endif()

# ---------------------------------
# Target macros
# Defines target operating system, architecture and compiler
# ---------------------------------
SET_BUILD_PLATFORM()

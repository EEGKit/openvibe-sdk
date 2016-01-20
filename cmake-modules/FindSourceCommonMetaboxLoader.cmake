# ---------------------------------
# Finds Scenario IO sources
# Appendsscenario_io_source_files to source_files
# Adds appropriate include dir
# ---------------------------------


MESSAGE(STATUS "Including Metabox Loader from ${OV_SOURCE_DEPENDENCIES_PATH}/metabox-loader")

FILE(GLOB_RECURSE metabox_loader_source_files ${OV_SOURCE_DEPENDENCIES_PATH}/metabox-loader/*.cpp ${OV_SOURCE_DEPENDENCIES_PATH}/metabox-loader/*.h)
INCLUDE_DIRECTORIES("${OV_SOURCE_DEPENDENCIES_PATH}/metabox-loader")
SET(source_files "${source_files};${metabox_loader_source_files}")

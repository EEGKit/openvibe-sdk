# ---------------------------------
# Finds Lepton sources
# Sets lepton_source_files
# Adds appropriate include dir
# ---------------------------------

FILE(GLOB_RECURSE lepton_source_files ${OV_SOURCE_DEPENDENCIES_PATH}/lepton/*.cpp ${OV_SOURCE_DEPENDENCIES_PATH}/lepton/*.h)
ADD_DEFINITIONS(-DTARGET_HAS_LEPTON)
INCLUDE_DIRECTORIES("${OV_SOURCE_DEPENDENCIES_PATH}")
SET(source_files "${source_files};${lepton_source_files}")

#######################################################################
# Software License Agreement : GNU Affero General Public License v3.0
# https://choosealicense.com/licenses/agpl-3.0/ 
#######################################################################

PROJECT(openvibe-test)

FILE(GLOB_RECURSE SRC_FILES *.cpp *.hpp)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})

SET_BUILD_PLATFORM()							# Default build Platform
SET_GTEST_PROJECT_CONFIG()						# Default Google Test Config
INCLUDE("FindOpenViBE")							# Find OpenViBE base includes and library

ADD_TEST(NAME base COMMAND ${PROJECT_NAME})
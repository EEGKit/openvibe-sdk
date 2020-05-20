#######################################################################
# Software License Agreement : GNU Affero General Public License v3.0
# https://choosealicense.com/licenses/agpl-3.0/ 
#######################################################################

PROJECT(ov-base-test)

FILE(GLOB_RECURSE SRC_FILES *.cpp *.hpp)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})

SET_BUILD_PLATFORM()	# Default build Platform
INCLUDE("FindOpenViBE")											# Find OpenViBE base includes and library
INCLUDE("FindOpenViBECommon")									# Find OpenViBE Common includes and library
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${OVT_UNIT_TESTS_FOLDER})	# Place project in folder unit-test (for some IDE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})	# Add google Test

ADD_TEST(NAME base COMMAND ${PROJECT_NAME})

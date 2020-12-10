#######################################################################
# Software License Agreement (AGPL-3 License)
#
# OpenViBE SDK Test Software
# Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
# Copyright (C) Inria, 2015-2017,V1.0
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License version 3,
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.
# If not, see <http://www.gnu.org/licenses/>.
#######################################################################

# Base framework features tests
PROJECT(openvibe-toolkit-test)

# ---------------------------------
# Target macros
# Defines target operating system, architecture and compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# ----------------------
# Configure test target
# ----------------------

# Test that needs to called with parameters
SET(TEST_WITH_PARAM
	uoMatrixToolkitTest.cpp
	urImportScenarioFromFileTest.cpp
	urExportScenarioToFileTest.cpp
	urValidateScenarioTest.cpp
)

# Test that needs to called without parameters
SET(TEST_NO_PARAM
)

# Create test sources list
# This macro auto-generate ${PROJECT_NAME}.cpp
# in the build tree. ${PROJECT_NAME}.cpp is
# the test driver called by ctest to run
# the different tests added to this target.
CREATE_TEST_SOURCELIST (Tests
	${PROJECT_NAME}.cpp
	${TEST_WITH_PARAM}
	${TEST_NO_PARAM}
	)

ADD_EXECUTABLE(${PROJECT_NAME} ${Tests})
INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEToolkit")
INCLUDE("FindOpenViBEModuleSystem")
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${TESTS_FOLDER})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OVT_UNIT_TOOLKIT_LIB} ${GTEST_BOTH_LIBRARIES} ${OV_LIBS})

# Add test without parameter to driver
FOREACH(test ${TEST_NO_PARAM})
	GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
	ADD_TEST(NAME ${TName} COMMAND ${PROJECT_NAME} ${TName})
ENDFOREACH()

# Add test with parameter to driver
ADD_TEST(NAME uoMatrixToolkitTest COMMAND ${PROJECT_NAME} uoMatrixToolkitTest "${OVT_TEST_TEMPORARY_DIR}")
ADD_TEST(NAME urImportScenarioFromFileTest COMMAND ${PROJECT_NAME} urImportScenarioFromFileTest "${OVT_OPENVIBE_DATA}/kernel/openvibe.conf" "${CMAKE_CURRENT_SOURCE_DIR}/data/")
ADD_TEST(NAME urExportScenarioToFileTest COMMAND ${PROJECT_NAME} urExportScenarioToFileTest "${OVT_OPENVIBE_DATA}/kernel/openvibe.conf" "${OVT_TEST_TEMPORARY_DIR}")
ADD_TEST(NAME urValidateScenarioTest COMMAND ${PROJECT_NAME} urValidateScenarioTest "${OVT_OPENVIBE_DATA}/kernel/openvibe.conf" "${CMAKE_CURRENT_SOURCE_DIR}/data/")


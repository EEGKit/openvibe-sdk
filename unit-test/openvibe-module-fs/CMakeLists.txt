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
PROJECT(openvibe-module-fs-test-utf)
SET_BUILD_PLATFORM()

ADD_EXECUTABLE(${PROJECT_NAME} uoFilesTestUTF.cpp)

INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEModuleFS")
INCLUDE("FindThirdPartyBoost")
#INCLUDE("FindThirdPartyBoost_System")
INCLUDE("FindThirdPartyBoost_FileSystem")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})

ADD_DEFINITIONS("-DDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
ADD_DEFINITIONS("-DTMP_DIR=\"${OVT_TEST_TEMPORARY_DIR}\"")
ADD_TEST(NAME uoFSTestUTF COMMAND ${PROJECT_NAME})

PROJECT(openvibe-module-fs-test)

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# ----------------------
# Configure test target
# ----------------------

# Test that needs to called with parameters
SET(TEST_WITH_PARAM
	uoEntryEnumeratorTest.cpp
	uoFilesTest.cpp
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
INCLUDE("FindOpenViBEModuleFS")
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${OVT_UNIT_TESTS_FOLDER})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OVT_UNIT_TOOLKIT_LIB} ${OV_MODULE_FS})

# Add test without parameter to driver 
FOREACH(test ${TEST_NO_PARAM})
	GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
	ADD_TEST(NAME ${TName} COMMAND ${PROJECT_NAME} ${TName})
ENDFOREACH()

# Add test with parameter to driver
ADD_TEST(NAME uoEntryEnumeratorTest COMMAND ${PROJECT_NAME} uoEntryEnumeratorTest "${CMAKE_CURRENT_SOURCE_DIR}/data/")
ADD_TEST(NAME uoFilesTest COMMAND ${PROJECT_NAME} uoFilesTest "${OVT_TEST_TEMPORARY_DIR}")

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

PROJECT(openvibe-module-socket-test)

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
 uoSocketClientServerBaseTest.cpp
 uoSocketClientServerSyncCommunicationTest.cpp
 uoSocketClientServerASyncCommunicationTest.cpp
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
INCLUDE("FindOpenViBEModuleSocket")
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${TESTS_FOLDER})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OVT_UNIT_TOOLKIT_LIB} ${OV_MODULE_SOCKET})

# Add test without parameter to driver 
FOREACH(test ${TEST_NO_PARAM})
	GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
	ADD_TEST(NAME ${TName} COMMAND ${PROJECT_NAME} ${TName})
ENDFOREACH()

# Add test with parameter to driver
SET(LOCALHOST "127.0.0.1")
SET(TEST_PORT "1024")
ADD_TEST(NAME uoSocketClientServerBaseTest COMMAND ${PROJECT_NAME} uoSocketClientServerBaseTest ${LOCALHOST} ${TEST_PORT})
ADD_TEST(NAME uoSocketClientServerHostNameTest COMMAND ${PROJECT_NAME} uoSocketClientServerBaseTest "localhost" ${TEST_PORT})
ADD_TEST(NAME uoSocketClientServerSyncCommunicationTest COMMAND ${PROJECT_NAME} uoSocketClientServerSyncCommunicationTest ${LOCALHOST} ${TEST_PORT} "1000")
ADD_TEST(NAME uoSocketClientServerASyncCommunicationTest COMMAND ${PROJECT_NAME} uoSocketClientServerASyncCommunicationTest ${LOCALHOST} ${TEST_PORT} "1000")

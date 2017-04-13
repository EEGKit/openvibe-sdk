#######################################################################
# Software License Agreement (AGPL-3 License)
#
# CertiViBE Test Software
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

PROJECT(openvibe-module-csv-test)

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
	uoCSVWriterTest.cpp
	uoCSVReaderTest.cpp
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
INCLUDE("FindOpenViBEModuleCSV")

SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${OVT_UNIT_TESTS_FOLDER})
# set(OV_MODULE_CSV "C:/gitdir/build/certivibe-debug/modules/csv/openvibe-module-csv")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OVT_UNIT_TOOLKIT_LIB} ${GTEST_BOTH_LIBRARIES} ${OV_LIBS} ${OV_MODULE_CSV})

# Add test without parameter to driver
FOREACH(test ${TEST_NO_PARAM})
	GET_FILENAME_COMPONENT(TName ${test} NAME_WE)
	ADD_TEST(NAME ${TName} COMMAND ${PROJECT_NAME} ${TName})
ENDFOREACH()

# Add test with parameter to driver
ADD_TEST(NAME uoCSVWriterTest COMMAND ${PROJECT_NAME} uoCSVWriterTest "${OVT_TEST_TEMPORARY_DIR}")
ADD_TEST(NAME uoCSVReaderTest COMMAND ${PROJECT_NAME} uoCSVReaderTest "${CMAKE_CURRENT_SOURCE_DIR}/data/")
#ADD_TEST(NAME uoCSVWriterTest COMMAND ${PROJECT_NAME} uoCSVWriter)
#ADD_TEST(NAME uoCSVReaderTest COMMAND ${PROJECT_NAME} uoCSVReader)
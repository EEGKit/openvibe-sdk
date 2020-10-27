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

FUNCTION(SETUP_TEST)
	SET_BUILD_PLATFORM()

	INCLUDE("FindOpenViBE")
	INCLUDE("FindOpenViBECommon")
	INCLUDE("FindOpenViBEModuleXML")
	INCLUDE("FindOpenViBEModuleFS")
	INCLUDE("FindThirdPartyBoost")
	INCLUDE("FindThirdPartyBoost_FileSystem")
	
	SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${TESTS_FOLDER})	# Place project in folder unit-test (for some IDE)

	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})

	# Add test with parameter to driver
	ADD_DEFINITIONS("-DDATA_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")
	ADD_DEFINITIONS("-DTEMP_DIR=\"${OVT_TEST_TEMPORARY_DIR}\"")
ENDFUNCTION()

PROJECT(openvibe-module-xml-reader-test)
ADD_EXECUTABLE(${PROJECT_NAME} uoXMLReaderTest.cpp)
SETUP_TEST()
ADD_TEST(NAME uoXMLReaderTest COMMAND ${PROJECT_NAME})

PROJECT(openvibe-module-xml-writer-test)
ADD_EXECUTABLE(${PROJECT_NAME} uoXMLWriterTest.cpp)
SETUP_TEST()
ADD_TEST(NAME uoXMLWriterTest COMMAND ${PROJECT_NAME})

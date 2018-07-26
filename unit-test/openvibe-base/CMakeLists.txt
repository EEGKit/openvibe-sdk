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

	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${GTEST_BOTH_LIBRARIES})

	INCLUDE("FindOpenViBE")
	INCLUDE("FindOpenViBECommon")
ENDFUNCTION()

# Base framework features tests
PROJECT(openvibe-base-time-arithmetic-test)
ADD_EXECUTABLE(${PROJECT_NAME} uoTimeArithmeticTest.cpp)
SETUP_TEST()
ADD_TEST(NAME uoTimeArithmeticTest COMMAND ${PROJECT_NAME})

# Tests for the CMatrix class
PROJECT(openvibe-base-cmatrix-test)
ADD_EXECUTABLE(${PROJECT_NAME} uoCMatrixTest.cpp)
SETUP_TEST()
ADD_TEST(NAME uoCMatrixTest COMMAND ${PROJECT_NAME})


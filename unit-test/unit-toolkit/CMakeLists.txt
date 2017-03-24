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

PROJECT(openvibe-test-unit-toolkit)

# ------------------
# Project variables
# ------------------

SET(PROJECT_VERSION_MAJOR ${OVT_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OVT_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OVT_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------

SET_BUILD_PLATFORM()

# ----------------------
# Generate target
# ----------------------

ADD_DEFINITIONS( -DPROJECT_VERSION="${PROJECT_VERSION}" )
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
FILE(GLOB_RECURSE source_files src/*.cpp include/*.h)
ADD_LIBRARY(${PROJECT_NAME} ${source_files})
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER ${OVT_TOOLS_FOLDER})

INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEToolkit")

# -------------
# External Use
# -------------

# Target name used by other target for linking purpose
SET(OVT_UNIT_TOOLKIT_LIB ${PROJECT_NAME} PARENT_SCOPE)
SET(OVT_UNIT_TOOLKIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)


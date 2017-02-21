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

PROJECT(openvibe-scenario-player)

# ----------------------
# Project variables
# ----------------------
SET(PROJECT_VERSION_MAJOR ${OV_GLOBAL_VERSION_MAJOR})
SET(PROJECT_VERSION_MINOR ${OV_GLOBAL_VERSION_MINOR})
SET(PROJECT_VERSION_PATCH ${OV_GLOBAL_VERSION_PATCH})
SET(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# ----------------------
# Add executable
# ----------------------
ADD_DEFINITIONS( -DPROJECT_VERSION="${PROJECT_VERSION}" )
FILE(GLOB_RECURSE source_files src/*.cpp src/*.h src/*.hpp)
ADD_EXECUTABLE(${PROJECT_NAME} ${source_files})

# ----------------------
# External dependecies 
# ----------------------

# To be done after add executable because FindOpenViBEXXX files need a target name
INCLUDE("FindOpenViBE")
INCLUDE("FindOpenViBECommon")
INCLUDE("FindOpenViBEToolkit")
INCLUDE("FindOpenViBEModuleSystem")
INCLUDE("FindThirdPartyBoost")

# ---------------------------------
# Target macros
# Defines target operating system
# Defines target architecture
# Defines target compiler
# ---------------------------------
SET_BUILD_PLATFORM()

# -----------------------------
# Install files
# -----------------------------
INSTALL(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION ${DIST_BINDIR}
	LIBRARY DESTINATION ${DIST_LIBDIR}
	ARCHIVE DESTINATION ${DIST_LIBDIR})

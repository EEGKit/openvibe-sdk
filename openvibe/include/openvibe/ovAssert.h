/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

/**
 * \author Charles Garraud (Inria)
 * \date 2016-07-20
 * \brief Utility macros used to wrap framework behavior in response to:
 *        * Warning
 *        * Error
 *        * Fatal error
 */

/**
 * \brief Use this macro to trigger a warning
 * \param message the warning message
 *
 * A warning should be triggered to warn the api consumer about an
 * expected and important event that is not a fault.
 */
#define OV_WARNING(message) \
do { \
	this->getLogManager() << LogLevel_Warning << (message) << "\n"; \
} \
while (0)

/**
 * \brief Use this macro to trigger a warning conditionnally
 * \param expression the boolean condition to assess
 * \param message the warning message
 */
#define OV_WARNING_UNLESS(expression, message) \
do { \
	if (!(expression)) \
	{ \
		OV_WARNING((message)); \
	} \
} \
while (0)

/**
 * \brief Use this macro to trigger an error conditionnally
 * \param expression the boolean condition to assess
 * \param description the error description
 * \param type the error type (see ovErrorType.h)
 * \param returnValue the value to return if an error is to be triggered
 *
 * An error should be triggered when a failure occurs. A failure is a
 * faulty expected event that alter the correct behavior of the framework.
 *
 * Most OpenViBE API functions communicates a failure occurrence through
 * their return value (false boolean, null pointer, bad identifier...).
 * Therefore, this macro takes a return value that will be returned in case
 * the assessed condition is not met.
 */
#define OV_ERROR_UNLESS(expression, description, type, returnValue) \
do { \
	if (!(expression)) \
	{ \
		this->getLogManager() << LogLevel_Error \
							  << "[Error description] = " \
							  << (description) \
							  << " / [Error code] = " \
							  << static_cast<unsigned int>((type)) \
							  << "\n"; \
		this->getErrorManager().pushErrorAtLocation(type, description, __FILE__, __LINE__); \
		return returnValue; \
	} \
} \
while (0)

/**
 * \brief Shorthand for OV_ERROR_UNLESS with false as return value
 */
#define OV_ERROR_UNLESS_BOOL(expression, description, type) OV_ERROR_UNLESS(expression, description, type, false)

/**
 * \brief Use this macro to terminate the program on fatal error.
 * \param expression the boolean condition to assess
 * \param message the fatal message
 *
 * A fatal event should be triggered to when a failures that cannot be
 * handled smoothly occurs.
 */
#define OV_FATAL_UNLESS(expression, description, type) \
do { \
		this->getLogManager() << LogLevel_Fatal \
							  << "[Error description] = " \
							  << (description) \
							  << " / [Error code] = " \
							  << static_cast<unsigned int>((type)) \
							  << "\n"; \
		std::abort(); \
} \
while (0)


/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
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

#include "../ovIKernelObject.h"
#include "ovErrorType.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IError;

		/**
		 * \class IErrorManager
		 * \author Charles Garraud (Inria)
		 * \date 2016-07-12
		 * \brief Interface for Kernel error handler implementation
		 * \ingroup Group_Error
		 * \ingroup Group_Kernel
		 *
		 * This manager is reponsible for handling errors in the framework.
		 * Errors in the framework are considered as not acceptable behavior
		 * that can be detected and handled by the system. The concept of error
		 * is thus independant of warning or fatal crashes that must be handled
		 * separately.
		 */
		class OV_API IErrorManager : public IKernelObject
		{
		public:

			/**
			 * \brief Push error to the manager
			 * \param type the error type
			 * \param description a self-explanatory description message
			 *
			 * Errors already added to the manager will be nested in the
			 * newly added error.
			 */
			virtual void pushError(ErrorType type, const char* description) = 0;

			/**
			 * \brief Push error with location information to the manager
			 * \param type the error type
			 * \param description a self-explanatory description of the error
			 * \param filename the source file where the error was detected
			 * \param line the line number where the error was detected
			 *
			 * Errors already added to the manager will be nested in the
			 * newly added error.
			 */
			virtual void pushErrorAtLocation(ErrorType type, const char* description, const char* filename, unsigned int line) = 0;

			/**
			 * \brief Release manager errors
			 *
			 * Release last error added to the manager and potentially
			 * all the nested errors recursively.
			 *
			 * \warning After this call, do not use pointers to IError retrieved
			 *          before. It will lead to unexpected behavior mostly due
			 *          to dangling pointers.
			 */
			virtual void releaseErrors() = 0;

			/**
			 * \brief Check for existing errors in the manager
			 * \return true if manager contains errors, false otherwise
			 */
			virtual bool hasError() const = 0;

			/**
			 * \brief Get last error added to the manager
			 * \return the error if manager contains errors, nullptr otherwise
			 *
			 * \warning The manager keeps the ownership of IError instances.
			 *          Therefore the API consumer is not responsible for their
			 *          life cycle/management and must use releaseErrors to
			 *          release them.
			 */
			virtual const IError* getLastError() const = 0;

			/**
			 * \brief Get description of last error added to the manager
			 * \return the description if manager contains errors, empty string otherwise
			 */
			virtual const char* getLastErrorString() const = 0;

			/**
			 * \brief Get type of last error added to the manager
			 * \return the type if manager contains error, NoErrorFound otherwise
			 */
			virtual ErrorType getLastErrorType() const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Error_ErrorManager)
		};
	}
}

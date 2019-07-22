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

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IError
		 * \author Charles Garraud (Inria)
		 * \date 2016-07-12
		 * \brief Interface for Kernel error implementation
		 * \ingroup Group_Error
		 * \ingroup Group_Kernel
		 *
		 * IError interface aims at providing information about an error
		 * occurring in the framework. One specific concept here is the
		 * notion of nested errors. In a call stack, it can be
		 * interesting to catch a n-1 level error and enhance it instead
		 * of rethrowing it directy unchanged. In this case, we say
		 * the level n-1 error is nested into the level n error.
		 */
		class OV_API IError : public IKernelObject
		{
		public:

			IError() {}
			virtual ~IError() {}
			IError(const IError&)            = delete;
			IError& operator=(const IError&) = delete;

			/**
			 * \brief Retrieve error description
			 */
			virtual const char* getErrorString() const = 0;

			/**
			 * \brief Retrieve error location
			 * \return the error location with file:line format
			 */
			virtual const char* getErrorLocation() const = 0;

			/**
			 * \brief Retrieve error type
			 */
			virtual ErrorType getErrorType() const = 0;

			/**
			 * \brief Retrieve nested error
			 * \return the nested error if there is one, nullptr otherwise
			 *
			 * \warning The error keeps the ownership of the nested error.
			 *          Therefore the API consumer is not responsible for its
			 *          life cycle/management
			 */
			virtual const IError* getNestedError() const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Error_Error)
		};
	}
}

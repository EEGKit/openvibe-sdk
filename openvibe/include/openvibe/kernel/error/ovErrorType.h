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

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class ErrorType
		 * \author Charles Garraud (Inria)
		 * \date 2016-07-13
		 * \brief List of error types
		 * \ingroup Group_Error
		 * \ingroup Group_Kernel
		 */
		enum class ErrorType
		{
			NoErrorFound = 0, /*!< Used when no error occurs */
			Unknown, /*!< Used when error type is unknown */
			Internal, /*!< Used when no detail about the error type should be communicated */
			OutOfBound, /*!< Used when a container index variable is set to an out-of-range value */
			Overflow, /*!< Used to report arithmetic overflow */
			ResourceNotFound, /*!< Used to report that a resource (file, plugin etc.) was not found */
			BadArguments, /*!< Used when a function is fed with invalid arguments */
			BadCast, /*!< Used when a cast failed */
			BadAlloc, /*!< Used when an allocation failed */
			BadFileRead, /*!< Used when reading from a file failed */
			BadFileWrite, /*!< Used when writing to a file failed */
			BadNetworkConnection /*!< Used when network connection failed */
		};
	}
}

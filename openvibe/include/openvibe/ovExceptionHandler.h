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

#include <type_traits>
#include <exception>
#include <functional>
#include <stdexcept>

namespace OpenViBE
{
	using ExceptionHandlerType = std::function<void(const std::exception&)>;
	
	/**
	  * \brief Invokes code and potentially translates exceptions to boolean
	  * 
	  * \tparam Callback callable type (e.g. functor) with Callback() returning boolean
	  * 
	  * \param callable code that must be guarded against exceptions
	  * \param exceptionHandler callback that handles the exception
	  * \return false either if callable() returns false or an exception 
	  * 		occurs, true otherwise
	  * 
	  * \details This method is a specific exception-to-boolean translation
	  * 		 method. If an exception is caught, it is handled by calling
	  * 		 the provided exception handler.
	  */
	template <
		typename Callback,
		typename std::enable_if<std::is_same<bool, typename std::result_of<Callback()>::type>::value>::type* = nullptr
	>
	bool translateException(Callback&& callable, ExceptionHandlerType exceptionHandler)
	{
		try { return callable(); }
		catch (const std::exception& exception)
		{
			exceptionHandler(exception);
			return false;
		}
		catch (...)
		{
			exceptionHandler(std::runtime_error("unknown exception"));
			return false;
		}
	}
} // namespace OpenViBE

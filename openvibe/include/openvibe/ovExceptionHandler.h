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

#include <iostream>
#include <type_traits>
#include <exception>

namespace OpenViBE
{
	
	/**
	  * \brief Applies specific actions when an exception occurs
	  * \tparam T handler type
	  * 
	  * \param handler helper object used to handle exceptions
	  * \param errorHint additonal information
	  * \param exception the exception to handle (null for exceptions of unknown types)
	  * 
	  * \details This function should typically be specialized for specific handlers.
	  * 		 Typical handlers would be Kernel objects able to log information
	  * 		 and to provide details on the entity suffering the exception.
	  */
	template<typename T>
	void handleException(const T& handler, const char* errorHint = nullptr, const std::exception* exception = nullptr)
	{
		std::cerr << "  [handler address: " << &handler << "]\n";
		std::cerr << "  [hint: " << (errorHint ? errorHint : "no hint") << "]\n";
		std::cerr << "  [cause:"  << (exception ? exception->what() : "unknown") << "]\n";
	}
	
	
	/**
	  * \brief Invokes code and potentially translates exceptions to boolean
	  * 
	  * \tparam Callback callable type (e.g. functor) with Callback() returning boolean
	  * \tparam Handler exception handler type (\see handleException)
	  * 
	  * \param callable code that must be guarded against exceptions
	  * \param handler helper object used to handle exceptions
	  * \param errorHint additional information used if an error occurs
	  * \return false either if callable() returns false or an exception 
	  * 		occurs, true otherwise
	  * 
	  * \details This method is a specific exception-to-boolean translation
	  * 		 method. If an exception is caught, it is handled by calling
	  * 		 handleException.
	  */
	template <typename Callback,typename Handler>
	typename std::enable_if<std::is_same<bool, typename std::result_of<Callback()>::type >::value, bool>::type
	translateException(Callback&& callable, const Handler& handler, const char* errorHint)
	{
		try
		{
			return callable();
		}
		catch(const std::exception& exception)
		{
			handleException(handler, errorHint, &exception);
			return false;
		}
		catch(...)
		{
			handleException(handler, errorHint);
			return false;
   		}
	}
}

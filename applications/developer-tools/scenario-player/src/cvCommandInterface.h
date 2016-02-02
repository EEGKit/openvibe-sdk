/*********************************************************************
* Software License Agreement (AGPL-3 License)                        *
*                                                                    *
* CertiViBE                                                          *
* Based on OpenViBE V1.1.0, Copyright (C) INRIA, 2006-2015           *
* Copyright (C) INRIA, 2015-2017,V1.0                                *
*                                                                    *
* \author Charles Garraud (INRIA)                                    *
*                                                                    *
* This program is free software: you can redistribute it and/or      *
* modify it under the terms of the GNU Affero General Public License *
* as published by the Free Software Foundation, either version 3 of  *
* the License.                                                       *
*                                                                    *
* This program is distributed in the hope that it will be useful,    *
* but WITHOUT ANY WARRANTY; without even the implied warranty of     *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
* Affero General Public License for more details.                    *
*                                                                    *
* You should have received a copy of the GNU Affero General Public   *
* License along with this program.                                   *
* If not, see <http://www.gnu.org/licenses/>.                        *
*********************************************************************/

#pragma once

#include "cvsp_defines.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class CommandInterface
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Base abstract class for commands
	* \ingroup ScenarioPlayer
	*
	* A command is an object that encapsulates all necessary information to perform an action later one.
	* Typically, a command implementation should contain an implementation of CommandInterface interface,
	* a list of setX/getX/hasX for each property (hasX is here to check a property has been set or not).
	*
	*/
	class CommandInterface
	{

	public:

		CommandInterface() = default;
		virtual ~CommandInterface() = default;

		friend std::ostream& operator<< (std::ostream& os, const CommandInterface& cmd);

		/**
		* \brief Clear command internal state
		*/
		virtual void clear() = 0;

		/**
		* \brief Execute the command
		* \param[in] kernelFacade the kernel facade that gives access to kernel features 
		*/
		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const = 0;

	protected:

		// use of the non-virtual interface pattern to implement printing in the class hierarchy
		virtual void doPrint(std::ostream& os) const = 0;

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class
		CommandInterface(const CommandInterface&) = delete;
		CommandInterface &operator=(const CommandInterface&) = delete;
	};

	inline std::ostream& operator<<(std::ostream& os, const CommandInterface& cmd)
	{
		cmd.doPrint(os);
		return os;
	}
}
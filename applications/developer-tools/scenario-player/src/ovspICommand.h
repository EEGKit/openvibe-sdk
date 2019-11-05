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

#include "ovsp_defines.h"

namespace OpenViBE
{
	class KernelFacade;

	/**
	* \struct SCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Base abstract struct for commands
	* \ingroup ScenarioPlayer
	*
	* A command is an object that encapsulates all necessary information to perform an action later one.
	* Typically, a command implementation should contain an implementation of CommandInterface interface,
	* and a list of properties.
	*
	*/
	struct SCommand
	{
		SCommand()          = default;
		virtual ~SCommand() = default;

		friend std::ostream& operator<<(std::ostream& os, const SCommand& cmd);

		/**
		* \brief Execute the command
		* \param[in] kernelFacade the kernel facade that gives access to kernel features
		*/
		virtual EPlayerReturnCode execute(KernelFacade& kernelFacade) const = 0;

	protected:

		// use of the non-virtual interface pattern to implement printing in the class hierarchy
		virtual void doPrint(std::ostream& os) const = 0;

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class
		SCommand(const SCommand&)            = delete;
		SCommand& operator=(const SCommand&) = delete;
	};

	inline std::ostream& operator<<(std::ostream& os, const SCommand& cmd)
	{
		cmd.doPrint(os);
		return os;
	}
}	// namespace OpenViBE

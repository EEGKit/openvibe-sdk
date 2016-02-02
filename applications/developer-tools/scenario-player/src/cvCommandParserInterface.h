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

#include <vector>
#include <memory>

#include "cvsp_defines.h"

namespace CertiViBE
{
	class CommandInterface;

	/**
	* \class CommandParserInterface
	* \author cgarraud (INRIA)
	* \date 2016-01-27
	* \brief Base abstract class for command parser
	* \ingroup ScenarioPlayer
	*
	* Command parsers aim at parsing a list of commands from a specific input.
	*
	*/
	class CommandParserInterface
	{

	public:

		virtual ~CommandParserInterface() = default;
		CommandParserInterface() = default;


		/**
		* \brief Initialize parser
		*
		*/
		virtual void initialize() = 0;


		/**
		* \brief Unitialize parser
		*
		*/
		virtual void uninitialize() = 0;

		/**
		* \brief Retrieve the list of commands
		* \pre This method should be called after the parse() method
		*
		*/
		virtual std::vector<std::shared_ptr<CommandInterface>> getCommandList() const = 0;


		/**
		* \brief Retrieve the list of commands
		* \pre This method should be called after the initialize() method
		*
		*/
		virtual PlayerReturnCode parse() = 0;

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class
		CommandParserInterface(const CommandParserInterface&) = delete;
		CommandParserInterface &operator=(const CommandParserInterface&) = delete;
	};
}
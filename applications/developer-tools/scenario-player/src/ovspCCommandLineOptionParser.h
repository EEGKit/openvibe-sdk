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
#include "ovspICommandParser.h"

namespace OpenViBE
{
	/**
	* \class CommandLineOptionParser
	* \author cgarraud (INRIA)
	* \date 2016-01-27
	* \brief Parser implementation that parses command from command-line arguments
	* \ingroup ScenarioPlayer
	*
	* The current implementation retrieves the options from a ProgramOptions parser and
	* simply builds the commands from the parsed options.
	*
	*/
	class CommandLineOptionParser final : public ICommandParser
	{
	public:

		/**
		*
		* \brief Constructor
		* \param[in] parser Specific instantiation of ProgramOptions parser
		*
		*/
		explicit CommandLineOptionParser(ProgramOptionParser& parser);

		void initialize() override;
		void uninitialize() override;

		std::vector<std::shared_ptr<SCommand>> getCommandList() const override;

		EPlayerReturnCodes parse() override;

	private:

		ProgramOptionParser& m_parser;
		std::vector<std::shared_ptr<SCommand>> m_cmdList;
	};
}	// namespace OpenViBE

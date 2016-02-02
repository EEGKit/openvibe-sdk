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

#include "cvCommandParserInterface.h"
#include "cvsp_defines.h"

namespace CertiViBE
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
	class CommandLineOptionParser : public CommandParserInterface
	{

	public:

		/**
		*
		* \brief Constructor
		* \param[in] parser Specific instantiation of ProgramOptions parser
		*
		*/
		explicit CommandLineOptionParser(ProgramOptionParser& parser);

		virtual void initialize() override;

		virtual void uninitialize() override;

		virtual std::vector<std::shared_ptr<CommandInterface>> getCommandList() const override;

		virtual PlayerReturnCode parse() override;

	private:

		ProgramOptionParser& m_OptionParser;
		std::vector<std::shared_ptr<CommandInterface>> m_CommandList;
	};
}
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

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <map>

#include "ovspICommandParser.h"
#include "ovsp_defines.h"

namespace OpenViBE
{
	/**
	* \class CommandFileParser
	* \author cgarraud (INRIA)
	* \date 2016-01-27
	* \brief Parser implementation that parses command a file
	* \ingroup ScenarioPlayer
	*
	* The current implementation retrieves the list of commands from a file.
	* 
	* Input file format requirements:
	* - Command names are defined within brackets [InitCommand] on a single line
	* - Each command parameter is defined on a single line below the command name:
	*  + Single value parameters are defined as: parameterName: value
	*  + Simple list parameters are defined as: parameterName: {val1,val2,...}
	*  + Token list parameters are defined as: parameterName: { (token1:val1),(token2:val2) } (no ',' authorized in (token, value) pair)
	*  .
	* .
	*
	* \todo The implementation uses a lot of string utility functions that need more
	* testing
	*
	* \note Use of regex would simplify the implementation but boost::regex is not header-only and std::regex not implemented in gcc 4.8
	*
	*/
	class CommandFileParser final : public ICommandParser
	{
	public:

		/**
		*
		* \brief Constructor
		* \param[in] commandFile path to the command file
		*
		*/
		explicit CommandFileParser(const std::string& commandFile);

		void initialize() override;

		void uninitialize() override;

		std::vector<std::shared_ptr<ICommand>> getCommandList() const override;

		PlayerReturnCode parse() override;

	private:

		using CallbackType = std::function<PlayerReturnCode(std::vector<std::string>)>;
		using Token = std::pair<std::string, std::string>;

		static std::string trim(const std::string& str);
		static std::pair<std::string, std::string> tokenize(const std::string& str);
		static std::vector<std::string> split(const std::string& str, char delimiter);
		static bool toBool(const std::string& str);
		static std::vector<std::string> toList(const std::string& str);
		static std::vector<Token> toTokenList(const std::string& str);

		PlayerReturnCode flush(const std::string& sectionTag, const std::vector<std::string>& sectionContent);

		PlayerReturnCode initCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode resetCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode loadKernelCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode loadScenarioCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode setupScenarioCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode runScenarioCommandCb(const std::vector<std::string>& sectionContent);

		std::string m_CommandFile;
		std::vector<std::shared_ptr<ICommand>> m_CommandList;
		std::map<std::string, CallbackType> m_CallbackList;
	};
} // namespace OpenViBE

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

#include <iostream>

#include "cvSetupScenarioCommand.h"

namespace CertiViBE
{
	void SetupScenarioCommand::setScenarioName(const std::string& name)
	{
		m_ScenarioName = name;
		m_HasScenarioName = true;
	}

	std::string SetupScenarioCommand::getScenarioName() const
	{
		return m_ScenarioName;
	}

	bool SetupScenarioCommand::hasScenarioName() const
	{
		return m_HasScenarioName;
	}

	void SetupScenarioCommand::setResetList(const std::vector<std::string>& resetList)
	{
		m_ResetList = resetList;
		m_HasResetList = true;
	}

	std::vector<std::string> SetupScenarioCommand::getResetList() const
	{
		return m_ResetList;
	}

	bool SetupScenarioCommand::hasResetList() const
	{
		return m_HasResetList;
	}

	void SetupScenarioCommand::setTokenList(const std::vector<Token>& tokenList)
	{
		m_TokenList = tokenList;
		m_HasTokenList = true;
	}

	std::vector<SetupScenarioCommand::Token> SetupScenarioCommand::getTokenList() const
	{
		return m_TokenList;
	}

	bool SetupScenarioCommand::hasTokenList() const
	{
		return m_HasTokenList;
	}


	void SetupScenarioCommand::clear()
	{
		m_ScenarioName.clear();
		m_ResetList.clear();
		m_TokenList.clear();
		m_HasScenarioName = false;
		m_HasResetList = false;
		m_HasTokenList = false;
	}

	PlayerReturnCode SetupScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		// to be implemented
		
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;
		std::cout << "Execution not yet implemented" << std::endl;

		return PlayerReturnCode::Success;
	}

	void SetupScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: SetupScenarioCommand" << std::endl;

		os << "scenarioName: " << ((this->hasScenarioName()) ? m_ScenarioName : "not set") << std::endl;

		os << "resetList:";
		if (this->hasResetList())
		{
			for (auto& token : m_ResetList)
			{
				os << " " << token;
			}
		}
		else
		{
			os << " not set";
		}
		os << std::endl;

		os << "tokenList:";
		if (this->hasTokenList())
		{
			for (auto& token : m_TokenList)
			{
				os << " (" << token.first << "," << token.second << ")";
			}
		}
		else
		{
			os << " not set";
		}
		os << std::endl;

	}
}
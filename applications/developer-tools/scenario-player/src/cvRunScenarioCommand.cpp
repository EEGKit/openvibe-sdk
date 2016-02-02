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

#include "cvKernelFacade.h"
#include "cvRunScenarioCommand.h"

namespace CertiViBE
{

	void RunScenarioCommand::setScenarioList(const std::vector<std::string>& scenarioList)
	{
		m_ScenarioList = scenarioList;
		m_HasScenarioList = true;
	}

	std::vector<std::string> RunScenarioCommand::getScenarioList() const
	{
		return m_ScenarioList;
	}

	bool RunScenarioCommand::hasScenarioList() const
	{
		return m_HasScenarioList;
	}

	void RunScenarioCommand::setPlayMode(PlayMode playMode)
	{
		m_PlayMode= playMode;
		m_HasPlayMode = true;
	}

	PlayMode RunScenarioCommand::getPlayMode() const
	{
		return m_PlayMode;
	}

	bool RunScenarioCommand::hasPlayMode() const
	{
		return m_HasPlayMode;
	}

	void RunScenarioCommand::setMaximumExecutionTime(double timeout)
	{
		m_MaximumExecutionTime = timeout;
		m_HasMaximumExecutionTime = true;
	}

	double RunScenarioCommand::getMaximumExecutionTime() const
	{
		return m_MaximumExecutionTime;
	}

	bool RunScenarioCommand::hasMaximumExecutionTime() const
	{
		return m_HasMaximumExecutionTime;
	}

	void RunScenarioCommand::setResetList(const std::vector<std::string>& resetList)
	{
		m_ResetList = resetList;
		m_HasResetList = true;
	}

	std::vector<std::string> RunScenarioCommand::getResetList() const
	{
		return m_ResetList;
	}

	bool RunScenarioCommand::hasResetList() const
	{
		return m_HasResetList;
	}

	void RunScenarioCommand::setTokenList(const std::vector<Token>& tokenList)
	{
		m_TokenList = tokenList;
		m_HasTokenList = true;
	}

	std::vector<RunScenarioCommand::Token> RunScenarioCommand::getTokenList() const
	{
		return m_TokenList;
	}

	bool RunScenarioCommand::hasTokenList() const
	{
		return m_HasTokenList;
	}

	void RunScenarioCommand::clear()
	{
		m_ScenarioList.clear();
		m_PlayMode = PlayMode::Standard;
		m_MaximumExecutionTime = -1.0;
		m_ResetList.clear();
		m_TokenList.clear();

		m_HasScenarioList = false;
		m_HasPlayMode = false;
		m_HasMaximumExecutionTime = false;
		m_HasResetList = false;
		m_HasTokenList = false;
	}

	PlayerReturnCode RunScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;
		
		KernelFacade::RunParameters parameters;

		if (this->hasScenarioList())
		{
			parameters.m_ScenarioList = this->getScenarioList();
		}
		else
		{
			std::cerr << "Missing required arguments for command: ScenarioList" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		if (this->hasMaximumExecutionTime())
		{
			parameters.m_MaximumExecutionTime = this->getMaximumExecutionTime();
		}

		if (this->hasPlayMode())
		{
			parameters.m_PlayMode = this->getPlayMode();
		}

		if (this->hasResetList())
		{
			parameters.m_ResetList = this->getResetList();
		}

		if (this->hasTokenList())
		{
			parameters.m_TokenList = this->getTokenList();
		}

		return kernelFacade.runScenarioList(parameters);
	}

	void RunScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: RunScenarioCommand" << std::endl;

		os << "scenarioList:";
		if (this->hasScenarioList())
		{
			for (auto& scenario : m_ScenarioList)
			{
				os << " " << scenario;
			}
		}
		else
		{
			os << " not set";
		}
		os << std::endl;

		os << "playMode: ";
		if (this->hasPlayMode())
		{
			std::string modeAsString = (m_PlayMode == PlayMode::Fastfoward) ? "fastforward" : "standard";
			os << modeAsString;
		}
		else
		{
			os << "not set";
		}
		os << std::endl;


		os << "MaximumExecutionTime: " << ((this->hasMaximumExecutionTime()) ? std::to_string(m_MaximumExecutionTime) : "not set") << std::endl;

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
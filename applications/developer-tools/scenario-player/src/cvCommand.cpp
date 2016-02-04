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

#include "cvKernelFacade.h"
#include "cvCommand.h"

namespace CertiViBE
{
	PlayerReturnCode InitCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		return kernelFacade.initialize(*this);
	}

	void InitCommand::doPrint(std::ostream& os) const
	{

		os << "command name: InitCommand" << std::endl;
		os << "benchmark: " << ( m_Benchmark ? std::to_string(m_Benchmark.get()) : "not set") << std::endl;
	}

	PlayerReturnCode LoadKernelCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		// default config file is an empty one so it is not problem to give it directly as param
		return kernelFacade.loadKernel(*this);
	}

	void LoadKernelCommand::doPrint(std::ostream& os) const
	{
		os << "command name: LoadKernelCommand" << std::endl;
		os << "configFile: " << (m_ConfigurationFile ? m_ConfigurationFile.get() : "not set") << std::endl;
	}

	PlayerReturnCode LoadScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		if (!m_ScenarioName || !m_ScenarioFile)
		{
			std::cerr << "Missing required arguments for command" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		return kernelFacade.loadScenario(*this);
	}

	void LoadScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: LoadScenarioCommand" << std::endl;
		os << "scenarioName: " << (m_ScenarioName ? m_ScenarioName.get() : "not set") << std::endl;
		os << "scenarioFile: " << (m_ScenarioFile ? m_ScenarioFile.get() : "not set") << std::endl;
	}

	PlayerReturnCode ResetCommand::execute(KernelFacade& kernelFacade) const
	{
		// to be implemented

		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		std::cout << "Not implemented yet" << std::endl;

		return PlayerReturnCode::Success;
	}

	void ResetCommand::doPrint(std::ostream& os) const
	{
		os << "command name: ResetCommand" << std::endl;
	}

	PlayerReturnCode RunScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		if (!m_ScenarioList)
		{
			std::cerr << "Missing required arguments for command: ScenarioList" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		return kernelFacade.runScenarioList(*this);
	}

	void RunScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: RunScenarioCommand" << std::endl;

		os << "scenarioList:";
		if (m_ScenarioList)
		{
			for (auto& scenario : m_ScenarioList.get())
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
		if (m_PlayMode)
		{
			std::string modeAsString = (m_PlayMode == PlayMode::Fastfoward) ? "fastforward" : "standard";
			os << modeAsString;
		}
		else
		{
			os << "not set";
		}
		os << std::endl;


		os << "MaximumExecutionTime: " << ( m_MaximumExecutionTime ? std::to_string(m_MaximumExecutionTime.get()) : "not set") << std::endl;

		os << "resetList:";
		if (m_ResetList)
		{
			for (auto& token : m_ResetList.get())
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
		if (m_TokenList)
		{
			for (auto& token : m_TokenList.get())
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

		os << "scenarioName: " << (m_ScenarioName ? m_ScenarioName.get() : "not set") << std::endl;

		os << "resetList:";
		if (m_ResetList)
		{
			for (auto& token : m_ResetList.get())
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
		if (m_TokenList)
		{
			for (auto& token : m_TokenList.get())
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
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

#include "ovspCKernelFacade.h"
#include "ovspCCommand.h"

namespace OpenViBE
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
		os << "Benchmark: " << (this->benchmark ? std::to_string(this->benchmark.get()) : "not set") << std::endl;
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
		os << "ConfigurationFile: " << (this->configurationFile ? this->configurationFile.get() : "not set") << std::endl;
	}

	PlayerReturnCode LoadScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		if (!this->scenarioName || !this->scenarioFile)
		{
			std::cerr << "Missing required arguments for command" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		return kernelFacade.loadScenario(*this);
	}

	void LoadScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: LoadScenarioCommand" << std::endl;
		os << "ScenarioName: " << (this->scenarioName ? this->scenarioName.get() : "not set") << std::endl;
		os << "ScenarioFile: " << (this->scenarioFile ? this->scenarioFile.get() : "not set") << std::endl;
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

		if (!this->scenarioList)
		{
			std::cerr << "Missing required arguments for command: ScenarioList" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		return kernelFacade.runScenarioList(*this);
	}

	void RunScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: RunScenarioCommand" << std::endl;

		os << "ScenarioList:";
		if (this->scenarioList)
		{
			for (auto& scenario : this->scenarioList.get())
			{
				os << " " << scenario;
			}
		}
		else
		{
			os << " not set";
		}
		os << std::endl;

		os << "PlayMode: ";
		if (this->playMode)
		{
			std::string modeAsString = (this->playMode == PlayerPlayMode::Fastfoward) ? "fastforward" : "standard";
			os << modeAsString;
		}
		else
		{
			os << "not set";
		}
		os << std::endl;


		os << "MaximumExecutionTime: " << (this->maximumExecutionTime ? std::to_string(this->maximumExecutionTime.get()) : "not set") << std::endl;

		os << "TokenList:";
		if (this->tokenList)
		{
			for (auto& token : this->tokenList.get())
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

		os << "ScenarioName: " << (this->scenarioName ? this->scenarioName.get() : "not set") << std::endl;

		os << "TokenList:";
		if (this->tokenList)
		{
			for (auto& token : this->tokenList.get())
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
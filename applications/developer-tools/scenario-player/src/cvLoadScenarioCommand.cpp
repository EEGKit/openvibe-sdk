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
#include "cvLoadScenarioCommand.h"

namespace CertiViBE
{

	void LoadScenarioCommand::setScenarioFile(const std::string& file)
	{
		m_ScenarioFile = file;
		m_HasScenarioFile = true;
	}

	std::string LoadScenarioCommand::getScenarioFile() const
	{
		return m_ScenarioFile;
	}

	bool LoadScenarioCommand::hasScenarioFile() const
	{
		return m_HasScenarioFile;
	}

	
	void LoadScenarioCommand::setScenarioName(const std::string& name)
	{
		m_ScenarioName = name;
		m_HasScenarioName = true;
	}

	std::string LoadScenarioCommand::getScenarioName() const
	{
		return m_ScenarioName;
	}

	bool LoadScenarioCommand::hasScenarioName() const
	{
		return m_HasScenarioName;
	}

	
	void LoadScenarioCommand::clear()
	{
		m_ScenarioFile.clear();
		m_ScenarioName.clear();
		m_HasScenarioFile = false;
		m_HasScenarioName = false;
	}

	PlayerReturnCode LoadScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;
		
		if (!this->hasScenarioName() || !this->hasScenarioFile())
		{
			std::cerr << "Missing required arguments for command" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		return kernelFacade.loadScenario(this->getScenarioName(), this->getScenarioFile());
	}

	void LoadScenarioCommand::doPrint(std::ostream& os) const
	{
		os << "command name: LoadScenarioCommand" << std::endl;
		os << "scenarioName: " << ((this->hasScenarioName()) ? m_ScenarioName : "not set") << std::endl;
		os << "scenarioFile: " << ((this->hasScenarioFile()) ? m_ScenarioFile : "not set") << std::endl;
	}
}
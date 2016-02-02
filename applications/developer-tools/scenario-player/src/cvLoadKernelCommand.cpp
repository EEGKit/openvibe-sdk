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
#include "cvLoadKernelCommand.h"

namespace CertiViBE
{

	void LoadKernelCommand::setConfigurationFile(const std::string& file)
	{
		m_ConfigFile = file;
		m_HasConfigFile = true;
	}

	std::string LoadKernelCommand::getConfigurationFile() const
	{
		return m_ConfigFile;
	}

	bool LoadKernelCommand::hasConfigurationFile() const
	{
		return m_HasConfigFile;
	}

	void LoadKernelCommand::clear()
	{
		m_ConfigFile.clear();
		m_HasConfigFile = false;
	}

	PlayerReturnCode LoadKernelCommand::execute(KernelFacade& kernelFacade) const
	{
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		// default config file is an empty one so it is not problem to give it directly as param
		return kernelFacade.loadKernel(this->getConfigurationFile());
	}

	void LoadKernelCommand::doPrint(std::ostream& os) const
	{
		os << "command name: LoadKernelCommand" << std::endl;
		os << "configFile: " << ((this->hasConfigurationFile()) ? m_ConfigFile : "not set") << std::endl;
	}
}
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

#include <string>

#include "cvsp_defines.h"
#include "cvCommandInterface.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class LoadScenarioCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives scenario loading
	* \ingroup ScenarioPlayer
	*
	* LoadScenarioCommand contains the following properties:
	* - ScenarioFile: Path to xml scenario file (mandatory)
	* - ScenarioName: Name of the scenario (mandatory)
	* .
	*
	*/
	class LoadScenarioCommand : public CommandInterface
	{

	public:

		void setScenarioFile(const std::string& file);
		std::string getScenarioFile() const;
		bool hasScenarioFile() const;
		
		void setScenarioName(const std::string& name);
		std::string getScenarioName() const;
		bool hasScenarioName() const;

		
		virtual void clear() override;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;

	private:

		std::string m_ScenarioFile;
		std::string m_ScenarioName;
		bool m_HasScenarioFile{ false };
		bool m_HasScenarioName{ false };
	};
}
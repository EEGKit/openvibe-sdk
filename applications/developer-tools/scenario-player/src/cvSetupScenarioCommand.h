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
#include <vector>

#include "cvsp_defines.h"
#include "cvCommandInterface.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class SetupScenarioCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the setup of a scenario
	* \ingroup ScenarioPlayer
	*
	* SetupScenarioCommand contains the following properties:
	* - ScenarioName: name of the scenario to setup (mandatory)
	* - ResetList: List of token to reset (optional)
	* - TokenList: List of scenario specific (optional)
	* .
	*
	*/
	class SetupScenarioCommand : public CommandInterface
	{

	public:

		using Token = std::pair<std::string, std::string>;

		void setScenarioName(const std::string& name);
		std::string getScenarioName() const;
		bool hasScenarioName() const;

		void setResetList(const std::vector<std::string>& resetList);
		std::vector<std::string> getResetList() const;
		bool hasResetList() const;

		void setTokenList(const std::vector<Token>& tokenList);
		std::vector<Token> getTokenList() const;
		bool hasTokenList() const;

		virtual void clear() override;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;

	private:

		std::string m_ScenarioName;
		std::vector<std::string> m_ResetList;
		std::vector<Token> m_TokenList;

		bool m_HasScenarioName{ false };
		bool m_HasResetList{ false };
		bool m_HasTokenList{ false };
	};
}
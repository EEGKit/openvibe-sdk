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
	* \class RunScenarioCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the execution of a list of scenarios
	* \ingroup ScenarioPlayer
	*
	* RunScenarioCommand contains the following properties:
	* - ScenarioList: Names of scenario that must be executed (mandatory)
	* - PlayMode: 0 for standard, 1 for fastforward (optional)
	* - MaximumExecutionTime: Scenarios playing execution time limit (optional)
	* - ResetList: Names of scenario whose setup must be reset (optional)
	* - TokenList: List of global (token,value) pairs (optional)
	* .
	*
	*/
	class RunScenarioCommand : public CommandInterface
	{

	public:

		using Token = std::pair<std::string, std::string>;
		
		void setScenarioList(const std::vector<std::string>& scenarioList);
		std::vector<std::string> getScenarioList() const;
		bool hasScenarioList() const;
		
		void setPlayMode(PlayMode mode);
		PlayMode getPlayMode() const;
		bool hasPlayMode() const;

		void setMaximumExecutionTime(double timeout);
		double getMaximumExecutionTime() const;
		bool hasMaximumExecutionTime() const;

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

		std::vector<std::string> m_ScenarioList;
		std::vector<std::string> m_ResetList;
		std::vector<Token> m_TokenList;
		PlayMode m_PlayMode{ PlayMode::Standard };
		double m_MaximumExecutionTime{ -1.0 };

		bool m_HasScenarioList{ false };
		bool m_HasPlayMode{ false };
		bool m_HasMaximumExecutionTime{ false };
		bool m_HasResetList{ false };
		bool m_HasTokenList{ false };
	};
}
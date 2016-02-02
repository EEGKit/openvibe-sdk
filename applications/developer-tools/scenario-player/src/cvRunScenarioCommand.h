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
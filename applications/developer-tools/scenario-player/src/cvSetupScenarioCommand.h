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
	* - Name: name of the scenario to setup (required)
	* - ResetList: List of token to reset (optional)
	* - TokenList: List of scenario specific (token,value) pairs (optional)
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
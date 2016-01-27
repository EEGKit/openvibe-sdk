
#include <iostream>

#include "cvSetupScenarioCommand.h"

namespace CertiViBE
{
	void SetupScenarioCommand::setScenarioName(const std::string& name)
	{
		m_ScenarioName = name;
		m_HasScenarioName = true;
	}

	std::string SetupScenarioCommand::getScenarioName() const
	{
		return m_ScenarioName;
	}

	bool SetupScenarioCommand::hasScenarioName() const
	{
		return m_HasScenarioName;
	}

	void SetupScenarioCommand::setResetList(const std::vector<std::string>& resetList)
	{
		m_ResetList = resetList;
		m_HasResetList = true;
	}

	std::vector<std::string> SetupScenarioCommand::getResetList() const
	{
		return m_ResetList;
	}

	bool SetupScenarioCommand::hasResetList() const
	{
		return m_HasResetList;
	}

	void SetupScenarioCommand::setTokenList(const std::vector<Token>& tokenList)
	{
		m_TokenList = tokenList;
		m_HasTokenList = true;
	}

	std::vector<SetupScenarioCommand::Token> SetupScenarioCommand::getTokenList() const
	{
		return m_TokenList;
	}

	bool SetupScenarioCommand::hasTokenList() const
	{
		return m_HasTokenList;
	}


	void SetupScenarioCommand::clear()
	{
		m_ScenarioName.clear();
		m_ResetList.clear();
		m_TokenList.clear();
		m_HasScenarioName = false;
		m_HasResetList = false;
		m_HasTokenList = false;
	}

	PlayerReturnCode SetupScenarioCommand::execute(KernelFacade& kernelFacade) const
	{
		// to be implemented
		std::cout << "SetupScenarioCommand not yet implemented" << std::endl;

		return PlayerReturnCode::Sucess;
	}
}
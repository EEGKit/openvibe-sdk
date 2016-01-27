
#include <iostream>

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
		// to be implemented
		std::cout << "LoadScenarioCommand not yet implemented" << std::endl;

		return PlayerReturnCode::Sucess;
	}
}
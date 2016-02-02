
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
#pragma once

#include <string>

#include "cvsp_defines.h"
#include "cvCommandInterface.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class LoadKernelCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives kernel loading
	* \ingroup ScenarioPlayer
	*
	* LoadKernelCommand contains the following properties:
	* - ConfigurationFile: Path to kernel configuration file (optional)
	* .
	*
	*/
	class LoadKernelCommand : public CommandInterface
	{

	public:

		void setConfigurationFile(const std::string& file);
		std::string getConfigurationFile() const;
		bool hasConfigurationFile() const;

		virtual void clear() override;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;

	private:

		std::string m_ConfigFile;
		bool m_HasConfigFile{ false };
	};
}
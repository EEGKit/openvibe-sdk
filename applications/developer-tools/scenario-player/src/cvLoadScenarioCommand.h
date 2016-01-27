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
	* - ScenarioFile: Path to xml scenario file (required)
	* - ScenarioName: Name of the scenario (required)
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
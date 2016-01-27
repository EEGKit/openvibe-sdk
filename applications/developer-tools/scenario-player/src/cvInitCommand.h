#pragma once

#include "cvsp_defines.h"
#include "cvCommandInterface.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class InitCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the initialization of the tool
	* \ingroup ScenarioPlayer
	*
	* InitCommand class contains the following properties:
	* - Benchmark: Flag to enable benchmark on execute command (optional)
	* .
	*
	*/
	class InitCommand : public CommandInterface
	{

	public:

		void setBenchmark(bool flag);
		bool getBenchmark() const;
		bool hasBenchmark() const;

		virtual void clear() override;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	private:

		bool m_Benchmark{ false };
		bool m_HasBenchmark{ false };
	};
}
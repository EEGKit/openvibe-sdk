#pragma once

#include "cvsp_defines.h"
#include "cvCommandInterface.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class ResetCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives tool reset to its initial state
	* \ingroup ScenarioPlayer
	*/
	class ResetCommand : public CommandInterface
	{

	public:


		virtual void clear() override
		{
		}

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;
	};
}
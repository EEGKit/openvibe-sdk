
#include <iostream>

#include "cvKernelFacade.h"
#include "cvResetCommand.h"

namespace CertiViBE
{
	PlayerReturnCode ResetCommand::execute(KernelFacade& kernelFacade) const
	{
		// to be implemented

		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;

		std::cout << "Not implemented yet" << std::endl;
		
		return PlayerReturnCode::Success;
	}

	void ResetCommand::doPrint(std::ostream& os) const
	{
		os << "command name: ResetCommand" << std::endl;
	}
}
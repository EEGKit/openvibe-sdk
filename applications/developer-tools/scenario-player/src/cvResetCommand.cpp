
#include <iostream>

#include "cvResetCommand.h"

namespace CertiViBE
{
	PlayerReturnCode ResetCommand::execute(KernelFacade& kernelFacade) const
	{
		// to be implemented

		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;
		std::cout << "Execution not yet implemented" << std::endl;

		return PlayerReturnCode::Sucess;
	}

	void ResetCommand::doPrint(std::ostream& os) const
	{
		os << "command name: ResetCommand" << std::endl;
	}
}
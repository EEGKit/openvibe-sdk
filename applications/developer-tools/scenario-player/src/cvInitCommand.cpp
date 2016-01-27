
#include <iostream>

#include "cvInitCommand.h"

namespace CertiViBE
{
	
	void InitCommand::setBenchmark(bool flag)
	{
		m_Benchmark = flag;
		m_HasBenchmark = true;
	}

	bool InitCommand::getBenchmark() const
	{
		return m_Benchmark;
	}

	bool InitCommand::hasBenchmark() const
	{
		return m_HasBenchmark;
	}

	void InitCommand::clear()
	{
		m_HasBenchmark = false;
		m_Benchmark = false;
	}

	PlayerReturnCode InitCommand::execute(KernelFacade& kernelFacade) const
	{
		// to be implemented

		std::cout << "InitCommand not yet implemented" << std::endl;

		return PlayerReturnCode::Sucess;
	}
}
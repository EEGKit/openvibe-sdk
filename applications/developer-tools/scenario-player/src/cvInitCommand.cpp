
#include <iostream>
#include <string>

#include "cvKernelFacade.h"
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
		std::cout << "About to execute:" << std::endl;
		std::cout << *this << std::endl;
		
		return kernelFacade.initialize(this->getBenchmark());
	}

	void InitCommand::doPrint(std::ostream& os) const
	{

		os << "command name: InitCommand" << std::endl;
		os << "benchmark: " << ((this->hasBenchmark()) ? std::to_string(m_Benchmark) : "not set") << std::endl;
	}
}
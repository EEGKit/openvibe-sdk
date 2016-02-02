/*********************************************************************
* Software License Agreement (AGPL-3 License)                        *
*                                                                    *
* CertiViBE                                                          *
* Based on OpenViBE V1.1.0, Copyright (C) INRIA, 2006-2015           *
* Copyright (C) INRIA, 2015-2017,V1.0                                *
*                                                                    *
* \author Charles Garraud (INRIA)                                    *
*                                                                    *
* This program is free software: you can redistribute it and/or      *
* modify it under the terms of the GNU Affero General Public License *
* as published by the Free Software Foundation, either version 3 of  *
* the License.                                                       *
*                                                                    *
* This program is distributed in the hope that it will be useful,    *
* but WITHOUT ANY WARRANTY; without even the implied warranty of     *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
* Affero General Public License for more details.                    *
*                                                                    *
* You should have received a copy of the GNU Affero General Public   *
* License along with this program.                                   *
* If not, see <http://www.gnu.org/licenses/>.                        *
*********************************************************************/

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
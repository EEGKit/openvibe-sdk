/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>

#include "ovtKernelContext.h"

namespace OpenViBE {
namespace Test {
bool ctx::initialize()
{
#if defined TARGET_OS_Windows
	const CString kernelFile = Directories::getLibDir() + "/openvibe-kernel.dll";
#elif defined TARGET_OS_Linux
	const CString kernelFile = Directories::getLibDir() + "/libopenvibe-kernel.so";
#elif defined TARGET_OS_MacOS
	const CString kernelFile = Directories::getLibDir() + "/libopenvibe-kernel.dylib";
#endif
	CString error;

	if (!m_KernelLoader.load(kernelFile, &error))
	{
		std::cerr << "ERROR: impossible to load kernel from file located at: " << kernelFile << std::endl;
		std::cerr << "ERROR: kernel error: " << error << std::endl;
		return false;
	}

	m_KernelLoader.initialize();

	Kernel::IKernelDesc* kernelDesc{ nullptr };
	m_KernelLoader.getKernelDesc(kernelDesc);

	if (!kernelDesc)
	{
		std::cerr << "ERROR: impossible to retrieve kernel descriptor " << std::endl;
		return false;
	}

	const CString configFile = CString(Directories::getDataDir() + "/kernel/openvibe.conf");

	Kernel::IKernelContext* ctx = kernelDesc->createKernel("test-kernel", configFile);

	if (!ctx)
	{
		std::cerr << "ERROR: impossible to create kernel context " << std::endl;
		return false;
	}

	ctx->initialize();

	m_Context = ctx;

	return true;
}

bool ctx::uninitialize()
{
	if (m_Context)
	{
		Kernel::IKernelDesc* kernelDesc{ nullptr };
		m_KernelLoader.getKernelDesc(kernelDesc);
		kernelDesc->releaseKernel(m_Context);
		m_Context = nullptr;
	}

	m_KernelLoader.uninitialize();
	m_KernelLoader.unload();

	return true;
}
}  // namespace Test
}  // namespace OpenViBE

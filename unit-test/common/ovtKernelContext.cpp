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

namespace OpenViBETest
{
	using namespace OpenViBE;
	using namespace OpenViBE::Kernel;
	using namespace OpenViBE::Plugins;

	bool KernelContext::initialize()
	{
		CString kernelFile;

#if defined TARGET_OS_Windows
		kernelFile = OpenViBE::Directories::getLibDir() + "/openvibe-kernel.dll";
#elif defined TARGET_OS_Linux
		kernelFile = OpenViBE::Directories::getLibDir() + "/libopenvibe-kernel.so";
#elif defined TARGET_OS_MacOS
		kernelFile = OpenViBE::Directories::getLibDir() + "/libopenvibe-kernel.dylib";
#endif
		CString error;

		if (!m_KernelLoader.load(kernelFile, &error))
		{
			std::cerr << "ERROR: impossible to load kernel from file located at: " << kernelFile << std::endl;
			std::cerr << "ERROR: kernel error: " << error << std::endl;
			return false;
		}

		m_KernelLoader.initialize();

		IKernelDesc* kernelDesc{ nullptr };
		m_KernelLoader.getKernelDesc(kernelDesc);

		if (!kernelDesc)
		{
			std::cerr << "ERROR: impossible to retrieve kernel descriptor " << std::endl;
			return false;
		}

		CString configurationFile = CString(OpenViBE::Directories::getDataDir() + "/kernel/openvibe.conf");

		IKernelContext* kernelContext = kernelDesc->createKernel("test-kernel", configurationFile);

		if (!kernelContext)
		{
			std::cerr << "ERROR: impossible to create kernel context " << std::endl;
			return false;
		}

		kernelContext->initialize();

		m_Context = kernelContext;

		return true;
	}

	bool KernelContext::uninitialize()
	{
		if (m_Context)
		{
			IKernelDesc* kernelDesc{ nullptr };
			m_KernelLoader.getKernelDesc(kernelDesc);
			kernelDesc->releaseKernel(m_Context);
			m_Context = nullptr;
		}

		m_KernelLoader.uninitialize();
		m_KernelLoader.unload();

		return true;
	}
}

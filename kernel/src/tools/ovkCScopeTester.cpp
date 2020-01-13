#include "ovkCScopeTester.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Tools;

CScopeTester::CScopeTester(const IKernelContext& ctx, const CString& sPrefix)
	: m_sPrefix(sPrefix), m_kernelCtx(ctx) { m_kernelCtx.getLogManager() << LogLevel_Debug << "## CScopeTester [" << m_sPrefix << "] enter\n"; }

CScopeTester::~CScopeTester() { m_kernelCtx.getLogManager() << LogLevel_Debug << "## CScopeTester [" << m_sPrefix << "] leave\n"; }

#include "ovkCScopeTester.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Tools;

CScopeTester::CScopeTester(const IKernelContext& ctx, const CString& prefix)
	: m_prefix(prefix), m_kernelCtx(ctx) { m_kernelCtx.getLogManager() << LogLevel_Debug << "## CScopeTester [" << m_prefix << "] enter\n"; }

CScopeTester::~CScopeTester() { m_kernelCtx.getLogManager() << LogLevel_Debug << "## CScopeTester [" << m_prefix << "] leave\n"; }

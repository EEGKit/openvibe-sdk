#include "ovkCAlgorithmContext.h"
#include "ovkCAlgorithmProxy.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

CAlgorithmContext::CAlgorithmContext(const IKernelContext& rKernelContext, CAlgorithmProxy& rAlgorithmProxy, const IPluginObjectDesc& rPluginObjectDesc)
	: TKernelObject<IAlgorithmContext>(rKernelContext), m_rLogManager(rKernelContext.getLogManager()), m_rAlgorithmProxy(rAlgorithmProxy) {}

CAlgorithmContext::~CAlgorithmContext() {}

IConfigurationManager& CAlgorithmContext::getConfigurationManager() const
{
	return getKernelContext().getConfigurationManager();
}

IAlgorithmManager& CAlgorithmContext::getAlgorithmManager() const
{
	return getKernelContext().getAlgorithmManager();
}

ILogManager& CAlgorithmContext::getLogManager() const
{
	return m_rLogManager;
}

IErrorManager& CAlgorithmContext::getErrorManager() const
{
	return getKernelContext().getErrorManager();
}

ITypeManager& CAlgorithmContext::getTypeManager() const
{
	return getKernelContext().getTypeManager();
}

CIdentifier CAlgorithmContext::getNextInputParameterIdentifier(const CIdentifier& rPreviousInputParameterIdentifier) const
{
	return m_rAlgorithmProxy.getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
}

IParameter* CAlgorithmContext::getInputParameter(const CIdentifier& rInputParameterIdentifier)
{
	return m_rAlgorithmProxy.getInputParameter(rInputParameterIdentifier);
}

CIdentifier CAlgorithmContext::getNextOutputParameterIdentifier(const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	return m_rAlgorithmProxy.getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
}

IParameter* CAlgorithmContext::getOutputParameter(const CIdentifier& rOutputParameterIdentifier)
{
	return m_rAlgorithmProxy.getOutputParameter(rOutputParameterIdentifier);
}

bool CAlgorithmContext::isInputTriggerActive(const CIdentifier& rInputTriggerIdentifier) const
{
	return m_rAlgorithmProxy.isInputTriggerActive(rInputTriggerIdentifier);
}

bool CAlgorithmContext::activateOutputTrigger(const CIdentifier& rOutputTriggerIdentifier, const bool bTriggerState)
{
	return m_rAlgorithmProxy.activateOutputTrigger(rOutputTriggerIdentifier, bTriggerState);
}

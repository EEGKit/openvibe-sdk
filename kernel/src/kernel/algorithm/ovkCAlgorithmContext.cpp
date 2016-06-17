#include "ovkCAlgorithmContext.h"
#include "ovkCAlgorithm.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

CAlgorithmContext::CAlgorithmContext(const IKernelContext& rKernelContext, CAlgorithm& rAlgorithm, const IPluginObjectDesc& rPluginObjectDesc)
	:TKernelObject < IAlgorithmContext >(rKernelContext)
	,m_rLogManager(rKernelContext.getLogManager())
	,m_rAlgorithm(rAlgorithm)
{
}

CAlgorithmContext::~CAlgorithmContext(void)
{
}

IConfigurationManager& CAlgorithmContext::getConfigurationManager(void) const
{
	return getKernelContext().getConfigurationManager();
}

IAlgorithmManager& CAlgorithmContext::getAlgorithmManager(void) const
{
	return getKernelContext().getAlgorithmManager();
}

ILogManager& CAlgorithmContext::getLogManager(void) const
{
	return m_rLogManager;
}

ITypeManager& CAlgorithmContext::getTypeManager(void) const
{
	return getKernelContext().getTypeManager();
}

CIdentifier CAlgorithmContext::getNextInputParameterIdentifier(
	const CIdentifier& rPreviousInputParameterIdentifier) const
{
	return m_rAlgorithm.getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
}

IParameter* CAlgorithmContext::getInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	return m_rAlgorithm.getInputParameter(rInputParameterIdentifier);
}

CIdentifier CAlgorithmContext::getNextOutputParameterIdentifier(
	const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	return m_rAlgorithm.getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
}

IParameter* CAlgorithmContext::getOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	return m_rAlgorithm.getOutputParameter(rOutputParameterIdentifier);
}

boolean CAlgorithmContext::isInputTriggerActive(
	const CIdentifier& rInputTriggerIdentifier) const
{
	return m_rAlgorithm.isInputTriggerActive(rInputTriggerIdentifier);
}

boolean CAlgorithmContext::activateOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const boolean bTriggerState)
{
	return m_rAlgorithm.activateOutputTrigger(rOutputTriggerIdentifier, bTriggerState);
}

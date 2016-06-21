#include "ovkCAlgorithmProto.h"
#include "ovkCAlgorithmProxy.h"

using namespace OpenViBE;
using namespace Kernel;

CAlgorithmProto::CAlgorithmProto(const IKernelContext& rKernelContext, CAlgorithmProxy& rAlgorithmProxy)
	:TKernelObject < IAlgorithmProto >(rKernelContext)
	,m_rAlgorithmProxy(rAlgorithmProxy)
{
}

boolean CAlgorithmProto::addInputParameter(
	const CIdentifier& rInputParameterIdentifier,
	const CString& sInputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	return m_rAlgorithmProxy.addInputParameter(rInputParameterIdentifier, sInputName, eParameterType, rSubTypeIdentifier);
}

boolean CAlgorithmProto::addOutputParameter(
	const CIdentifier& rOutputParameterIdentifier,
	const CString& sOutputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	return m_rAlgorithmProxy.addOutputParameter(rOutputParameterIdentifier, sOutputName, eParameterType, rSubTypeIdentifier);
}

boolean CAlgorithmProto::addInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const CString& rInputTriggerName)
{
	return m_rAlgorithmProxy.addInputTrigger(rInputTriggerIdentifier, rInputTriggerName);
}

boolean CAlgorithmProto::addOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const CString& rOutputTriggerName)
{
	return m_rAlgorithmProxy.addOutputTrigger(rOutputTriggerIdentifier, rOutputTriggerName);
}

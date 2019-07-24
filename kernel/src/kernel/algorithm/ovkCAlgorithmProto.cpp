#include "ovkCAlgorithmProto.h"
#include "ovkCAlgorithmProxy.h"

using namespace OpenViBE;
using namespace Kernel;

CAlgorithmProto::CAlgorithmProto(const IKernelContext& rKernelContext, CAlgorithmProxy& rAlgorithmProxy)
	: TKernelObject<IAlgorithmProto>(rKernelContext), m_rAlgorithmProxy(rAlgorithmProxy) {}

bool CAlgorithmProto::addInputParameter(const CIdentifier& rInputParameterIdentifier, const CString& sInputName,
										const EParameterType eParameterType, const CIdentifier& rSubTypeIdentifier)
{
	return m_rAlgorithmProxy.addInputParameter(rInputParameterIdentifier, sInputName, eParameterType, rSubTypeIdentifier);
}

bool CAlgorithmProto::addOutputParameter(const CIdentifier& rOutputParameterIdentifier, const CString& sOutputName,
										 const EParameterType eParameterType, const CIdentifier& rSubTypeIdentifier)
{
	return m_rAlgorithmProxy.addOutputParameter(rOutputParameterIdentifier, sOutputName, eParameterType, rSubTypeIdentifier);
}

bool CAlgorithmProto::addInputTrigger(const CIdentifier& rInputTriggerIdentifier, const CString& rInputTriggerName)
{
	return m_rAlgorithmProxy.addInputTrigger(rInputTriggerIdentifier, rInputTriggerName);
}

bool CAlgorithmProto::addOutputTrigger(const CIdentifier& rOutputTriggerIdentifier, const CString& rOutputTriggerName)
{
	return m_rAlgorithmProxy.addOutputTrigger(rOutputTriggerIdentifier, rOutputTriggerName);
}

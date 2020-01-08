#include "ovkCAlgorithmProto.h"
#include "ovkCAlgorithmProxy.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

CAlgorithmProto::CAlgorithmProto(const IKernelContext& ctx, CAlgorithmProxy& rAlgorithmProxy)
	: TKernelObject<IAlgorithmProto>(ctx), m_rAlgorithmProxy(rAlgorithmProxy) {}

bool CAlgorithmProto::addInputParameter(const CIdentifier& inputParameterID, const CString& sInputName,
										const EParameterType eParameterType, const CIdentifier& subTypeID)
{
	return m_rAlgorithmProxy.addInputParameter(inputParameterID, sInputName, eParameterType, subTypeID);
}

bool CAlgorithmProto::addOutputParameter(const CIdentifier& outputParameterID, const CString& sOutputName,
										 const EParameterType eParameterType, const CIdentifier& subTypeID)
{
	return m_rAlgorithmProxy.addOutputParameter(outputParameterID, sOutputName, eParameterType, subTypeID);
}

bool CAlgorithmProto::addInputTrigger(const CIdentifier& inputTriggerID, const CString& rInputTriggerName)
{
	return m_rAlgorithmProxy.addInputTrigger(inputTriggerID, rInputTriggerName);
}

bool CAlgorithmProto::addOutputTrigger(const CIdentifier& outputTriggerID, const CString& rOutputTriggerName)
{
	return m_rAlgorithmProxy.addOutputTrigger(outputTriggerID, rOutputTriggerName);
}

#include <cassert>

#include "ovkCAlgorithmProxy.h"
#include "ovkCAlgorithmContext.h"
#include "ovkCAlgorithmProto.h"

#include "../../ovk_tools.h"

#include <openvibe/ovExceptionHandler.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

namespace
{
	void handleException(const CAlgorithmProxy* algorithm, const char* errorHint, const std::exception& exception)
	{
		algorithm->getLogManager() << LogLevel_Error << "Exception caught in algorithm\n";
		algorithm->getLogManager() << LogLevel_Error << "  [name: " << algorithm->getAlgorithmDesc().getName() << "]\n";
		algorithm->getLogManager() << LogLevel_Error << "  [class identifier: " << algorithm->getAlgorithmDesc().getCreatedClass() << "]\n";
		algorithm->getLogManager() << LogLevel_Error << "  [hint: " << (errorHint ? errorHint : "no hint") << "]\n";
		algorithm->getLogManager() << LogLevel_Error << "  [cause: " << exception.what() << "]\n";
	}
}

CAlgorithmProxy::CAlgorithmProxy(const IKernelContext& rKernelContext, IAlgorithm& rAlgorithm, const IAlgorithmDesc& rAlgorithmDesc)
	:TKernelObject < IAlgorithmProxy >(rKernelContext)
	,m_pInputConfigurable(NULL)
	,m_pOutputConfigurable(NULL)
	,m_rAlgorithmDesc(rAlgorithmDesc)
	,m_rAlgorithm(rAlgorithm)
	,m_bIsInitialized(false)
{
	m_pInputConfigurable=dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));
	m_pOutputConfigurable=dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));

	// FIXME
	CAlgorithmProto l_oProto(rKernelContext, *this);
	rAlgorithmDesc.getAlgorithmPrototype(l_oProto);
}

CAlgorithmProxy::~CAlgorithmProxy(void)
{
	getKernelContext().getKernelObjectFactory().releaseObject(m_pOutputConfigurable);
	getKernelContext().getKernelObjectFactory().releaseObject(m_pInputConfigurable);
}

IAlgorithm& CAlgorithmProxy::getAlgorithm(void)
{
	return m_rAlgorithm;
}

const IAlgorithm& CAlgorithmProxy::getAlgorithm(void) const
{
	return m_rAlgorithm;
}

const IAlgorithmDesc& CAlgorithmProxy::getAlgorithmDesc(void) const
{
	return m_rAlgorithmDesc;
}

boolean CAlgorithmProxy::addInputParameter(
	const CIdentifier& rInputParameterIdentifier,
	const CString& sInputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	if (m_pInputConfigurable->getParameter(rInputParameterIdentifier)!=NULL)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Input parameter id " << rInputParameterIdentifier << " already exists\n";
		return false;
	}
	m_pInputConfigurable->createParameter(rInputParameterIdentifier, eParameterType, rSubTypeIdentifier);
	m_vInputParameterName[rInputParameterIdentifier]=sInputName;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputParameterIdentifier(
	const CIdentifier& rPreviousInputParameterIdentifier) const
{
	return m_pInputConfigurable->getNextParameterIdentifier(rPreviousInputParameterIdentifier);
}

IParameter* CAlgorithmProxy::getInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	IParameter* l_pParameter=m_pInputConfigurable->getParameter(rInputParameterIdentifier);
	if (!l_pParameter)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null input parameter id " << rInputParameterIdentifier;
		if (m_pOutputConfigurable->getParameter(rInputParameterIdentifier))
		{
			getLogManager() << " Did you mean " << CString("output") << " parameter ?";
		}
		getLogManager() << "\n";
	}
	return l_pParameter;
}

EParameterType CAlgorithmProxy::getInputParameterType(
	const CIdentifier& rInputParameterIdentifier) const
{
	IParameter* l_pParameter=m_pInputConfigurable->getParameter(rInputParameterIdentifier);
	if (!l_pParameter)
	{
		return ParameterType_None;
	}
	return l_pParameter->getType();
}

CString CAlgorithmProxy::getInputParameterName(
	const CIdentifier& rInputParameterIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itName=m_vInputParameterName.find(rInputParameterIdentifier);
	if (itName==m_vInputParameterName.end())
	{
		return "";
	}
	return itName->second;
}

boolean CAlgorithmProxy::removeInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	if (!m_pInputConfigurable->removeParameter(rInputParameterIdentifier))
	{
		return false;
	}
	m_vInputParameterName.erase(m_vInputParameterName.find(rInputParameterIdentifier));
	return true;
}

boolean CAlgorithmProxy::addOutputParameter(
	const CIdentifier& rOutputParameterIdentifier,
	const CString& sOutputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	if (m_pOutputConfigurable->getParameter(rOutputParameterIdentifier)!=NULL)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Output parameter id " << rOutputParameterIdentifier << " already exists\n";
		return false;
	}
	m_pOutputConfigurable->createParameter(rOutputParameterIdentifier, eParameterType, rSubTypeIdentifier);
	m_vOutputParameterName[rOutputParameterIdentifier]=sOutputName;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputParameterIdentifier(
	const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	return m_pOutputConfigurable->getNextParameterIdentifier(rPreviousOutputParameterIdentifier);
}

IParameter* CAlgorithmProxy::getOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	IParameter* l_pParameter=m_pOutputConfigurable->getParameter(rOutputParameterIdentifier);
	if (!l_pParameter)
	{
		getLogManager() << LogLevel_Warning << "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null output parameter id " << rOutputParameterIdentifier;
		if (m_pInputConfigurable->getParameter(rOutputParameterIdentifier))
		{
			getLogManager() << " Did you mean " << CString("input") << " parameter ?";
		}
		getLogManager() << "\n";
	}
	return l_pParameter;
}

EParameterType CAlgorithmProxy::getOutputParameterType(
	const CIdentifier& rOutputParameterIdentifier) const
{
	IParameter* l_pParameter=m_pOutputConfigurable->getParameter(rOutputParameterIdentifier);
	if (!l_pParameter)
	{
		return ParameterType_None;
	}
	return l_pParameter->getType();
}

CString CAlgorithmProxy::getOutputParameterName(
	const CIdentifier& rOutputParameterIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itName=m_vOutputParameterName.find(rOutputParameterIdentifier);
	if (itName==m_vOutputParameterName.end())
	{
		return "";
	}
	return itName->second;
}

boolean CAlgorithmProxy::removeOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	if (!m_pOutputConfigurable->removeParameter(rOutputParameterIdentifier))
	{
		return false;
	}
	m_vOutputParameterName.erase(m_vOutputParameterName.find(rOutputParameterIdentifier));
	return true;
}

boolean CAlgorithmProxy::addInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const CString& rInputTriggerName)
{
	if (m_vInputTrigger.find(rInputTriggerIdentifier)!=m_vInputTrigger.end())
	{
		return false;
	}
	m_vInputTrigger[rInputTriggerIdentifier].first=rInputTriggerName;
	m_vInputTrigger[rInputTriggerIdentifier].second=false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputTriggerIdentifier(
	const CIdentifier& rPreviousInputTriggerIdentifier) const
{
	return getNextIdentifier< pair<CString, boolean> >(m_vInputTrigger, rPreviousInputTriggerIdentifier);
}

CString CAlgorithmProxy::getInputTriggerName(
	const CIdentifier& rInputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger==m_vInputTrigger.end())
	{
		return "";
	}
	return itTrigger->second.first;
}

boolean CAlgorithmProxy::isInputTriggerActive(
	const CIdentifier& rInputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger==m_vInputTrigger.end())
	{
		return false;
	}
	return itTrigger->second.second;
}

boolean CAlgorithmProxy::activateInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const boolean bTriggerState)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger==m_vInputTrigger.end())
	{
		return false;
	}
	itTrigger->second.second=true;
	return true;
}

boolean CAlgorithmProxy::removeInputTrigger(
	const CIdentifier& rInputTriggerIdentifier)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger==m_vInputTrigger.end())
	{
		return false;
	}
	m_vInputTrigger.erase(itTrigger);
	return true;
}

boolean CAlgorithmProxy::addOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const CString& rOutputTriggerName)
{
	if (m_vOutputTrigger.find(rOutputTriggerIdentifier)!=m_vOutputTrigger.end())
	{
		return false;
	}
	m_vOutputTrigger[rOutputTriggerIdentifier].first=rOutputTriggerName;
	m_vOutputTrigger[rOutputTriggerIdentifier].second=false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputTriggerIdentifier(
	const CIdentifier& rPreviousOutputTriggerIdentifier) const
{
	return getNextIdentifier< pair<CString, boolean> >(m_vOutputTrigger, rPreviousOutputTriggerIdentifier);
}

CString CAlgorithmProxy::getOutputTriggerName(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger==m_vOutputTrigger.end())
	{
		return "";
	}
	return itTrigger->second.first;
}

boolean CAlgorithmProxy::isOutputTriggerActive(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, boolean> >::const_iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger==m_vOutputTrigger.end())
	{
		return false;
	}
	return itTrigger->second.second;
}

boolean CAlgorithmProxy::activateOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const boolean bTriggerState)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger==m_vOutputTrigger.end())
	{
		return false;
	}
	itTrigger->second.second=true;
	return true;
}

boolean CAlgorithmProxy::removeOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger=m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger==m_vOutputTrigger.end())
	{
		return false;
	}
	m_vOutputTrigger.erase(itTrigger);
	return true;
}


boolean CAlgorithmProxy::initialize(void)
{
	assert(!m_bIsInitialized);
	
	return translateException(
		[&]() {
			CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
			// The dual state initialized or not does not take into account
			// a partially initialized state. Thus, we have to trust algorithms to implement
			// their initialization routine as a rollback transaction mechanism
			m_bIsInitialized = m_rAlgorithm.initialize(l_oAlgorithmContext);
			return m_bIsInitialized;
		},
		std::bind(handleException, this, "Algorithm initialization", std::placeholders::_1)
	);
}

boolean CAlgorithmProxy::uninitialize(void)
{
	assert(m_bIsInitialized);
	
	return translateException(
		[&]() {
			CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
			return m_rAlgorithm.uninitialize(l_oAlgorithmContext);
		},
		std::bind(handleException, this, "Algorithm uninitialization", std::placeholders::_1)
	);
}

boolean CAlgorithmProxy::process(void)
{
	assert(m_bIsInitialized);
	
	return translateException(
		[&]() {
			
			CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
			
			this->setAllOutputTriggers(false);
			
			bool l_bResult = m_rAlgorithm.process(l_oAlgorithmContext);
			
			this->setAllInputTriggers(false);
			
			return l_bResult;
		},
		std::bind(handleException, this, "Algorithm processing", std::placeholders::_1)
	);
}

boolean CAlgorithmProxy::process(
	const CIdentifier& rTriggerIdentifier)
{
	assert(m_bIsInitialized);
	
	if (!this->activateInputTrigger(rTriggerIdentifier, true))
	{
		return false;
	}
	
	return this->process();
}

void CAlgorithmProxy::setAllInputTriggers(const boolean bTriggerStatus)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger;
	for (itTrigger=m_vInputTrigger.begin(); itTrigger!=m_vInputTrigger.end(); itTrigger++)
	{
		itTrigger->second.second=bTriggerStatus;
	}
}

void CAlgorithmProxy::setAllOutputTriggers(const boolean bTriggerStatus)
{
	map<CIdentifier, pair<CString, boolean> >::iterator itTrigger;
	for (itTrigger=m_vOutputTrigger.begin(); itTrigger!=m_vOutputTrigger.end(); itTrigger++)
	{
		itTrigger->second.second=bTriggerStatus;
	}
}

boolean CAlgorithmProxy::isAlgorithmDerivedFrom(const CIdentifier& rClassIdentifier)
{
	return m_rAlgorithm.isDerivedFromClass(rClassIdentifier);
}

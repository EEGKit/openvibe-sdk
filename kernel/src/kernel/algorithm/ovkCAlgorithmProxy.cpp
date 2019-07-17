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

CAlgorithmProxy::CAlgorithmProxy(const IKernelContext& rKernelContext, IAlgorithm& rAlgorithm, const IAlgorithmDesc& rAlgorithmDesc)
	: TKernelObject<IAlgorithmProxy>(rKernelContext)
	  , m_pInputConfigurable(nullptr)
	  , m_pOutputConfigurable(nullptr)
	  , m_rAlgorithmDesc(rAlgorithmDesc)
	  , m_rAlgorithm(rAlgorithm)
	  , m_bIsInitialized(false)
{
	m_pInputConfigurable  = dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));
	m_pOutputConfigurable = dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));

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

bool CAlgorithmProxy::addInputParameter(
	const CIdentifier& rInputParameterIdentifier,
	const CString& sInputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	OV_ERROR_UNLESS_KRF(
		m_pInputConfigurable->getParameter(rInputParameterIdentifier) == nullptr,
		"For algorithm " << m_rAlgorithmDesc.getName() << " : Input parameter id " << rInputParameterIdentifier.toString() << " already exists",
		ErrorType::BadResourceCreation
	);

	m_pInputConfigurable->createParameter(rInputParameterIdentifier, eParameterType, rSubTypeIdentifier);
	m_vInputParameterName[rInputParameterIdentifier] = sInputName;
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
	IParameter* l_pParameter = m_pInputConfigurable->getParameter(rInputParameterIdentifier);

	OV_ERROR_UNLESS_KRN(
		l_pParameter,
		"For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null input parameter id " << rInputParameterIdentifier.toString(),
		ErrorType::ResourceNotFound
	);

	return l_pParameter;
}

EParameterType CAlgorithmProxy::getInputParameterType(
	const CIdentifier& rInputParameterIdentifier) const
{
	IParameter* l_pParameter = m_pInputConfigurable->getParameter(rInputParameterIdentifier);
	if (!l_pParameter)
	{
		return ParameterType_None;
	}
	return l_pParameter->getType();
}

CString CAlgorithmProxy::getInputParameterName(
	const CIdentifier& rInputParameterIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itName = m_vInputParameterName.find(rInputParameterIdentifier);
	if (itName == m_vInputParameterName.end())
	{
		return "";
	}
	return itName->second;
}

bool CAlgorithmProxy::removeInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	if (!m_pInputConfigurable->removeParameter(rInputParameterIdentifier)) { return false; }
	m_vInputParameterName.erase(m_vInputParameterName.find(rInputParameterIdentifier));
	return true;
}

bool CAlgorithmProxy::addOutputParameter(
	const CIdentifier& rOutputParameterIdentifier,
	const CString& sOutputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	OV_ERROR_UNLESS_KRF(
		m_pOutputConfigurable->getParameter(rOutputParameterIdentifier) == nullptr,
		"For algorithm " << m_rAlgorithmDesc.getName() << " : Output parameter id " << rOutputParameterIdentifier.toString() << " already exists",
		ErrorType::BadResourceCreation
	);

	m_pOutputConfigurable->createParameter(rOutputParameterIdentifier, eParameterType, rSubTypeIdentifier);
	m_vOutputParameterName[rOutputParameterIdentifier] = sOutputName;
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
	IParameter* l_pParameter = m_pOutputConfigurable->getParameter(rOutputParameterIdentifier);

	OV_ERROR_UNLESS_KRN(
		l_pParameter,
		"For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null output parameter id " << rOutputParameterIdentifier.toString(),
		ErrorType::ResourceNotFound
	);

	return l_pParameter;
}

EParameterType CAlgorithmProxy::getOutputParameterType(
	const CIdentifier& rOutputParameterIdentifier) const
{
	IParameter* l_pParameter = m_pOutputConfigurable->getParameter(rOutputParameterIdentifier);
	if (!l_pParameter)
	{
		return ParameterType_None;
	}
	return l_pParameter->getType();
}

CString CAlgorithmProxy::getOutputParameterName(
	const CIdentifier& rOutputParameterIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itName = m_vOutputParameterName.find(rOutputParameterIdentifier);
	if (itName == m_vOutputParameterName.end())
	{
		return "";
	}
	return itName->second;
}

bool CAlgorithmProxy::removeOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	if (!m_pOutputConfigurable->removeParameter(rOutputParameterIdentifier)) { return false; }
	m_vOutputParameterName.erase(m_vOutputParameterName.find(rOutputParameterIdentifier));
	return true;
}

bool CAlgorithmProxy::addInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const CString& rInputTriggerName)
{
	if (m_vInputTrigger.find(rInputTriggerIdentifier) != m_vInputTrigger.end()) { return false; }
	m_vInputTrigger[rInputTriggerIdentifier].first  = rInputTriggerName;
	m_vInputTrigger[rInputTriggerIdentifier].second = false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputTriggerIdentifier(
	const CIdentifier& rPreviousInputTriggerIdentifier) const
{
	return getNextIdentifier<pair<CString, bool>>(m_vInputTrigger, rPreviousInputTriggerIdentifier);
}

CString CAlgorithmProxy::getInputTriggerName(
	const CIdentifier& rInputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, bool>>::const_iterator itTrigger = m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger == m_vInputTrigger.end())
	{
		return "";
	}
	return itTrigger->second.first;
}

bool CAlgorithmProxy::isInputTriggerActive(
	const CIdentifier& rInputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, bool>>::const_iterator itTrigger = m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	return itTrigger->second.second;
}

bool CAlgorithmProxy::activateInputTrigger(const CIdentifier& rInputTriggerIdentifier, const bool bTriggerState)
{
	map<CIdentifier, pair<CString, bool>>::iterator itTrigger = m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	itTrigger->second.second = true;
	return true;
}

bool CAlgorithmProxy::removeInputTrigger(
	const CIdentifier& rInputTriggerIdentifier)
{
	map<CIdentifier, pair<CString, bool>>::iterator itTrigger = m_vInputTrigger.find(rInputTriggerIdentifier);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	m_vInputTrigger.erase(itTrigger);
	return true;
}

bool CAlgorithmProxy::addOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const CString& rOutputTriggerName)
{
	if (m_vOutputTrigger.find(rOutputTriggerIdentifier) != m_vOutputTrigger.end()) { return false; }
	m_vOutputTrigger[rOutputTriggerIdentifier].first  = rOutputTriggerName;
	m_vOutputTrigger[rOutputTriggerIdentifier].second = false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputTriggerIdentifier(
	const CIdentifier& rPreviousOutputTriggerIdentifier) const
{
	return getNextIdentifier<pair<CString, bool>>(m_vOutputTrigger, rPreviousOutputTriggerIdentifier);
}

CString CAlgorithmProxy::getOutputTriggerName(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, bool>>::const_iterator itTrigger = m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger == m_vOutputTrigger.end())
	{
		return "";
	}
	return itTrigger->second.first;
}

bool CAlgorithmProxy::isOutputTriggerActive(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	map<CIdentifier, pair<CString, bool>>::const_iterator itTrigger = m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	return itTrigger->second.second;
}

bool CAlgorithmProxy::activateOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const bool bTriggerState)
{
	map<CIdentifier, pair<CString, bool>>::iterator itTrigger = m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	itTrigger->second.second = true;
	return true;
}

bool CAlgorithmProxy::removeOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier)
{
	map<CIdentifier, pair<CString, bool>>::iterator itTrigger = m_vOutputTrigger.find(rOutputTriggerIdentifier);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	m_vOutputTrigger.erase(itTrigger);
	return true;
}


bool CAlgorithmProxy::initialize(void)
{
	assert(!m_bIsInitialized);

	return translateException(
		[&]()
		{
			CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
			// The dual state initialized or not does not take into account
			// a partially initialized state. Thus, we have to trust algorithms to implement
			// their initialization routine as a rollback transaction mechanism
			m_bIsInitialized = m_rAlgorithm.initialize(l_oAlgorithmContext);
			return m_bIsInitialized;
		},
		std::bind(&CAlgorithmProxy::handleException, this, "Algorithm initialization", std::placeholders::_1)
	);
}

bool CAlgorithmProxy::uninitialize(void)
{
	assert(m_bIsInitialized);

	return translateException(
		[&]()
		{
			CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
			return m_rAlgorithm.uninitialize(l_oAlgorithmContext);
		},
		std::bind(&CAlgorithmProxy::handleException, this, "Algorithm uninitialization", std::placeholders::_1)
	);
}

bool CAlgorithmProxy::process(void)
{
	assert(m_bIsInitialized);

	return translateException(
		[&]()
		{
			CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);

			this->setAllOutputTriggers(false);

			bool l_bResult = m_rAlgorithm.process(l_oAlgorithmContext);

			this->setAllInputTriggers(false);

			return l_bResult;
		},
		std::bind(&CAlgorithmProxy::handleException, this, "Algorithm processing", std::placeholders::_1)
	);
}

bool CAlgorithmProxy::process(
	const CIdentifier& rTriggerIdentifier)
{
	assert(m_bIsInitialized);

	if (!this->activateInputTrigger(rTriggerIdentifier, true)) { return false; }

	return this->process();
}

void CAlgorithmProxy::setAllInputTriggers(const bool bTriggerStatus)
{
	for (auto& trigger : m_vInputTrigger)
	{
		trigger.second.second = bTriggerStatus;
	}
}

void CAlgorithmProxy::setAllOutputTriggers(const bool bTriggerStatus)
{
	for (auto& trigger : m_vOutputTrigger)
	{
		trigger.second.second = bTriggerStatus;
	}
}

bool CAlgorithmProxy::isAlgorithmDerivedFrom(const CIdentifier& rClassIdentifier)
{
	return m_rAlgorithm.isDerivedFromClass(rClassIdentifier);
}

void CAlgorithmProxy::handleException(const char* errorHint, const std::exception& exception)
{
	this->getLogManager() << LogLevel_Error << "Exception caught in algorithm\n";
	this->getLogManager() << LogLevel_Error << "  [name: " << this->getAlgorithmDesc().getName() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [class identifier: " << this->getAlgorithmDesc().getCreatedClass() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [hint: " << (errorHint ? errorHint : "no hint") << "]\n";
	this->getLogManager() << LogLevel_Error << "  [cause: " << exception.what() << "]\n";

	OV_ERROR_KRV("Caught exception: " << exception.what(), ErrorType::ExceptionCaught);
}

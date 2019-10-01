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

CAlgorithmProxy::CAlgorithmProxy(const IKernelContext& ctx, IAlgorithm& rAlgorithm, const IAlgorithmDesc& rAlgorithmDesc)
	: TKernelObject<IAlgorithmProxy>(ctx), m_rAlgorithmDesc(rAlgorithmDesc)
	  , m_rAlgorithm(rAlgorithm)
{
	m_pInputConfigurable  = dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));
	m_pOutputConfigurable = dynamic_cast<IConfigurable*>(getKernelContext().getKernelObjectFactory().createObject(OV_ClassId_Kernel_Configurable));

	// FIXME
	CAlgorithmProto algorithmProto(ctx, *this);
	rAlgorithmDesc.getAlgorithmPrototype(algorithmProto);
}

CAlgorithmProxy::~CAlgorithmProxy()
{
	getKernelContext().getKernelObjectFactory().releaseObject(m_pOutputConfigurable);
	getKernelContext().getKernelObjectFactory().releaseObject(m_pInputConfigurable);
}

IAlgorithm& CAlgorithmProxy::getAlgorithm() { return m_rAlgorithm; }

const IAlgorithm& CAlgorithmProxy::getAlgorithm() const { return m_rAlgorithm; }

const IAlgorithmDesc& CAlgorithmProxy::getAlgorithmDesc() const { return m_rAlgorithmDesc; }

bool CAlgorithmProxy::addInputParameter(const CIdentifier& parameterID, const CString& name, const EParameterType parameterType,
										const CIdentifier& subTypeID)
{
	OV_ERROR_UNLESS_KRF(m_pInputConfigurable->getParameter(parameterID) == nullptr,
						"For algorithm " << m_rAlgorithmDesc.getName() << " : Input parameter id " << parameterID.toString() << " already exists",
						ErrorType::BadResourceCreation);

	m_pInputConfigurable->createParameter(parameterID, parameterType, subTypeID);
	m_vInputParameterName[parameterID] = name;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputParameterIdentifier(const CIdentifier& parameterID) const
{
	return m_pInputConfigurable->getNextParameterIdentifier(parameterID);
}

IParameter* CAlgorithmProxy::getInputParameter(const CIdentifier& parameterID)
{
	IParameter* parameter = m_pInputConfigurable->getParameter(parameterID);

	OV_ERROR_UNLESS_KRN(
		parameter, "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null input parameter id " << parameterID.toString(),
		ErrorType::ResourceNotFound);

	return parameter;
}

EParameterType CAlgorithmProxy::getInputParameterType(const CIdentifier& parameterID) const
{
	IParameter* parameter = m_pInputConfigurable->getParameter(parameterID);
	if (!parameter) { return ParameterType_None; }
	return parameter->getType();
}

CString CAlgorithmProxy::getInputParameterName(const CIdentifier& parameterID) const
{
	const auto itName = m_vInputParameterName.find(parameterID);
	if (itName == m_vInputParameterName.end()) { return ""; }
	return itName->second;
}

bool CAlgorithmProxy::removeInputParameter(const CIdentifier& parameterID)
{
	if (!m_pInputConfigurable->removeParameter(parameterID)) { return false; }
	m_vInputParameterName.erase(m_vInputParameterName.find(parameterID));
	return true;
}

bool CAlgorithmProxy::addOutputParameter(const CIdentifier& parameterID, const CString& sOutputName, const EParameterType parameterType,
										 const CIdentifier& subTypeID)
{
	OV_ERROR_UNLESS_KRF(m_pOutputConfigurable->getParameter(parameterID) == nullptr,
						"For algorithm " << m_rAlgorithmDesc.getName() << " : Output parameter id " << parameterID.toString() <<
						" already exists",
						ErrorType::BadResourceCreation);

	m_pOutputConfigurable->createParameter(parameterID, parameterType, subTypeID);
	m_vOutputParameterName[parameterID] = sOutputName;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputParameterIdentifier(const CIdentifier& parameterID) const
{
	return m_pOutputConfigurable->getNextParameterIdentifier(parameterID);
}

IParameter* CAlgorithmProxy::getOutputParameter(const CIdentifier& parameterID)
{
	IParameter* parameter = m_pOutputConfigurable->getParameter(parameterID);

	OV_ERROR_UNLESS_KRN(
		parameter, "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null output parameter id " << parameterID.toString(),
		ErrorType::ResourceNotFound);

	return parameter;
}

EParameterType CAlgorithmProxy::getOutputParameterType(const CIdentifier& parameterID) const
{
	IParameter* parameter = m_pOutputConfigurable->getParameter(parameterID);
	if (!parameter) { return ParameterType_None; }
	return parameter->getType();
}

CString CAlgorithmProxy::getOutputParameterName(const CIdentifier& parameterID) const
{
	const auto itName = m_vOutputParameterName.find(parameterID);
	if (itName == m_vOutputParameterName.end()) { return ""; }
	return itName->second;
}

bool CAlgorithmProxy::removeOutputParameter(const CIdentifier& parameterID)
{
	if (!m_pOutputConfigurable->removeParameter(parameterID)) { return false; }
	m_vOutputParameterName.erase(m_vOutputParameterName.find(parameterID));
	return true;
}

bool CAlgorithmProxy::addInputTrigger(const CIdentifier& triggerID, const CString& name)
{
	if (m_vInputTrigger.find(triggerID) != m_vInputTrigger.end()) { return false; }
	m_vInputTrigger[triggerID].first  = name;
	m_vInputTrigger[triggerID].second = false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputTriggerIdentifier(const CIdentifier& triggerID) const
{
	return getNextIdentifier<pair<CString, bool>>(m_vInputTrigger, triggerID);
}

CString CAlgorithmProxy::getInputTriggerName(const CIdentifier& triggerID) const
{
	const auto itTrigger = m_vInputTrigger.find(triggerID);
	if (itTrigger == m_vInputTrigger.end()) { return ""; }
	return itTrigger->second.first;
}

bool CAlgorithmProxy::isInputTriggerActive(const CIdentifier& triggerID) const
{
	const auto itTrigger = m_vInputTrigger.find(triggerID);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	return itTrigger->second.second;
}

bool CAlgorithmProxy::activateInputTrigger(const CIdentifier& triggerID, const bool /*triggerState*/)
{
	const auto itTrigger = m_vInputTrigger.find(triggerID);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	itTrigger->second.second = true;
	return true;
}

bool CAlgorithmProxy::removeInputTrigger(const CIdentifier& triggerID)
{
	const auto itTrigger = m_vInputTrigger.find(triggerID);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	m_vInputTrigger.erase(itTrigger);
	return true;
}

bool CAlgorithmProxy::addOutputTrigger(const CIdentifier& triggerID, const CString& name)
{
	if (m_vOutputTrigger.find(triggerID) != m_vOutputTrigger.end()) { return false; }
	m_vOutputTrigger[triggerID].first  = name;
	m_vOutputTrigger[triggerID].second = false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputTriggerIdentifier(const CIdentifier& triggerID) const
{
	return getNextIdentifier<pair<CString, bool>>(m_vOutputTrigger, triggerID);
}

CString CAlgorithmProxy::getOutputTriggerName(const CIdentifier& triggerID) const
{
	const auto itTrigger = m_vOutputTrigger.find(triggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return ""; }
	return itTrigger->second.first;
}

bool CAlgorithmProxy::isOutputTriggerActive(const CIdentifier& triggerID) const
{
	const auto itTrigger = m_vOutputTrigger.find(triggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	return itTrigger->second.second;
}

bool CAlgorithmProxy::activateOutputTrigger(const CIdentifier& triggerID, const bool /*bTriggerState*/)
{
	const auto itTrigger = m_vOutputTrigger.find(triggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	itTrigger->second.second = true;
	return true;
}

bool CAlgorithmProxy::removeOutputTrigger(const CIdentifier& triggerID)
{
	const auto itTrigger = m_vOutputTrigger.find(triggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	m_vOutputTrigger.erase(itTrigger);
	return true;
}


bool CAlgorithmProxy::initialize()
{
	assert(!m_bIsInitialized);

	return translateException([&]()
							  {
								  CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
								  // The dual state initialized or not does not take into account
								  // a partially initialized state. Thus, we have to trust algorithms to implement
								  // their initialization routine as a rollback transaction mechanism
								  m_bIsInitialized = m_rAlgorithm.initialize(l_oAlgorithmContext);
								  return m_bIsInitialized;
							  },
							  std::bind(&CAlgorithmProxy::handleException, this, "Algorithm initialization", std::placeholders::_1));
}

bool CAlgorithmProxy::uninitialize()
{
	assert(m_bIsInitialized);

	return translateException([&]()
							  {
								  CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
								  return m_rAlgorithm.uninitialize(l_oAlgorithmContext);
							  },
							  std::bind(&CAlgorithmProxy::handleException, this, "Algorithm uninitialization", std::placeholders::_1));
}

bool CAlgorithmProxy::process()
{
	assert(m_bIsInitialized);

	return translateException([&]()
							  {
								  CAlgorithmContext l_oAlgorithmContext(getKernelContext(), *this, m_rAlgorithmDesc);
								  this->setAllOutputTriggers(false);
								  const bool result = m_rAlgorithm.process(l_oAlgorithmContext);
								  this->setAllInputTriggers(false);
								  return result;
							  },
							  std::bind(&CAlgorithmProxy::handleException, this, "Algorithm processing", std::placeholders::_1));
}

bool CAlgorithmProxy::process(const CIdentifier& triggerID)
{
	assert(m_bIsInitialized);
	if (!this->activateInputTrigger(triggerID, true)) { return false; }
	return this->process();
}

void CAlgorithmProxy::setAllInputTriggers(const bool status) { for (auto& trigger : m_vInputTrigger) { trigger.second.second = status; } }

void CAlgorithmProxy::setAllOutputTriggers(const bool status) { for (auto& trigger : m_vOutputTrigger) { trigger.second.second = status; } }

bool CAlgorithmProxy::isAlgorithmDerivedFrom(const CIdentifier& classID) { return m_rAlgorithm.isDerivedFromClass(classID); }

void CAlgorithmProxy::handleException(const char* errorHint, const std::exception& exception)
{
	this->getLogManager() << LogLevel_Error << "Exception caught in algorithm\n";
	this->getLogManager() << LogLevel_Error << "  [name: " << this->getAlgorithmDesc().getName() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [class identifier: " << this->getAlgorithmDesc().getCreatedClass() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [hint: " << (errorHint ? errorHint : "no hint") << "]\n";
	this->getLogManager() << LogLevel_Error << "  [cause: " << exception.what() << "]\n";

	OV_ERROR_KRV("Caught exception: " << exception.what(), ErrorType::ExceptionCaught);
}

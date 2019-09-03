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

bool CAlgorithmProxy::addInputParameter(const CIdentifier& InputParameterID, const CString& sInputName, const EParameterType eParameterType,
										const CIdentifier& subTypeID)
{
	OV_ERROR_UNLESS_KRF(m_pInputConfigurable->getParameter(InputParameterID) == nullptr,
						"For algorithm " << m_rAlgorithmDesc.getName() << " : Input parameter id " << InputParameterID.toString() << " already exists",
						ErrorType::BadResourceCreation);

	m_pInputConfigurable->createParameter(InputParameterID, eParameterType, subTypeID);
	m_vInputParameterName[InputParameterID] = sInputName;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputParameterIdentifier(const CIdentifier& rPreviousInputParameterIdentifier) const
{
	return m_pInputConfigurable->getNextParameterIdentifier(rPreviousInputParameterIdentifier);
}

IParameter* CAlgorithmProxy::getInputParameter(const CIdentifier& InputParameterID)
{
	IParameter* parameter = m_pInputConfigurable->getParameter(InputParameterID);

	OV_ERROR_UNLESS_KRN(
		parameter, "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null input parameter id " << InputParameterID.toString(),
		ErrorType::ResourceNotFound);

	return parameter;
}

EParameterType CAlgorithmProxy::getInputParameterType(const CIdentifier& InputParameterID) const
{
	IParameter* parameter = m_pInputConfigurable->getParameter(InputParameterID);
	if (!parameter) { return ParameterType_None; }
	return parameter->getType();
}

CString CAlgorithmProxy::getInputParameterName(const CIdentifier& InputParameterID) const
{
	const auto itName = m_vInputParameterName.find(InputParameterID);
	if (itName == m_vInputParameterName.end()) { return ""; }
	return itName->second;
}

bool CAlgorithmProxy::removeInputParameter(const CIdentifier& InputParameterID)
{
	if (!m_pInputConfigurable->removeParameter(InputParameterID)) { return false; }
	m_vInputParameterName.erase(m_vInputParameterName.find(InputParameterID));
	return true;
}

bool CAlgorithmProxy::addOutputParameter(const CIdentifier& outputParameterID, const CString& sOutputName, const EParameterType eParameterType,
										 const CIdentifier& subTypeID)
{
	OV_ERROR_UNLESS_KRF(m_pOutputConfigurable->getParameter(outputParameterID) == nullptr,
						"For algorithm " << m_rAlgorithmDesc.getName() << " : Output parameter id " << outputParameterID.toString() <<
						" already exists",
						ErrorType::BadResourceCreation);

	m_pOutputConfigurable->createParameter(outputParameterID, eParameterType, subTypeID);
	m_vOutputParameterName[outputParameterID] = sOutputName;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputParameterIdentifier(const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	return m_pOutputConfigurable->getNextParameterIdentifier(rPreviousOutputParameterIdentifier);
}

IParameter* CAlgorithmProxy::getOutputParameter(const CIdentifier& outputParameterID)
{
	IParameter* parameter = m_pOutputConfigurable->getParameter(outputParameterID);

	OV_ERROR_UNLESS_KRN(
		parameter, "For algorithm " << m_rAlgorithmDesc.getName() << " : Requested null output parameter id " << outputParameterID.toString(),
		ErrorType::ResourceNotFound);

	return parameter;
}

EParameterType CAlgorithmProxy::getOutputParameterType(const CIdentifier& outputParameterID) const
{
	IParameter* parameter = m_pOutputConfigurable->getParameter(outputParameterID);
	if (!parameter) { return ParameterType_None; }
	return parameter->getType();
}

CString CAlgorithmProxy::getOutputParameterName(const CIdentifier& outputParameterID) const
{
	const auto itName = m_vOutputParameterName.find(outputParameterID);
	if (itName == m_vOutputParameterName.end()) { return ""; }
	return itName->second;
}

bool CAlgorithmProxy::removeOutputParameter(const CIdentifier& outputParameterID)
{
	if (!m_pOutputConfigurable->removeParameter(outputParameterID)) { return false; }
	m_vOutputParameterName.erase(m_vOutputParameterName.find(outputParameterID));
	return true;
}

bool CAlgorithmProxy::addInputTrigger(const CIdentifier& inputTriggerID, const CString& rInputTriggerName)
{
	if (m_vInputTrigger.find(inputTriggerID) != m_vInputTrigger.end()) { return false; }
	m_vInputTrigger[inputTriggerID].first  = rInputTriggerName;
	m_vInputTrigger[inputTriggerID].second = false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextInputTriggerIdentifier(const CIdentifier& rPreviousInputTriggerIdentifier) const
{
	return getNextIdentifier<pair<CString, bool>>(m_vInputTrigger, rPreviousInputTriggerIdentifier);
}

CString CAlgorithmProxy::getInputTriggerName(const CIdentifier& inputTriggerID) const
{
	const auto itTrigger = m_vInputTrigger.find(inputTriggerID);
	if (itTrigger == m_vInputTrigger.end()) { return ""; }
	return itTrigger->second.first;
}

bool CAlgorithmProxy::isInputTriggerActive(const CIdentifier& inputTriggerID) const
{
	map<CIdentifier, pair<CString, bool>>::const_iterator itTrigger = m_vInputTrigger.find(inputTriggerID);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	return itTrigger->second.second;
}

bool CAlgorithmProxy::activateInputTrigger(const CIdentifier& inputTriggerID, const bool /*bTriggerState*/)
{
	const auto itTrigger = m_vInputTrigger.find(inputTriggerID);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	itTrigger->second.second = true;
	return true;
}

bool CAlgorithmProxy::removeInputTrigger(const CIdentifier& inputTriggerID)
{
	const auto itTrigger = m_vInputTrigger.find(inputTriggerID);
	if (itTrigger == m_vInputTrigger.end()) { return false; }
	m_vInputTrigger.erase(itTrigger);
	return true;
}

bool CAlgorithmProxy::addOutputTrigger(const CIdentifier& outputTriggerID, const CString& rOutputTriggerName)
{
	if (m_vOutputTrigger.find(outputTriggerID) != m_vOutputTrigger.end()) { return false; }
	m_vOutputTrigger[outputTriggerID].first  = rOutputTriggerName;
	m_vOutputTrigger[outputTriggerID].second = false;
	return true;
}

CIdentifier CAlgorithmProxy::getNextOutputTriggerIdentifier(const CIdentifier& rPreviousOutputTriggerIdentifier) const
{
	return getNextIdentifier<pair<CString, bool>>(m_vOutputTrigger, rPreviousOutputTriggerIdentifier);
}

CString CAlgorithmProxy::getOutputTriggerName(const CIdentifier& outputTriggerID) const
{
	const auto itTrigger = m_vOutputTrigger.find(outputTriggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return ""; }
	return itTrigger->second.first;
}

bool CAlgorithmProxy::isOutputTriggerActive(const CIdentifier& outputTriggerID) const
{
	const auto itTrigger = m_vOutputTrigger.find(outputTriggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	return itTrigger->second.second;
}

bool CAlgorithmProxy::activateOutputTrigger(const CIdentifier& outputTriggerID, const bool /*bTriggerState*/)
{
	const auto itTrigger = m_vOutputTrigger.find(outputTriggerID);
	if (itTrigger == m_vOutputTrigger.end()) { return false; }
	itTrigger->second.second = true;
	return true;
}

bool CAlgorithmProxy::removeOutputTrigger(const CIdentifier& outputTriggerID)
{
	const auto itTrigger = m_vOutputTrigger.find(outputTriggerID);
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

bool CAlgorithmProxy::process(const CIdentifier& rTriggerIdentifier)
{
	assert(m_bIsInitialized);
	if (!this->activateInputTrigger(rTriggerIdentifier, true)) { return false; }
	return this->process();
}

void CAlgorithmProxy::setAllInputTriggers(const bool bTriggerStatus) { for (auto& trigger : m_vInputTrigger) { trigger.second.second = bTriggerStatus; } }

void CAlgorithmProxy::setAllOutputTriggers(const bool bTriggerStatus) { for (auto& trigger : m_vOutputTrigger) { trigger.second.second = bTriggerStatus; } }

bool CAlgorithmProxy::isAlgorithmDerivedFrom(const CIdentifier& rClassIdentifier) { return m_rAlgorithm.isDerivedFromClass(rClassIdentifier); }

void CAlgorithmProxy::handleException(const char* errorHint, const std::exception& exception)
{
	this->getLogManager() << LogLevel_Error << "Exception caught in algorithm\n";
	this->getLogManager() << LogLevel_Error << "  [name: " << this->getAlgorithmDesc().getName() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [class identifier: " << this->getAlgorithmDesc().getCreatedClass() << "]\n";
	this->getLogManager() << LogLevel_Error << "  [hint: " << (errorHint ? errorHint : "no hint") << "]\n";
	this->getLogManager() << LogLevel_Error << "  [cause: " << exception.what() << "]\n";

	OV_ERROR_KRV("Caught exception: " << exception.what(), ErrorType::ExceptionCaught);
}

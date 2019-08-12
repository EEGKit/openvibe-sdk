#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"
#include "ovkCPlayer.h"

using namespace OpenViBE;
using namespace Kernel;

CPlayerContext::CPlayerContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox)
	: TKernelObject<IPlayerContext>(rKernelContext)
	  , m_rSimulatedBox(*pSimulatedBox)
	  , m_rPluginManager(rKernelContext.getPluginManager())
	  , m_rAlgorithmManager(rKernelContext.getAlgorithmManager())
	  , m_rConfigurationManager(rKernelContext.getConfigurationManager())
	  , m_rLogManager(rKernelContext.getLogManager())
	  , m_rErrorManager(rKernelContext.getErrorManager())
	  , m_rScenarioManager(rKernelContext.getScenarioManager())
	  , m_rTypeManager(rKernelContext.getTypeManager())
	  , m_BoxLogManager(*this, m_rLogManager, m_rSimulatedBox) {}

CPlayerContext::~CPlayerContext() {}

bool CPlayerContext::sendSignal(const CMessageSignal& rMessageSignal)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendSignal - Not yet implemented\n";
	return false;
}

bool CPlayerContext::sendMessage(const CMessageEvent& rMessageEvent, const CIdentifier& rTargetIdentifier)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

bool CPlayerContext::sendMessage(const CMessageEvent& rMessageEvent, const CIdentifier* pTargetIdentifier, const uint32_t ui32TargetIdentifierCount)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

uint64_t CPlayerContext::getCurrentTime() const
{
	return m_rSimulatedBox.getScheduler().getCurrentTime();
}

uint64_t CPlayerContext::getCurrentLateness() const
{
	return m_rSimulatedBox.getScheduler().getCurrentLateness();
}

double CPlayerContext::getCurrentCPUUsage() const
{
	return m_rSimulatedBox.getScheduler().getCPUUsage();
}

double CPlayerContext::getCurrentFastForwardMaximumFactor() const
{
	return m_rSimulatedBox.getScheduler().getFastForwardMaximumFactor();
}

bool CPlayerContext::stop()
{
	return m_rSimulatedBox.getScheduler().getPlayer().stop();
}

bool CPlayerContext::pause()
{
	return m_rSimulatedBox.getScheduler().getPlayer().pause();
}

bool CPlayerContext::play()
{
	return m_rSimulatedBox.getScheduler().getPlayer().play();
}

bool CPlayerContext::forward()
{
	return m_rSimulatedBox.getScheduler().getPlayer().forward();
}

EPlayerStatus CPlayerContext::getStatus() const
{
	return m_rSimulatedBox.getScheduler().getPlayer().getStatus();
}

IAlgorithmManager& CPlayerContext::getAlgorithmManager() const
{
	return m_rAlgorithmManager;
}

IConfigurationManager& CPlayerContext::getConfigurationManager() const
{
	return m_rConfigurationManager;
}

ILogManager& CPlayerContext::getLogManager() const
{
	return m_BoxLogManager;
}

IErrorManager& CPlayerContext::getErrorManager() const
{
	return m_rErrorManager;
}

IScenarioManager& CPlayerContext::getScenarioManager() const
{
	return m_rScenarioManager;
}

ITypeManager& CPlayerContext::getTypeManager() const
{
	return m_rTypeManager;
}

bool CPlayerContext::canCreatePluginObject(const CIdentifier& pluginIdentifier) const
{
	return m_rPluginManager.canCreatePluginObject(pluginIdentifier);
}

Plugins::IPluginObject* CPlayerContext::createPluginObject(const CIdentifier& pluginIdentifier) const
{
	return m_rPluginManager.createPluginObject(pluginIdentifier);
}

bool CPlayerContext::releasePluginObject(Plugins::IPluginObject* pluginObject) const
{
	return m_rPluginManager.releasePluginObject(pluginObject);
}

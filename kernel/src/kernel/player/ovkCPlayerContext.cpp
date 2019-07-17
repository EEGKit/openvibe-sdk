#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"
#include "ovkCPlayer.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

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

CPlayerContext::~CPlayerContext(void) {}

bool CPlayerContext::sendSignal(
	const CMessageSignal& rMessageSignal)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendSignal - Not yet implemented\n";
	return false;
}

bool CPlayerContext::sendMessage(
	const CMessageEvent& rMessageEvent,
	const CIdentifier& rTargetIdentifier)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

bool CPlayerContext::sendMessage(
	const CMessageEvent& rMessageEvent,
	const CIdentifier* pTargetIdentifier,
	const uint32 ui32TargetIdentifierCount)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

uint64 CPlayerContext::getCurrentTime(void) const
{
	return m_rSimulatedBox.getScheduler().getCurrentTime();
}

uint64 CPlayerContext::getCurrentLateness(void) const
{
	return m_rSimulatedBox.getScheduler().getCurrentLateness();
}

double CPlayerContext::getCurrentCPUUsage(void) const
{
	return m_rSimulatedBox.getScheduler().getCPUUsage();
}

double CPlayerContext::getCurrentFastForwardMaximumFactor(void) const
{
	return m_rSimulatedBox.getScheduler().getFastForwardMaximumFactor();
}

bool CPlayerContext::stop(void)
{
	return m_rSimulatedBox.getScheduler().getPlayer().stop();
}

bool CPlayerContext::pause(void)
{
	return m_rSimulatedBox.getScheduler().getPlayer().pause();
}

bool CPlayerContext::play(void)
{
	return m_rSimulatedBox.getScheduler().getPlayer().play();
}

bool CPlayerContext::forward(void)
{
	return m_rSimulatedBox.getScheduler().getPlayer().forward();
}

EPlayerStatus CPlayerContext::getStatus(void) const
{
	return m_rSimulatedBox.getScheduler().getPlayer().getStatus();
}

IAlgorithmManager& CPlayerContext::getAlgorithmManager(void) const
{
	return m_rAlgorithmManager;
}

IConfigurationManager& CPlayerContext::getConfigurationManager(void) const
{
	return m_rConfigurationManager;
}

ILogManager& CPlayerContext::getLogManager(void) const
{
	return m_BoxLogManager;
}

IErrorManager& CPlayerContext::getErrorManager(void) const
{
	return m_rErrorManager;
}

IScenarioManager& CPlayerContext::getScenarioManager(void) const
{
	return m_rScenarioManager;
}

ITypeManager& CPlayerContext::getTypeManager(void) const
{
	return m_rTypeManager;
}

bool CPlayerContext::canCreatePluginObject(const OpenViBE::CIdentifier& pluginIdentifier) const
{
	return m_rPluginManager.canCreatePluginObject(pluginIdentifier);
}

OpenViBE::Plugins::IPluginObject* CPlayerContext::createPluginObject(const OpenViBE::CIdentifier& pluginIdentifier) const
{
	return m_rPluginManager.createPluginObject(pluginIdentifier);
}

bool CPlayerContext::releasePluginObject(OpenViBE::Plugins::IPluginObject* pluginObject) const
{
	return m_rPluginManager.releasePluginObject(pluginObject);
}

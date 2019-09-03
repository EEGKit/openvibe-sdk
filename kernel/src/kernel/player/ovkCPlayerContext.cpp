#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"
#include "ovkCPlayer.h"

using namespace OpenViBE;
using namespace Kernel;

CPlayerContext::CPlayerContext(const IKernelContext& ctx, CSimulatedBox* pSimulatedBox)
	: TKernelObject<IPlayerContext>(ctx), m_rSimulatedBox(*pSimulatedBox), m_rPluginManager(ctx.getPluginManager()),
	  m_rAlgorithmManager(ctx.getAlgorithmManager()), m_rConfigurationManager(ctx.getConfigurationManager()),
	  m_rLogManager(ctx.getLogManager()), m_rErrorManager(ctx.getErrorManager()), m_rScenarioManager(ctx.getScenarioManager()),
	  m_rTypeManager(ctx.getTypeManager()), m_BoxLogManager(*this, m_rLogManager, m_rSimulatedBox) {}

bool CPlayerContext::sendSignal(const CMessageSignal& /*messageSignal*/)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendSignal - Not yet implemented\n";
	return false;
}

bool CPlayerContext::sendMessage(const CMessageEvent& /*messageEvent*/, const CIdentifier& /*dstID*/)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

bool CPlayerContext::sendMessage(const CMessageEvent& /*messageEvent*/, const CIdentifier* /*dstID*/, const uint32_t /*nDstID*/)
{
	// TODO
	this->getLogManager() << LogLevel_Debug << "CPlayerContext::sendMessage - Not yet implemented\n";
	return false;
}

uint64_t CPlayerContext::getCurrentTime() const { return m_rSimulatedBox.getScheduler().getCurrentTime(); }

uint64_t CPlayerContext::getCurrentLateness() const { return m_rSimulatedBox.getScheduler().getCurrentLateness(); }

double CPlayerContext::getCurrentCPUUsage() const { return m_rSimulatedBox.getScheduler().getCPUUsage(); }

double CPlayerContext::getCurrentFastForwardMaximumFactor() const { return m_rSimulatedBox.getScheduler().getFastForwardMaximumFactor(); }

bool CPlayerContext::stop() { return m_rSimulatedBox.getScheduler().getPlayer().stop(); }

bool CPlayerContext::pause() { return m_rSimulatedBox.getScheduler().getPlayer().pause(); }

bool CPlayerContext::play() { return m_rSimulatedBox.getScheduler().getPlayer().play(); }

bool CPlayerContext::forward() { return m_rSimulatedBox.getScheduler().getPlayer().forward(); }

EPlayerStatus CPlayerContext::getStatus() const { return m_rSimulatedBox.getScheduler().getPlayer().getStatus(); }

#include <openvibe/ovITimeArithmetics.h>

#include "ovkCPlayer.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"

#include "../configuration/ovkCConfigurationManager.h"
#include "../scenario/ovkCScenarioManager.h"
#include "../scenario/ovkCScenarioSettingKeywordParserCallback.h"

#include <system/ovCTime.h>

#include <xml/IReader.h>

#include <string>
#include <iostream>
#include <fstream>

//___________________________________________________________________//
//                                                                   //

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Kernel;
using namespace Plugins;


const uint64_t g_ui64Scheduler_Default_Frequency_      = 128;
const uint64_t g_ui64Scheduler_Maximum_Loops_Duration_ = (100LL << 22); /* 100/1024 seconds, approx 100ms */

//___________________________________________________________________//
//                                                                   //

CPlayer::CPlayer(const IKernelContext& rKernelContext)
	: TKernelObject<IPlayer>(rKernelContext), m_oKernelContextBridge(rKernelContext), m_oScheduler(m_oKernelContextBridge, *this)
{
	uint64_t schedulerFrequency = this->getConfigurationManager().expandAsUInteger("${Kernel_PlayerFrequency}");
	if (schedulerFrequency == 0)
	{
		OV_WARNING_K("Invalid frequency configuration " << CString("Kernel_PlayerFrequency") << "=" << this->getConfigurationManager().expand("${Kernel_PlayerFrequency}") << " restored to default " << g_ui64Scheduler_Default_Frequency_);
		schedulerFrequency = g_ui64Scheduler_Default_Frequency_;
	}
	else
	{
		getLogManager() << LogLevel_Trace << "Player frequency set to " << schedulerFrequency << "\n";
	}
	m_oScheduler.setFrequency(schedulerFrequency);
}

CPlayer::~CPlayer()
{
	if (this->isHoldingResources()) { this->uninitialize(); }

	delete m_pRuntimeConfigurationManager;
	delete m_pRuntimeScenarioManager;
}

//___________________________________________________________________//
//                                                                   //

bool CPlayer::setScenario(const CIdentifier& rScenarioIdentifier, const CNameValuePairList* pLocalConfigurationTokens)
{
	OV_ERROR_UNLESS_KRF(!this->isHoldingResources(), "Trying to configure a player with non-empty resources", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Debug << "Player setScenario\n";

	// Create a fresh runtime configuration manager which will handle scenario-specific
	// configuration, such as the scenario settings and local settings (workspace)

	delete m_pRuntimeConfigurationManager;
	m_pRuntimeConfigurationManager = new CConfigurationManager(this->getKernelContext(), &this->getKernelContext().getConfigurationManager());
	m_pRuntimeConfigurationManager->addConfigurationFromFile(this->getKernelContext().getConfigurationManager().expand("${Kernel_DelayedConfiguration}"));
	IScenario& originalScenario = this->getScenarioManager().getScenario(rScenarioIdentifier);

	delete m_pRuntimeScenarioManager;
	m_pRuntimeScenarioManager = new CScenarioManager(this->getKernelContext());
	m_pRuntimeScenarioManager->cloneScenarioImportersAndExporters(this->getKernelContext().getScenarioManager());

	OV_ERROR_UNLESS_KRF(m_pRuntimeScenarioManager->createScenario(m_oRuntimeScenarioIdentifier), 
						"Fail to create a scenario duplicate for the current runtime session", ErrorType::BadResourceCreation);

	IScenario& l_rRuntimeScenario = m_pRuntimeScenarioManager->getScenario(m_oRuntimeScenarioIdentifier);
	l_rRuntimeScenario.merge(originalScenario, nullptr, true, true);

	if (l_rRuntimeScenario.hasAttribute(OV_AttributeId_ScenarioFilename))
	{
		std::string filename      = l_rRuntimeScenario.getAttributeValue(OV_AttributeId_ScenarioFilename).toASCIIString();
		std::string directoryName = ".";
		m_pRuntimeConfigurationManager->createConfigurationToken("Player_ScenarioFilename", filename.c_str());

		size_t iDir = filename.rfind("/");
		if (iDir != std::string::npos)
		{
			directoryName = filename.substr(0, iDir).c_str();
		}
		m_pRuntimeConfigurationManager->createConfigurationToken("Player_ScenarioDirectory", directoryName.c_str());
		m_pRuntimeConfigurationManager->createConfigurationToken("__volatile_ScenarioDir", directoryName.c_str());
		std::string workspaceConfigurationFile = directoryName + "/" + std::string("openvibe-workspace.conf");
		this->getLogManager() << LogLevel_Trace << "Player adds workspace configuration file [" << CString(workspaceConfigurationFile.c_str()) << "] to runtime configuration manager\n";
		m_pRuntimeConfigurationManager->addConfigurationFromFile(CString(workspaceConfigurationFile.c_str()));
		std::string scenarioConfigurationFile = directoryName + "/" + std::string("scenario.conf");
		this->getLogManager() << LogLevel_Trace << "Player adds scenario configuration file [" << CString(scenarioConfigurationFile.c_str()) << "] to runtime configuration manager\n";
		m_pRuntimeConfigurationManager->addConfigurationFromFile(CString(scenarioConfigurationFile.c_str()));

		size_t ext = filename.rfind(".");
		if (ext != std::string::npos)
		{
			std::string scenarioConfigurationFile = filename.substr(0, ext) + std::string(".conf");
			this->getLogManager() << LogLevel_Trace << "Player adds scenario configuration file [" << CString(scenarioConfigurationFile.c_str()) << "] to runtime configuration manager\n";
			m_pRuntimeConfigurationManager->addConfigurationFromFile(CString(scenarioConfigurationFile.c_str()));
		}
	}

	// Sets configuration tokens for this player
	// Once every token file, applies the configuration tokens coming from an external application defining its own scenario specific tokens
	if (pLocalConfigurationTokens != nullptr)
	{
		this->getLogManager() << LogLevel_Trace << "Player setScenario: add local configuration token from map.\n";
		for (unsigned int i = 0; i < pLocalConfigurationTokens->getSize(); i++)
		{
			CString l_sName;
			CString l_sValue;
			if (pLocalConfigurationTokens->getValue(i, l_sName, l_sValue))
			{
				this->getLogManager() << LogLevel_Debug << "Player setScenario: add local configuration token: [" << l_sName << "] = [" << l_sValue << "].\n";
				CIdentifier l_oTokenIdentifier = m_pRuntimeConfigurationManager->lookUpConfigurationTokenIdentifier(l_sName);
				if (l_oTokenIdentifier == OV_UndefinedIdentifier)
				{
					m_pRuntimeConfigurationManager->createConfigurationToken(l_sName, l_sValue);
				}
				else
				{
					m_pRuntimeConfigurationManager->setConfigurationTokenValue(l_oTokenIdentifier, l_sValue);
				}
			}
				// This should not happen
			else
			{
				this->getLogManager() << LogLevel_Trace << "Player setScenario: Could not acces to value of pLocalConfigurationTokens at index " << i << ".\n";
			}
		}
	}

	OV_ERROR_UNLESS_KRF(l_rRuntimeScenario.checkSettings(m_pRuntimeConfigurationManager), "Checking settings failed for scenario duplicate instantiated for the current runtime session", ErrorType::BadArgument);

	return m_oScheduler.setScenario(m_oRuntimeScenarioIdentifier);
}

IConfigurationManager& CPlayer::getRuntimeConfigurationManager() const { return *m_pRuntimeConfigurationManager; }
IScenarioManager& CPlayer::getRuntimeScenarioManager() const { return *m_pRuntimeScenarioManager; }
CIdentifier CPlayer::getRuntimeScenarioIdentifier() const { return m_oRuntimeScenarioIdentifier; }

EPlayerReturnCode CPlayer::initialize()
{
	OV_ERROR_UNLESS_K(!this->isHoldingResources(), "Trying to configure a player with non-empty resources", ErrorType::BadCall, PlayerReturnCode_Failed);

	this->getLogManager() << LogLevel_Trace << "Player initialized.\n";

	m_f64FastForwardMaximumFactor = 0;
	// At this point we've inserted the bridge as a stand-in for Kernel context to the local CConfigurationManager, but the manager in the bridge is still the
	// 'global' one. Now lets change the config manager in the bridge to point to the local manager in order to load configurations into the local manager.
	m_oKernelContextBridge.setConfigurationManager(m_pRuntimeConfigurationManager);

	SchedulerInitializationCode code = m_oScheduler.initialize();

	if (code == SchedulerInitialization_Failed)
	{
		OV_ERROR_K("Failed to initialize player", ErrorType::Internal, PlayerReturnCode_Failed);
	}
	if (code == SchedulerInitialization_BoxInitializationFailed)
	{
		OV_ERROR_K("Failed to initialize player", ErrorType::Internal, PlayerReturnCode_BoxInitializationFailed);
	}

	m_oBenchmarkChrono.reset(uint32_t(m_oScheduler.getFrequency()));

	m_ui64CurrentTimeToReach = 0;
	m_ui64Lateness           = 0;
	m_ui64InnerLateness      = 0;

	m_eStatus = PlayerStatus_Stop;
	return PlayerReturnCode_Sucess;
}

bool CPlayer::uninitialize()
{
	this->getLogManager() << LogLevel_Trace << "Player uninitialize\n";

	m_oScheduler.uninitialize();

	m_oKernelContextBridge.setConfigurationManager(nullptr);

	m_sScenarioConfigurationFile  = "";
	m_sWorkspaceConfigurationFile = "";
	if (m_pRuntimeConfigurationManager != nullptr)
	{
		delete m_pRuntimeConfigurationManager;
		m_pRuntimeConfigurationManager = nullptr;
	}

	return true;
}

bool CPlayer::stop()
{
	OV_ERROR_UNLESS_KRF(this->isHoldingResources(), "Trying to use an uninitialized player", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Trace << "Player stop\n";

	m_eStatus = PlayerStatus_Stop;

	return true;
}

bool CPlayer::pause()
{
	OV_ERROR_UNLESS_KRF(this->isHoldingResources(), "Trying to use an uninitialized player", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Trace << "Player pause\n";

	m_eStatus = PlayerStatus_Pause;

	return true;
}

bool CPlayer::step()
{
	OV_ERROR_UNLESS_KRF(this->isHoldingResources(), "Trying to use an uninitialized player", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Trace << "Player step\n";

	m_eStatus = PlayerStatus_Step;

	return true;
}

bool CPlayer::play()
{
	OV_ERROR_UNLESS_KRF(this->isHoldingResources(), "Trying to use an uninitialized player", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Trace << "Player play\n";

	m_eStatus = PlayerStatus_Play;

	return true;
}

bool CPlayer::forward()
{
	OV_ERROR_UNLESS_KRF(this->isHoldingResources(), "Trying to use an uninitialized player", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Trace << "Player forward\n";

	m_eStatus = PlayerStatus_Forward;

	return true;
}

EPlayerStatus CPlayer::getStatus() const { return m_eStatus; }

bool CPlayer::setFastForwardMaximumFactor(const double f64FastForwardFactor)
{
	m_f64FastForwardMaximumFactor = (f64FastForwardFactor < 0 ? 0 : f64FastForwardFactor);
	return true;
}

double CPlayer::getFastForwardMaximumFactor() const { return m_f64FastForwardMaximumFactor; }

double CPlayer::getCPUUsage() const { return m_oScheduler.getCPUUsage(); }

bool CPlayer::isHoldingResources() const { return m_oScheduler.isHoldingResources(); }

bool CPlayer::loop(const uint64_t ui64ElapsedTime, const uint64_t ui64MaximumTimeToReach)
{
	OV_ERROR_UNLESS_KRF(this->isHoldingResources(), "Trying to use an uninitialized player", ErrorType::BadCall);

	if (m_eStatus == PlayerStatus_Stop) { return true; }

	bool l_bHasTimeToReach = false;
	switch (m_eStatus)
	{
			// Calls a single controller loop and goes back to pause state
		case PlayerStatus_Step:
			m_ui64CurrentTimeToReach += ITimeArithmetics::sampleCountToTime(m_oScheduler.getFrequency(), 1LL);
			l_bHasTimeToReach = true;
			m_eStatus         = PlayerStatus_Pause;
			break;

			// Calls multiple controller loops
		case PlayerStatus_Forward:
			// We can't know what m_ui64CurrentTimeToReach should be in advance
			// We will try to do as many scheduler loops as possible until
			// g_ui64Scheduler_Maximum_Loops_Duration_ seconds elapsed
			if (m_f64FastForwardMaximumFactor != 0)
			{
				m_ui64CurrentTimeToReach += uint64_t(m_f64FastForwardMaximumFactor * ui64ElapsedTime);
				l_bHasTimeToReach = true;
			}
			break;

			// Simply updates time according to delta time
		case PlayerStatus_Play:
			m_ui64CurrentTimeToReach += ui64ElapsedTime;
			l_bHasTimeToReach = true;
			break;

		default: return true;
	}

#if defined CPlayer_Debug_Time
::printf("---\n");
::printf("Factor        : %lf\n", m_f64FastForwardMaximumFactor);
::printf("Current time  : %llx\n", m_oScheduler.getCurrentTime());
::printf("Time to reach : %llx\n", m_ui64CurrentTimeToReach);
#endif // CPlayer_Debug_Time

	uint64_t l_ui64SchedulerStepDuration = m_oScheduler.getStepDuration();
	uint64_t l_ui64StartTime             = System::Time::zgetTime();
	bool l_bFinished                = false;
	while (!l_bFinished)
	{
		uint64_t l_ui64NextSchedulerTime = m_oScheduler.getCurrentTime() + l_ui64SchedulerStepDuration;

#if defined CPlayer_Debug_Time
::printf("    Next time : %llx\n", l_ui64NextSchedulerTime);
#endif // CPlayer_Debug_Time
		if (m_eStatus == PlayerStatus_Stop)
		{
			l_bFinished = true;
		}

		if ((l_bHasTimeToReach && (l_ui64NextSchedulerTime > m_ui64CurrentTimeToReach)) || ((m_eStatus == PlayerStatus_Forward || l_bHasTimeToReach) && (m_oScheduler.getCurrentTime() >= ui64MaximumTimeToReach)))
		{
			l_bFinished = true;
#if defined CPlayer_Debug_Time
::printf("Breaks because would get over time to reach\n");
#endif // CPlayer_Debug_Time
		}
		else
		{
			if (l_bHasTimeToReach) { m_ui64InnerLateness = (m_ui64CurrentTimeToReach > l_ui64NextSchedulerTime ? m_ui64CurrentTimeToReach - l_ui64NextSchedulerTime : 0); }
			else { m_ui64InnerLateness = 0; }

			if (!m_oScheduler.loop())
			{
				m_eStatus = PlayerStatus_Stop;
				this->getLogManager() << LogLevel_Error << "Scheduler loop failed.\n";
				return false;
			}

#if defined CPlayer_Debug_Time
::printf("Iterates (%f / %f - %s)\n", (m_oScheduler.getCurrentTime()>>22)/1024., (ui64MaximumTimeToReach>>22)/1024., (m_eStatus==PlayerStatus_Forward?"true":"false"));
#endif // CPlayer_Debug_Time
		}
		if (System::Time::zgetTime() > l_ui64StartTime + g_ui64Scheduler_Maximum_Loops_Duration_)
		{
			l_bFinished = true;
#if defined CPlayer_Debug_Time
::printf("Breaks because of loop time out\n");
#endif // CPlayer_Debug_Time
		}
	}

	if ((m_eStatus == PlayerStatus_Forward && m_ui64CurrentTimeToReach > m_oScheduler.getCurrentTime() + l_ui64SchedulerStepDuration) || !l_bHasTimeToReach)
	{
		m_ui64CurrentTimeToReach = m_oScheduler.getCurrentTime();
	}

	uint64_t l_ui64Lateness;
	if (m_ui64CurrentTimeToReach > m_oScheduler.getCurrentTime())
	{
		l_ui64Lateness = m_ui64CurrentTimeToReach - m_oScheduler.getCurrentTime();
	}
	else { l_ui64Lateness = 0; }

#if defined CPlayer_Debug_Time
::printf("Done -- New time to reach : %llx\n", m_ui64CurrentTimeToReach);
#endif // CPlayer_Debug_Time

	uint64_t l_ui64LatenessSec = l_ui64Lateness >> 32;
	uint64_t m_ui64LatenessSec = m_ui64Lateness >> 32;
	OV_WARNING_UNLESS_K(l_ui64LatenessSec == m_ui64LatenessSec, "<" << LogColor_PushStateBit << LogColor_ForegroundBlue 
						<< "Player" << LogColor_PopStateBit << "::" << LogColor_PushStateBit << LogColor_ForegroundBlue 
						<< "can not reach realtime" << LogColor_PopStateBit << "> " << l_ui64LatenessSec << " second(s) late...\n");

	return true;
}

uint64_t CPlayer::getCurrentSimulatedTime() const { return m_oScheduler.getCurrentTime(); }
uint64_t CPlayer::getCurrentSimulatedLateness() const { return m_ui64InnerLateness; }

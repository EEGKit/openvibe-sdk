
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
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;


const uint64 g_ui64Scheduler_Default_Frequency_ = 128;
const uint64 g_ui64Scheduler_Maximum_Loops_Duration_ = (100LL << 22); /* 100/1024 seconds, approx 100ms */

//___________________________________________________________________//
//                                                                   //

CPlayer::CPlayer(const IKernelContext& rKernelContext)
	:TKernelObject<IPlayer>(rKernelContext)
	,m_oKernelContextBridge(rKernelContext)
	,m_pRuntimeConfigurationManager(NULL)
	,m_pRuntimeScenarioManager(NULL)
	,m_pScenarioSettingKeywordParserCallback(NULL)
	,m_oScheduler(m_oKernelContextBridge, *this)
	,m_ui64CurrentTimeToReach(0)
	,m_ui64Lateness(0)
	,m_eStatus(PlayerStatus_Uninitialized)
	,m_bIsInitializing(false)
{
	uint64 l_ui64SchedulerFrequency=this->getConfigurationManager().expandAsUInteger("${Kernel_PlayerFrequency}");
	if(l_ui64SchedulerFrequency==0)
	{
		getLogManager() << LogLevel_ImportantWarning << "Invalid frequency configuration " << CString("Kernel_PlayerFrequency") << "=" << this->getConfigurationManager().expand("${Kernel_PlayerFrequency}") << " restored to default " << g_ui64Scheduler_Default_Frequency_ << "\n";
		l_ui64SchedulerFrequency=g_ui64Scheduler_Default_Frequency_;
	}
	else
	{
		getLogManager() << LogLevel_Trace << "Player frequency set to " << l_ui64SchedulerFrequency << "\n";
	}
	m_oScheduler.setFrequency(l_ui64SchedulerFrequency);
}

CPlayer::~CPlayer(void)
{
	if(m_eStatus!=PlayerStatus_Uninitialized)
	{
		this->uninitialize();
	}

	delete m_pRuntimeConfigurationManager;
	delete m_pRuntimeScenarioManager;
}

//___________________________________________________________________//
//                                                                   //

boolean CPlayer::setScenario(
	const CIdentifier& rScenarioIdentifier,
	const OpenViBE::CNameValuePairList* pLocalConfigurationTokens)
{
	if(m_eStatus!=PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to configure an initialized player !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player setScenario\n";

	/*
	// Create a fresh runtime configuration manager which will handle scenario-specific
	// configuration, such as the scenario settings and local settings (workspace)
	if (m_pRuntimeConfigurationManager && m_pScenarioSettingKeywordParserCallback)
	{
		m_pRuntimeConfigurationManager->unregisterKeywordParser(*m_pScenarioSettingKeywordParserCallback);
	}
	*/

	delete m_pRuntimeConfigurationManager;
	m_pRuntimeConfigurationManager=new CConfigurationManager(this->getKernelContext(), &this->getKernelContext().getConfigurationManager());
	m_pRuntimeConfigurationManager->addConfigurationFromFile(this->getKernelContext().getConfigurationManager().expand("${Kernel_DelayedConfiguration}"));
	IScenario& l_rOriginalScenario = this->getScenarioManager().getScenario(rScenarioIdentifier);

	delete m_pRuntimeScenarioManager;
	m_pRuntimeScenarioManager = new CScenarioManager(this->getKernelContext());



	// Create a copy of the scenario
	if (!m_pRuntimeScenarioManager->createScenario(m_oRuntimeScenarioIdentifier))
	{
		return false;
	}

	IScenario& l_rRuntimeScenario = m_pRuntimeScenarioManager->getScenario(m_oRuntimeScenarioIdentifier);
	l_rRuntimeScenario.merge(l_rOriginalScenario, NULL, true);
	// l_rRuntimeScenario.flatten();

	IScenario* l_pScenario = &l_rRuntimeScenario;

	if(l_pScenario)
	{
		if(l_pScenario->hasAttribute(OV_AttributeId_ScenarioFilename))
		{
			std::string l_sFilename = l_pScenario->getAttributeValue(OV_AttributeId_ScenarioFilename).toASCIIString();
			std::string l_sDirectoryName = ".";
			m_pRuntimeConfigurationManager->createConfigurationToken("Player_ScenarioFilename", l_sFilename.c_str());

			size_t iDir=l_sFilename.rfind("/");
			if(iDir!=std::string::npos)
			{
				l_sDirectoryName = l_sFilename.substr(0, iDir).c_str();
			}
			m_pRuntimeConfigurationManager->createConfigurationToken("Player_ScenarioDirectory", l_sDirectoryName.c_str());
			m_pRuntimeConfigurationManager->createConfigurationToken("__volatile_ScenarioDir", l_sDirectoryName.c_str());
			std::string l_sWorkspaceConfigurationFile = l_sDirectoryName + "/" + std::string("openvibe-workspace.conf");
			this->getLogManager() << LogLevel_Trace << "Player adds workspace configuration file [" << CString(l_sWorkspaceConfigurationFile.c_str()) << "] to runtime configuration manager\n";
			m_pRuntimeConfigurationManager->addConfigurationFromFile(CString(l_sWorkspaceConfigurationFile.c_str()));
			std::string l_sScenarioConfigurationFile = l_sDirectoryName + "/" + std::string("scenario.conf");
			this->getLogManager() << LogLevel_Trace << "Player adds scenario configuration file [" << CString(l_sScenarioConfigurationFile.c_str()) << "] to runtime configuration manager\n";
			m_pRuntimeConfigurationManager->addConfigurationFromFile(CString(l_sScenarioConfigurationFile.c_str()));

			size_t iExt = l_sFilename.rfind(".");
			if (iExt != std::string::npos)
			{
				std::string l_sScenarioConfigurationFile = l_sFilename.substr(0, iExt) + std::string(".conf");
				this->getLogManager() << LogLevel_Trace << "Player adds scenario configuration file [" << CString(l_sScenarioConfigurationFile.c_str()) << "] to runtime configuration manager\n";
				m_pRuntimeConfigurationManager->addConfigurationFromFile(CString(l_sScenarioConfigurationFile.c_str()));
			}
			
			// Sets configuration tokens for this player
			// Once every token file, applies the configuration tokens coming from Mensia Player
			if(pLocalConfigurationTokens != NULL)
			{
				this->getLogManager() << LogLevel_Trace << "Player setScenario: add local configuration token from map.\n";
				for(unsigned int i=0; i < pLocalConfigurationTokens->getSize(); i++)
				{
					CString l_sName;
					CString l_sValue;
					if(pLocalConfigurationTokens->getValue(i, l_sName, l_sValue))
					{
						this->getLogManager() << LogLevel_Debug << "Player setScenario: add local configuration token: ["<<l_sName<<"] = ["<<l_sValue<<"].\n";
						CIdentifier l_oTokenIdentifier=m_pRuntimeConfigurationManager->lookUpConfigurationTokenIdentifier(l_sName);
						if(l_oTokenIdentifier==OV_UndefinedIdentifier)
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
						this->getLogManager() << LogLevel_Trace << "Player setScenario: Could not acces to value of pLocalConfigurationTokens at index "<< i <<".\n";
					}
				}
			}

			if(!l_pScenario->checkSettings(m_pRuntimeConfigurationManager) )
			{
				return false;
			}
		}
	}

	return m_oScheduler.setScenario(m_oRuntimeScenarioIdentifier);
}

IConfigurationManager& CPlayer::getRuntimeConfigurationManager(void)
{
	return *m_pRuntimeConfigurationManager;
}

IScenarioManager& CPlayer::getRuntimeScenarioManager(void)
{
	return *m_pRuntimeScenarioManager;
}

EPlayerReturnCode CPlayer::initialize(void)
{
	if(m_eStatus!=PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to initialize an initialized player !\n";
		return PlayerReturnCode_Failed;
	}

	this->getLogManager() << LogLevel_Trace << "Player initialize\n";

	m_f64FastForwardMaximumFactor=0;
	// At this point we've inserted the bridge as a stand-in for Kernel context to the local CConfigurationManager, but the manager in the bridge is still the
	// 'global' one. Now lets change the config manager in the bridge to point to the local manager in order to load configurations into the local manager.
	m_oKernelContextBridge.setConfigurationManager(m_pRuntimeConfigurationManager);

	SchedulerInitializationCode l_eCode = m_oScheduler.initialize();
	if(l_eCode == SchedulerInitialization_Failed)
	{
		this->getLogManager() << LogLevel_Error << "Scheduler initialization failed\n";
		return PlayerReturnCode_Failed;
	}
	m_oBenchmarkChrono.reset(static_cast<uint32>(m_oScheduler.getFrequency()));

	m_ui64CurrentTimeToReach=0;
	m_ui64Lateness=0;
	m_ui64InnerLateness=0;

	m_eStatus=PlayerStatus_Stop;
	if(l_eCode == SchedulerInitialization_BoxInitializationFailed)
	{
		return PlayerReturnCode_BoxInitializationFailed;
	}
	return PlayerReturnCode_Sucess;
}

boolean CPlayer::uninitialize(void)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to uninitialize an uninitialized player !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player uninitialize\n";

	m_oScheduler.uninitialize();

	m_oKernelContextBridge.setConfigurationManager(NULL);

	m_sScenarioConfigurationFile="";
	m_sWorkspaceConfigurationFile="";
	delete m_pRuntimeConfigurationManager;
	m_pRuntimeConfigurationManager=NULL;

	m_eStatus=PlayerStatus_Uninitialized;
	return true;
}

boolean CPlayer::stop(void)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Player has to be initialized before to use it !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player stop\n";

	m_eStatus=PlayerStatus_Stop;

	return true;
}

boolean CPlayer::pause(void)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Player has to be initialized before to use it !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player pause\n";

	m_eStatus=PlayerStatus_Pause;

	return true;
}

boolean CPlayer::step(void)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Player has to be initialized before to use it !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player step\n";

	m_eStatus=PlayerStatus_Step;

	return true;
}

boolean CPlayer::play(void)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Player has to be initialized before to use it !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player play\n";

	m_eStatus=PlayerStatus_Play;

	return true;
}

boolean CPlayer::forward(void)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Player has to be initialized before to use it !\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Player forward\n";

	m_eStatus=PlayerStatus_Forward;

	return true;
}

EPlayerStatus CPlayer::getStatus(void) const
{
	return m_eStatus;
}

boolean CPlayer::setFastForwardMaximumFactor(const float64 f64FastForwardFactor)
{
	m_f64FastForwardMaximumFactor=(f64FastForwardFactor<0?0:f64FastForwardFactor);
	return true;
}

float64 CPlayer::getFastForwardMaximumFactor(void) const
{
	return m_f64FastForwardMaximumFactor;
}

float64 CPlayer::getCPUUsage() const
{
	return m_oScheduler.getCPUUsage();
}

boolean CPlayer::loop(
	const uint64 ui64ElapsedTime,
	const uint64 ui64MaximumTimeToReach)
{
	if(m_eStatus==PlayerStatus_Uninitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Player has to be initialized before to use it !\n";
		return false;
	}

	if(m_oScheduler.isInitializationAborted())
	{
		m_eStatus=PlayerStatus_Stop;
		return false;
	}
	
	if(m_eStatus==PlayerStatus_Stop)
	{
		return false;
	}

	boolean l_bHasTimeToReach=false;
	switch(m_eStatus)
	{
		// Calls a single controller loop and goes back to pause state
		case PlayerStatus_Step:
			m_ui64CurrentTimeToReach += ITimeArithmetics::sampleCountToTime(m_oScheduler.getFrequency(), 1LL);
			l_bHasTimeToReach=true;
			m_eStatus=PlayerStatus_Pause;
			break;

		// Calls multiple controller loops
		case PlayerStatus_Forward:
			// We can't know what m_ui64CurrentTimeToReach should be in advance
			// We will try to do as many scheduler loops as possible until
			// g_ui64Scheduler_Maximum_Loops_Duration_ seconds elapsed
			if(m_f64FastForwardMaximumFactor != 0)
			{
				m_ui64CurrentTimeToReach+=static_cast<uint64>(m_f64FastForwardMaximumFactor*ui64ElapsedTime);
				l_bHasTimeToReach=true;
			}
			break;

		// Simply updates time according to delta time
		case PlayerStatus_Play:
			m_ui64CurrentTimeToReach+=ui64ElapsedTime;
			l_bHasTimeToReach=true;
			break;

		default:
			return true;
			break;
	}

#if defined CPlayer_Debug_Time
::printf("---\n");
::printf("Factor        : %lf\n", m_f64FastForwardMaximumFactor);
::printf("Current time  : %llx\n", m_oScheduler.getCurrentTime());
::printf("Time to reach : %llx\n", m_ui64CurrentTimeToReach);
#endif // CPlayer_Debug_Time

	uint64 l_ui64SchedulerStepDuration=m_oScheduler.getStepDuration();
	uint64 l_ui64StartTime=System::Time::zgetTime();
	boolean l_bFinished=false;
	while(!l_bFinished)
	{
		uint64 l_ui64NextSchedulerTime=m_oScheduler.getCurrentTime() + l_ui64SchedulerStepDuration;

#if defined CPlayer_Debug_Time
::printf("    Next time : %llx\n", l_ui64NextSchedulerTime);
#endif // CPlayer_Debug_Time

		if((l_bHasTimeToReach && (l_ui64NextSchedulerTime > m_ui64CurrentTimeToReach)) || ((m_eStatus == PlayerStatus_Forward || l_bHasTimeToReach) && (m_oScheduler.getCurrentTime() >= ui64MaximumTimeToReach)))
		{
			l_bFinished=true;
#if defined CPlayer_Debug_Time
::printf("Breaks because would get over time to reach\n");
#endif // CPlayer_Debug_Time
		}
		else
		{
			if(l_bHasTimeToReach)
			{
				m_ui64InnerLateness = (m_ui64CurrentTimeToReach > l_ui64NextSchedulerTime ? m_ui64CurrentTimeToReach-l_ui64NextSchedulerTime : 0);
			}
			else
			{
				m_ui64InnerLateness = 0;
			}

			if(!m_oScheduler.loop())
			{
				m_eStatus=PlayerStatus_Stop;
				return false;
			}

#if defined CPlayer_Debug_Time
::printf("Iterates (%f / %f - %s)\n", (m_oScheduler.getCurrentTime()>>22)/1024., (ui64MaximumTimeToReach>>22)/1024., (m_eStatus==PlayerStatus_Forward?"true":"false"));
#endif // CPlayer_Debug_Time
		}
		if(System::Time::zgetTime() > l_ui64StartTime + g_ui64Scheduler_Maximum_Loops_Duration_ )
		{
			l_bFinished=true;
#if defined CPlayer_Debug_Time
::printf("Breaks because of loop time out\n");
#endif // CPlayer_Debug_Time
		}
	}

	if((m_eStatus==PlayerStatus_Forward && m_ui64CurrentTimeToReach > m_oScheduler.getCurrentTime() + l_ui64SchedulerStepDuration) || !l_bHasTimeToReach)
	{
		m_ui64CurrentTimeToReach=m_oScheduler.getCurrentTime();
	}

	uint64 l_ui64Lateness;
	if(m_ui64CurrentTimeToReach>m_oScheduler.getCurrentTime())
	{
		l_ui64Lateness=m_ui64CurrentTimeToReach-m_oScheduler.getCurrentTime();
	}
	else
	{
		l_ui64Lateness=0;
	}

#if defined CPlayer_Debug_Time
::printf("Done -- New time to reach : %llx\n", m_ui64CurrentTimeToReach);
#endif // CPlayer_Debug_Time

	uint64 l_ui64LatenessSec = l_ui64Lateness>>32;
	uint64 m_ui64LatenessSec = m_ui64Lateness>>32;
	if(l_ui64LatenessSec != m_ui64LatenessSec)
	{
		this->getLogManager() << (l_ui64LatenessSec==0?LogLevel_Info:(l_ui64LatenessSec>=10?LogLevel_ImportantWarning:LogLevel_Warning))
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "can not reach realtime" << LogColor_PopStateBit << "> "
			<< l_ui64LatenessSec << " second(s) late...\n";
		m_ui64Lateness=l_ui64Lateness;
	}

	return true;
}

uint64 CPlayer::getCurrentSimulatedTime(void) const
{
	return m_oScheduler.getCurrentTime();
}

uint64 CPlayer::getCurrentSimulatedLateness(void) const
{
	return m_ui64InnerLateness;
}

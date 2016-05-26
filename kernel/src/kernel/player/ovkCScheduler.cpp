
#include <openvibe/ovITimeArithmetics.h>

#include "ovkCScheduler.h"
#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"
#include "../scenario/ovkCScenarioSettingKeywordParserCallback.h"
#include "ovkCBoxSettingModifierVisitor.h"

#include <system/ovCTime.h>
#include <xml/IReader.h>
#include <fs/Files.h>

#include <string>
#include <cstring>


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <cstring>

#if defined TARGET_OS_Windows
 #define stricmp _stricmp
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <strings.h>
 #define stricmp strcasecmp
#else
 // TODO
#endif


//___________________________________________________________________//
//                                                                   //

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

#include <ovp_global_defines.h>
#define OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario    OpenViBE::CIdentifier(0x29574C87, 0x7BA77780)
#define OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer OpenViBE::CIdentifier(0x600463A3, 0x474B7F66)
namespace
{
	CIdentifier openScenario(const IKernelContext& rKernelContext, IScenarioManager& rScenarioManager, const char* sFileName);
}
//___________________________________________________________________//
//                                                                   //

#define OV_AttributeId_Box_Disabled                         OpenViBE::CIdentifier(0x341D3912, 0x1478DE86)

//___________________________________________________________________//
//                                                                   //

CScheduler::CScheduler(const IKernelContext& rKernelContext, CPlayer& rPlayer)
	:TKernelObject<IKernelObject>(rKernelContext)
	,m_rPlayer(rPlayer)
	,m_oScenarioIdentifier(OV_UndefinedIdentifier)
	,m_pScenario(NULL)
	,m_ui64Steps(0)
	,m_ui64Frequency(0)
	,m_ui64StepDuration(0)
	,m_ui64CurrentTime(0)
	,m_bIsInitialized(false)
	,m_bInitializationAborted(false)
{
}

CScheduler::~CScheduler(void)
{
	if(m_bIsInitialized)
	{
		this->uninitialize();
	}
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::setScenario(
	const CIdentifier& rScenarioIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler setScenario\n";

	if(m_bIsInitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to configure an initialized scheduler !\n";
		return false;
	}

	m_oScenarioIdentifier=rScenarioIdentifier;

	// We need to flatten the scenario here as the application using the scheduler needs time
	// between the moment the visualisation tree is complete and the moment when boxes are initialized.
	// The application needs to initialize necessary windows for the boxes to draw into.
	m_pScenario = &m_rPlayer.getRuntimeScenarioManager().getScenario(m_oScenarioIdentifier);

	if(!this->flattenScenario())
	{
		return false;
	}

	m_pScenario=NULL;

	return true;
}

boolean CScheduler::setFrequency(
	const uint64 ui64Frequency)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler setFrequency\n";

	if(m_bIsInitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to configure an initialized scheduler !\n";
		return false;
	}

	m_ui64Frequency=ui64Frequency;
	m_ui64StepDuration=(1LL<<32)/ui64Frequency;
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::flattenScenario()
{
	if(!m_pScenario->applyLocalSettings())
	{
		return false;
	}

	// We are going to find all metaboxes in the scenario and then push their contents to this one
	// As the scenario itself can contain more metaboxes, we are going to repeat this process
	// util there are no unhandled metaboxes left
	bool l_bHasFinishedHandlingMetaboxes = false;

	while (!l_bHasFinishedHandlingMetaboxes)
	{
		// First find all the metaboxes in the scenario
		std::vector<CIdentifier> l_vScenarioMetabox;

		CIdentifier l_oCurrentBoxIdentifier = OV_UndefinedIdentifier;
		CIdentifier l_oPreviousBoxIdentifier = OV_UndefinedIdentifier;
		while ((l_oCurrentBoxIdentifier = m_pScenario->getNextBoxIdentifier(l_oPreviousBoxIdentifier)) != OV_UndefinedIdentifier)
		{
			bool l_bCurrentBoxRemoved = false;
			if(m_pScenario->getBoxDetails(l_oCurrentBoxIdentifier)->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
			{
				// We only process this box if it is not disabled
				bool l_bIsBoxDisabled = m_pScenario->getBoxDetails(l_oCurrentBoxIdentifier)->hasAttribute(OV_AttributeId_Box_Disabled);

				if (l_bIsBoxDisabled)
				{
					m_pScenario->removeBox(l_oCurrentBoxIdentifier);
					l_bCurrentBoxRemoved = true;
				}
				else
				{
					// We verify that the box actually has a backend scenario
					CString l_sMetaboxIdentifier = m_pScenario->getBoxDetails(l_oCurrentBoxIdentifier)->getAttributeValue(OVP_AttributeId_Metabox_Scenario);
					CString l_sMetaboxScenarioPath = this->getKernelContext().getConfigurationManager().lookUpConfigurationTokenValue(CString("Metabox_Scenario_Path_For_") + l_sMetaboxIdentifier);


					if (FS::Files::fileExists(l_sMetaboxScenarioPath.toASCIIString()))
					{
						// If the scenario exists we will handle this metabox
						l_vScenarioMetabox.push_back(l_oCurrentBoxIdentifier);
					}
					else
					{
						// Non-utilisable metaboxes can be easily removed
						this->getKernelContext().getLogManager() << LogLevel_ImportantWarning << "The scenario for metabox [" << l_sMetaboxIdentifier << "] is missing.\n";
						m_pScenario->removeBox(l_oCurrentBoxIdentifier);
						l_bCurrentBoxRemoved = true;
					}
				}
			}
			if (!l_bCurrentBoxRemoved)
			{
				l_oPreviousBoxIdentifier = l_oCurrentBoxIdentifier;
			}
		}
		if (l_vScenarioMetabox.empty())
		{
			l_bHasFinishedHandlingMetaboxes = true;
		}

		// Now load each of the found metaboxes, load the scenario that represents it
		// Assign the settings from the box to the scenario
		// Calculate new settings for each box inside this scenario (by expanding $var settings)
		// Merge the scenario into this one
		// Re-connect all links for the scenario

		for (CIdentifier l_oMetaboxIdentifier : l_vScenarioMetabox)
		{
			IBox* l_pBox = m_pScenario->getBoxDetails(l_oMetaboxIdentifier);

			// The box has an attribute with the metabox ID and config manager has a path to each metabox scenario
			CString l_sMetaboxIdentifier = l_pBox->getAttributeValue(OVP_AttributeId_Metabox_Scenario);
			CString l_sMetaboxScenarioPath = this->getKernelContext().getConfigurationManager().lookUpConfigurationTokenValue(CString("Metabox_Scenario_Path_For_") + l_sMetaboxIdentifier);

			if (l_sMetaboxIdentifier != CString(""))
			{
				// We are going to copy the template scenario, flatten it and then copy all
				// Note that copy constructor for IScenario does not exist

				CIdentifier l_oMetaboxScenarioTemplateIdentifier = openScenario(this->getKernelContext(), m_rPlayer.getRuntimeScenarioManager(), l_sMetaboxScenarioPath);
				IScenario& l_rMetaboxScenarioInstance = m_rPlayer.getRuntimeScenarioManager().getScenario(l_oMetaboxScenarioTemplateIdentifier);

				if (!l_rMetaboxScenarioInstance.hasAttribute(OV_AttributeId_Scenario_MetaboxHash))
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Box " << l_pBox->getName() << " [" << l_sMetaboxScenarioPath << "] has no computed hash\n";
				}
				else if (l_pBox->getAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue) != l_rMetaboxScenarioInstance.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash))
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Box " << l_pBox->getName() << " [" << l_sMetaboxIdentifier << "] should be updated\n";
				}
				l_rMetaboxScenarioInstance.addAttribute(OV_AttributeId_ScenarioFilename, l_sMetaboxScenarioPath);

				// Push down the settings from the box to the scenario
				for (uint32 l_ui32SettingIndex = 0; l_ui32SettingIndex < l_pBox->getSettingCount(); l_ui32SettingIndex++)
				{
					CString l_sSettingValue;
					l_pBox->getSettingValue(l_ui32SettingIndex, l_sSettingValue);

					l_rMetaboxScenarioInstance.setSettingValue(l_ui32SettingIndex, l_sSettingValue);
				}

				// Create settings with the path to the Metabox,
				// these settings will be accessible from within the Metabox on runtime
				std::string l_sMetaboxFilename = l_sMetaboxScenarioPath.toASCIIString();
				std::string l_sMetaboxDirectoryPath = ".";
				l_rMetaboxScenarioInstance.addSetting("Player_MetaboxScenarioFilename", OV_TypeId_Filename, l_sMetaboxScenarioPath);

				size_t l_uiLastSlashPosition = l_sMetaboxFilename.rfind("/");
				if(l_uiLastSlashPosition != std::string::npos)
				{
					l_sMetaboxDirectoryPath = l_sMetaboxFilename.substr(0, l_uiLastSlashPosition).c_str();
				}

				l_rMetaboxScenarioInstance.addSetting("Player_MetaboxScenarioDirectory", OV_TypeId_Foldername, l_sMetaboxDirectoryPath.c_str());

				// apply the settings within the loaded scenario
				l_rMetaboxScenarioInstance.applyLocalSettings();

				std::map<CIdentifier, CIdentifier> l_mIdentifierCorrespondence;
				class CScenarioMergeCallback : public IScenario::IScenarioMergeCallback
				{
				public:
					CScenarioMergeCallback(std::map<CIdentifier, CIdentifier>& rIdentifierCorrespondence)
					    : m_rIdentifierCorrespondence(rIdentifierCorrespondence)
					{
					}

					virtual void process(CIdentifier& rOriginalIdentifier, CIdentifier& rNewIdentifier)
					{
						m_rIdentifierCorrespondence[rOriginalIdentifier] = rNewIdentifier;
					}
				private:
					std::map<CIdentifier, CIdentifier>& m_rIdentifierCorrespondence;

				};

				CScenarioMergeCallback l_oScenarioMergeCallback(l_mIdentifierCorrespondence);

				// Copy the boxes and the links from the template metabox scenario to this one
				m_pScenario->merge(l_rMetaboxScenarioInstance, &l_oScenarioMergeCallback, false);

				// Now reconnect all the pipes

				// Connect metabox inputs
				CIdentifier l_oLinkIdentifier;
				while((l_oLinkIdentifier = m_pScenario->getNextLinkIdentifierToBox(l_oLinkIdentifier, l_pBox->getIdentifier())) != OV_UndefinedIdentifier)
				{
					ILink* l_rLink = m_pScenario->getLinkDetails(l_oLinkIdentifier);

					// Find out the target inside the metabox scenario
					CIdentifier l_oTargetBoxIdentifier;
					uint32 l_ui32TargetBoxInputIdentifier = 0;
					l_rMetaboxScenarioInstance.getScenarioInputLink(l_rLink->getTargetBoxInputIndex(), l_oTargetBoxIdentifier, l_ui32TargetBoxInputIdentifier);

					// Now redirect the link to the newly created copy of the box in the scenario
					l_rLink->setTarget(l_mIdentifierCorrespondence[l_oTargetBoxIdentifier], l_ui32TargetBoxInputIdentifier);
				}

				// Connect metabox outputs
				l_oLinkIdentifier = OV_UndefinedIdentifier;
				while((l_oLinkIdentifier = m_pScenario->getNextLinkIdentifierFromBox(l_oLinkIdentifier, l_pBox->getIdentifier())) != OV_UndefinedIdentifier)
				{
					ILink* l_rLink = m_pScenario->getLinkDetails(l_oLinkIdentifier);
					// Find out from which box this link goes inside the metabox scenario
					CIdentifier l_oSourceBoxIdentifier;
					uint32 l_ui32SourceBoxOutputIdentifier = 0;
					l_rMetaboxScenarioInstance.getScenarioOutputLink(l_rLink->getSourceBoxOutputIndex(), l_oSourceBoxIdentifier, l_ui32SourceBoxOutputIdentifier);

					// Now redirect the link to the newly created copy of the box in the scenario
					l_rLink->setSource(l_mIdentifierCorrespondence[l_oSourceBoxIdentifier], l_ui32SourceBoxOutputIdentifier);
				}

			}
			else
			{
				this->getKernelContext().getLogManager() << LogLevel_Error << "Metabox with id [" << l_sMetaboxIdentifier << "] is not present\n";
				return false;
			}
		}

		// Remove processed metaboxes from the scenario
		for (CIdentifier l_oMetaboxIdentifier : l_vScenarioMetabox)
		{
			m_pScenario->removeBox(l_oMetaboxIdentifier);
		}
	}

	return true;

}

SchedulerInitializationCode CScheduler::initialize(void)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler initialize\n";

	if(m_bIsInitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to initialize an intialized scheduler !\n";
		return SchedulerInitialization_Failed;
	}

	m_pScenario = &m_rPlayer.getRuntimeScenarioManager().getScenario(m_oScenarioIdentifier);
	if(!m_pScenario)
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Scenario " << m_oScenarioIdentifier << " does not exist !\n";
		return SchedulerInitialization_Failed;
	}

	CBoxSettingModifierVisitor l_oBoxSettingModifierVisitor(&this->getKernelContext().getConfigurationManager());
	if(!m_pScenario->acceptVisitor(l_oBoxSettingModifierVisitor))
	{
		this->getLogManager() << LogLevel_Error << "Scenario " << m_oScenarioIdentifier << " setting modification with acceptVisitor() failed\n";
		return SchedulerInitialization_Failed;
	}

	CIdentifier l_oBoxIdentifier;
	while((l_oBoxIdentifier = m_pScenario->getNextBoxIdentifier(l_oBoxIdentifier)) != OV_UndefinedIdentifier)
	{
		int l_iPriority = 0;
		const IBox* l_pBox = m_pScenario->getBoxDetails(l_oBoxIdentifier);

		if(m_pScenario->hasNeedsUpdateBox() && this->getConfigurationManager().expandAsBoolean("${Kernel_AbortPlayerWhenBoxNeedsUpdate}", false))
		{
			this->getLogManager() << LogLevel_ImportantWarning << "Box [" << l_pBox->getName() << "] with class identifier [" << l_oBoxIdentifier << "] should be updated."
				" The token ${Kernel_AbortPlayerWhenBoxNeedsUpdate} was set to true, the scenario will not be played.\n";
			return SchedulerInitialization_Failed;
		}

		if (l_pBox->getAlgorithmClassIdentifier() != OVP_ClassId_BoxAlgorithm_Metabox)
		{
			const IPluginObjectDesc* l_pBoxDesc=this->getPluginManager().getPluginObjectDescCreating(l_pBox->getAlgorithmClassIdentifier());
			if(l_pBoxDesc == NULL)
			{
				this->getLogManager() << LogLevel_Warning << "Disabled box " << l_oBoxIdentifier << " with name " << l_pBox->getName() << " - Plugin object descriptor could not be found\n";
			}
			else if(l_pBoxDesc->hasFunctionality(PluginFunctionality_Visualization))
			{
				this->getLogManager() << LogLevel_Trace << "Disabled box " << l_oBoxIdentifier << " with name " << l_pBox->getName() << " - Box had visualization functionality and such boxes are not handled by Kernel module anymore\n";
			}
			else if(l_pBox->hasAttribute(OV_AttributeId_Box_Disabled))
			{
				this->getLogManager() << LogLevel_Trace << "Disabled box " << l_oBoxIdentifier << " with name " << l_pBox->getName() << " - Box had the " << CString("Disabled") << " attribute\n";
			}
			else
			{
				CSimulatedBox* l_pSimulatedBox=new CSimulatedBox(this->getKernelContext(), *this);
				l_pSimulatedBox->setScenarioIdentifier(m_oScenarioIdentifier);
				l_pSimulatedBox->setBoxIdentifier(l_oBoxIdentifier);
				::sscanf(l_pBox->getAttributeValue(OV_AttributeId_Box_Priority).toASCIIString(), "%i", &l_iPriority);
				m_vSimulatedBox[std::make_pair(-l_iPriority, l_oBoxIdentifier)]=l_pSimulatedBox;
				m_vSimulatedBoxChrono[l_oBoxIdentifier].reset(static_cast<uint32>(m_ui64Frequency));
			}
		}
		else // Metabox
		{
			CString l_sMetaboxIdentifier = l_pBox->getAttributeValue(OVP_AttributeId_Metabox_Scenario);
			//CString l_sMetaboxScenarioPath = this->getKernelContext().getConfigurationManager().lookUpConfigurationTokenValue(CString("Metabox_Scenario_Path_For_") + l_sMetaboxIdentifier);

			this->getKernelContext().getLogManager() << LogLevel_Warning << "A metabox with id [" << l_sMetaboxIdentifier << "] is still in the scenario and I don't know what to do with it\n";
		}
	}

	boolean l_bBoxInitialization = true;
	for(map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); itSimulatedBox++)
	{
		const IBox* l_pBox=m_pScenario->getBoxDetails(itSimulatedBox->first.second);
		this->getLogManager() << LogLevel_Trace << "Scheduled box : id = " << itSimulatedBox->first.second << " priority = " << -itSimulatedBox->first.first << " name = " << l_pBox->getName() << "\n";
		if(itSimulatedBox->second)
		{
			if(!itSimulatedBox->second->initialize())
			{
				l_bBoxInitialization = false;
			}
		}
	}

	m_ui64Steps=0;
	m_ui64CurrentTime=0;
	m_bIsInitialized=true;

	m_oBenchmarkChrono.reset((System::uint32)m_ui64Frequency);

	if(l_bBoxInitialization)
	{
		return SchedulerInitialization_Success;
	}
	return SchedulerInitialization_BoxInitializationFailed;
}

boolean CScheduler::uninitialize(void)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler uninitialize\n";

	if(!m_bIsInitialized)
	{
		this->getLogManager() << LogLevel_Warning << "Trying to uninitialize an uninitialized player !\n";
		return false;
	}

	for(map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); itSimulatedBox++)
	{
		if(itSimulatedBox->second)
		{
			itSimulatedBox->second->uninitialize();
		}
	}

	for(map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); itSimulatedBox++)
	{
		delete itSimulatedBox->second;
	}
	m_vSimulatedBox.clear();

	m_pScenario=NULL;

	m_bIsInitialized=false;
	return true;
}

boolean CScheduler::abortInitialization(void)
{
	m_bInitializationAborted=true;
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::loop(void)
{
	if(!m_bIsInitialized)
	{
		return false;
	}

	m_oBenchmarkChrono.stepIn();
	for(map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); itSimulatedBox++)
	{
		CSimulatedBox* l_pSimulatedBox=itSimulatedBox->second;
		System::CChrono& l_rSimulatedBoxChrono=m_vSimulatedBoxChrono[itSimulatedBox->first.second];

		IBox* l_pBox=m_pScenario->getBoxDetails(itSimulatedBox->first.second);
		if(!l_pBox) {
			this->getLogManager() << LogLevel_Warning << "Unable to get box details for box with id " << itSimulatedBox->first.second << "\n";
			continue;
		}

		l_rSimulatedBoxChrono.stepIn();
		if(l_pSimulatedBox)
		{
			l_pSimulatedBox->processClock();

			if(l_pSimulatedBox->isReadyToProcess())
			{
				// FIXME: test the return code
				l_pSimulatedBox->process();
			}


			//if the box is muted we still have to erase chunks that arrives at the input
			map < uint32, list < CChunk > >& l_rSimulatedBoxInput=m_vSimulatedBoxInput[itSimulatedBox->first.second];
			map < uint32, list < CChunk > >::iterator itSimulatedBoxInput;
			for(itSimulatedBoxInput=l_rSimulatedBoxInput.begin(); itSimulatedBoxInput!=l_rSimulatedBoxInput.end(); itSimulatedBoxInput++)
			{
				list < CChunk >& l_rSimulatedBoxInputChunkList=itSimulatedBoxInput->second;
				list < CChunk >::iterator itSimulatedBoxInputChunkList;
				for(itSimulatedBoxInputChunkList=l_rSimulatedBoxInputChunkList.begin(); itSimulatedBoxInputChunkList!=l_rSimulatedBoxInputChunkList.end(); itSimulatedBoxInputChunkList++)
				{
					l_pSimulatedBox->processInput(itSimulatedBoxInput->first, *itSimulatedBoxInputChunkList);

					if(l_pSimulatedBox->isReadyToProcess())
					{
						l_pSimulatedBox->process();
					}
				}
				l_rSimulatedBoxInputChunkList.clear();
			}
		}
		l_rSimulatedBoxChrono.stepOut();

		if(l_rSimulatedBoxChrono.hasNewEstimation())
		{
			IBox* l_pBox=m_pScenario->getBoxDetails(itSimulatedBox->first.second);
			l_pBox->addAttribute(OV_AttributeId_Box_ComputationTimeLastSecond, "");
			l_pBox->setAttributeValue(OV_AttributeId_Box_ComputationTimeLastSecond, CIdentifier(l_rSimulatedBoxChrono.getTotalStepInDuration()).toString());
		}
	}
	m_oBenchmarkChrono.stepOut();

	if((m_ui64Steps%m_ui64Frequency)==0)
	{
		this->getLogManager() << LogLevel_Debug
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Scheduler" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "elapsed time" << LogColor_PopStateBit << "> "
			<< m_ui64Steps/m_ui64Frequency << "s\n";
	}

	if(m_oBenchmarkChrono.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Scheduler" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "processor use" << LogColor_PopStateBit << "> "
			<< m_oBenchmarkChrono.getStepInPercentage() << "%\n";
	}

	m_ui64Steps++;

	m_ui64CurrentTime=m_ui64Steps*ITimeArithmetics::sampleCountToTime(m_ui64Frequency, 1LL);

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::sendInput(
	const CChunk& rChunk,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32InputIndex)
{
	IBox* l_pBox=m_pScenario->getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		getLogManager() << LogLevel_Warning << "Tried to send data chunk with invalid box identifier " << rBoxIdentifier << "\n";
		return false;
	}

	if(ui32InputIndex >= l_pBox->getInputCount())
	{
		getLogManager() << LogLevel_Warning << "Tried to send data chunk with invalid input index " << ui32InputIndex << " for box identifier" << rBoxIdentifier << "\n";
		return false;
	}
#if 1
	map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin();
	while(itSimulatedBox!=m_vSimulatedBox.end() && itSimulatedBox->first.second != rBoxIdentifier)
	{
		itSimulatedBox++;
	}
	if(itSimulatedBox==m_vSimulatedBox.end())
	{
//		getLogManager() << LogLevel_ImportantWarning << "Tried to send data chunk with valid box identifier but invalid simulated box identifier " << rBoxIdentifier << "\n";
		return false;
	}
#endif
	CSimulatedBox* l_pSimulatedBox=itSimulatedBox->second;
	if(!l_pSimulatedBox)
	{
		getLogManager() << LogLevel_ImportantWarning << "Tried to send data chunk with valid box identifier, valid simulated box identifier " << rBoxIdentifier << " but the box has never been created\n";
		return false;
	}

	// TODO: check if ui32InputIndex does not overflow

	m_vSimulatedBoxInput[rBoxIdentifier][ui32InputIndex].push_back(rChunk);

	return true;
}

uint64 CScheduler::getCurrentTime(void) const
{
	return m_ui64CurrentTime;
}

uint64 CScheduler::getCurrentLateness(void) const
{
	return m_rPlayer.getCurrentSimulatedLateness();
}

uint64 CScheduler::getFrequency(void) const
{
	return m_ui64Frequency;
}

uint64 CScheduler::getStepDuration(void) const
{
	return m_ui64StepDuration;
}

float64 CScheduler::getCPUUsage(void) const
{
	return (const_cast<System::CChrono&>(m_oBenchmarkChrono)).getStepInPercentage();
}

float64 CScheduler::getFastForwardMaximumFactor(void) const
{
	return m_rPlayer.getFastForwardMaximumFactor();
}

namespace
{
	/**
	 * @brief Loads a scenario into the given kernelContext's scenario manager
	 * @param rKernelContext a kernel context for algorithms and logs
	 * @param rScenarioManager the scenario manager to put the scenario in
	 * @param sFileName path to the scenario to load, its type is inferred from the extension
	 * @return CIdentifier of the scenario within the scenario manager
	 */
	CIdentifier openScenario(const IKernelContext& rKernelContext, IScenarioManager& rScenarioManager, const char* sFileName)
	{
		CIdentifier l_oScenarioIdentifier;
		if(rScenarioManager.createScenario(l_oScenarioIdentifier))
		{
			IScenario& l_rScenario = rScenarioManager.getScenario(l_oScenarioIdentifier);

			CMemoryBuffer l_oMemoryBuffer;
//			boolean l_bSuccess = false;

			FILE* l_pFile = FS::Files::open(sFileName, "rb");
			if(l_pFile)
			{
				::fseek(l_pFile, 0, SEEK_END);
				l_oMemoryBuffer.setSize(::ftell(l_pFile), true);
				::fseek(l_pFile, 0, SEEK_SET);
				if(::fread(reinterpret_cast<char*>(l_oMemoryBuffer.getDirectPointer()), (size_t)l_oMemoryBuffer.getSize(), 1, l_pFile)!=1)
				{
					rKernelContext.getLogManager() << LogLevel_Error << "Problem reading '" << sFileName << "'\n";
					::fclose(l_pFile);
					rScenarioManager.releaseScenario(l_oScenarioIdentifier);
					return OV_UndefinedIdentifier;
				}
				::fclose(l_pFile);
			}
			else
			{
				rKernelContext.getLogManager() << LogLevel_Error << "Unable to open '" << sFileName << "' for reading\n";
				rScenarioManager.releaseScenario(l_oScenarioIdentifier);
				return OV_UndefinedIdentifier;
			}

			if(l_oMemoryBuffer.getSize())
			{
				CIdentifier l_oImporterIdentifier = OV_UndefinedIdentifier;

				bool l_bIsFileBinary = false;
				l_bIsFileBinary = (::strlen(sFileName) > 4  && (stricmp(sFileName+strlen(sFileName)-4, ".mbs") == 0 ||
				                                                stricmp(sFileName+strlen(sFileName)-4, ".mbb") == 0));

				// this should be const but can't since default contstructor for Descriptors is not explicitly defined
				//OpenViBEPlugins::Mensia::CAlgorithmBinaryScenarioImporterDesc l_oMensiaBinaryScenarioImporterDesc;
				if(l_bIsFileBinary)
				{
					//l_oImporterIdentifier = rKernelContext.getAlgorithmManager().createAlgorithm(l_oMensiaBinaryScenarioImporterDesc);
				}
				else
				{
					l_oImporterIdentifier = rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_XMLScenarioImporter);
				}

				if(l_oImporterIdentifier!=OV_UndefinedIdentifier)
				{
					IAlgorithmProxy* l_pImporter = &rKernelContext.getAlgorithmManager().getAlgorithm(l_oImporterIdentifier);
					if(l_pImporter)
					{
						l_pImporter->initialize();

						TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(l_pImporter->getInputParameter(OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
						TParameterHandler < IScenario* > op_pScenario(l_pImporter->getOutputParameter(OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario));

						ip_pMemoryBuffer=&l_oMemoryBuffer;
						op_pScenario=&l_rScenario;

						bool l_bSuccess = l_pImporter->process();
						l_pImporter->uninitialize();
						rKernelContext.getAlgorithmManager().releaseAlgorithm(*l_pImporter);

						if (l_bSuccess)
						{
							return l_oScenarioIdentifier;
						}
					}
				}
			}

			rScenarioManager.releaseScenario(l_oScenarioIdentifier);
		}

		rKernelContext.getLogManager() << LogLevel_Warning << "Importing metabox failed\n";
		return OV_UndefinedIdentifier;
	}
}

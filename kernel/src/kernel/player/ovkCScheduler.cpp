
#include <openvibe/ovITimeArithmetics.h>
#include <openvibe/ovExceptionHandler.h>

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
{
}

CScheduler::~CScheduler(void)
{
	this->uninitialize();
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::setScenario(
	const CIdentifier& rScenarioIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler setScenario\n";

	OV_ERROR_UNLESS_KRF(
		!this->isHoldingResources(),
		"Trying to configure a scheduler with non-empty resources",
		ErrorType::BadCall
	);

	m_oScenarioIdentifier=rScenarioIdentifier;

	// We need to flatten the scenario here as the application using the scheduler needs time
	// between the moment the visualisation tree is complete and the moment when boxes are initialized.
	// The application needs to initialize necessary windows for the boxes to draw into.
	m_pScenario = &m_rPlayer.getRuntimeScenarioManager().getScenario(m_oScenarioIdentifier);

	if (!this->flattenScenario())
	{
		// error handling is performed within flattenScenario
		return false;
	}

	m_pScenario=NULL;

	return true;
}

boolean CScheduler::setFrequency(
	const uint64 ui64Frequency)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler setFrequency\n";

	OV_ERROR_UNLESS_KRF(
		!this->isHoldingResources(),
		"Trying to configure a scheduler with non-empty resources",
		ErrorType::BadCall
	);

	m_ui64Frequency=ui64Frequency;
	m_ui64StepDuration=(1LL<<32)/ui64Frequency;
	return true;
}

//___________________________________________________________________//
//                                                                   //

OpenViBE::boolean CScheduler::isHoldingResources() const
{
	return !m_vSimulatedBox.empty();
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::flattenScenario()
{
	OV_ERROR_UNLESS_KRF(
		m_pScenario->applyLocalSettings(),
		"Failed to flatten scenario: applying local settings failed",
		ErrorType::Internal
	);

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
		


		{
			CIdentifier* identifierList = nullptr;
			size_t nbElems = 0;
			m_pScenario->getBoxIdentifierList(&identifierList, &nbElems);
			for (size_t i = 0; i < nbElems; ++i)
			{
				const CIdentifier boxIdentifier = identifierList[i];
				const IBox* box = m_pScenario->getBoxDetails(boxIdentifier);

				if (box->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
				{
					if (box->hasAttribute(OV_AttributeId_Box_Disabled))	// We only process this box if it is not disabled
					{
						m_pScenario->removeBox(boxIdentifier);
					}
					else if (box->hasAttribute(OVP_AttributeId_Metabox_Identifier)) // We verify that the box actually has a backend scenario
					{
						OpenViBE::CIdentifier metaboxId;
						metaboxId.fromString(box->getAttributeValue(OVP_AttributeId_Metabox_Identifier));
						CString metaboxScenarioPath(this->getKernelContext().getMetaboxManager().getMetaboxFilePath(metaboxId));

						if (FS::Files::fileExists(metaboxScenarioPath.toASCIIString()))
						{
							// If the scenario exists we will handle this metabox
							l_vScenarioMetabox.push_back(boxIdentifier);
						}
						else
						{
							// Non-utilisable metaboxes can be easily removed
							this->getKernelContext().getLogManager() << LogLevel_ImportantWarning << "The scenario for metabox [" << metaboxId.toString().toASCIIString() << "] is missing.\n";
							m_pScenario->removeBox(boxIdentifier);
						}
					}
					else
					{
						this->getKernelContext().getLogManager() << LogLevel_ImportantWarning << "The metabox [" << boxIdentifier << "] is missing its identifier field.\n";
						m_pScenario->removeBox(boxIdentifier);
					}
				}
			}
			m_pScenario->releaseIdentifierList(identifierList);
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
			CString l_sMetaboxIdentifier = l_pBox->getAttributeValue(OVP_AttributeId_Metabox_Identifier);
			OpenViBE::CIdentifier metaboxId;
			metaboxId.fromString(l_sMetaboxIdentifier);
			CString l_sMetaboxScenarioPath(this->getKernelContext().getMetaboxManager().getMetaboxFilePath(metaboxId));

			OV_ERROR_UNLESS_KRF(
					l_sMetaboxIdentifier != CString(""),
					"Failed to find metabox with id " << l_sMetaboxIdentifier,
					ErrorType::ResourceNotFound
			);

			// We are going to copy the template scenario, flatten it and then copy all
			// Note that copy constructor for IScenario does not exist
			CIdentifier l_oMetaboxScenarioTemplateIdentifier;

			OV_ERROR_UNLESS_KRF(
			            m_rPlayer.getRuntimeScenarioManager().importScenarioFromFile(
			                l_oMetaboxScenarioTemplateIdentifier,
			                OV_ScenarioImportContext_SchedulerMetaboxImport,
			                l_sMetaboxScenarioPath),
			            "Failed to import the scenario file",
			            ErrorType::Internal
			            );

			IScenario& l_rMetaboxScenarioInstance = m_rPlayer.getRuntimeScenarioManager().getScenario(l_oMetaboxScenarioTemplateIdentifier);

			OV_WARNING_UNLESS_K(
				l_rMetaboxScenarioInstance.hasAttribute(OV_AttributeId_Scenario_MetaboxHash),
				"Box " << l_pBox->getName() << " [" << l_sMetaboxScenarioPath << "] has no computed hash"
			);

			OV_WARNING_UNLESS_K(
				l_pBox->getAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue) == l_rMetaboxScenarioInstance.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash),
				"Box " << l_pBox->getName() << " [" << l_sMetaboxIdentifier << "] should be updated"
			);

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
			if (l_uiLastSlashPosition != std::string::npos)
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
				explicit CScenarioMergeCallback(std::map<CIdentifier, CIdentifier>& rIdentifierCorrespondence)
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
			m_pScenario->merge(l_rMetaboxScenarioInstance, &l_oScenarioMergeCallback, false, false);

			// Now reconnect all the pipes

			// Connect metabox inputs
			{
				CIdentifier* identifierList = nullptr;
				size_t nbElems = 0;
				m_pScenario->getLinkIdentifierToBoxList(l_pBox->getIdentifier(), &identifierList, &nbElems);
				for (size_t i = 0; i < nbElems; ++i)
				{
					ILink* l_rLink = m_pScenario->getLinkDetails(identifierList[i]);
					// Find out the target inside the metabox scenario
					CIdentifier l_oTargetBoxIdentifier;
					uint32_t l_ui32TargetBoxInputIdentifier = 0;
					l_rMetaboxScenarioInstance.getScenarioInputLink(l_rLink->getTargetBoxInputIndex(), l_oTargetBoxIdentifier, l_ui32TargetBoxInputIdentifier);

					// Now redirect the link to the newly created copy of the box in the scenario
					l_rLink->setTarget(l_mIdentifierCorrespondence[l_oTargetBoxIdentifier], l_ui32TargetBoxInputIdentifier);
				}
				m_pScenario->releaseIdentifierList(identifierList);
			}

			// Connect metabox outputs
			{
				CIdentifier* identifierList = nullptr;
				size_t nbElems = 0;
				m_pScenario->getLinkIdentifierFromBoxList(l_pBox->getIdentifier(), &identifierList, &nbElems);
				for (size_t i = 0; i < nbElems; ++i)
				{
					ILink* l_rLink = m_pScenario->getLinkDetails(identifierList[i]);
					// Find out from which box this link goes inside the metabox scenario
					CIdentifier l_oSourceBoxIdentifier;
					uint32_t l_ui32SourceBoxOutputIdentifier = 0;
					l_rMetaboxScenarioInstance.getScenarioOutputLink(l_rLink->getSourceBoxOutputIndex(), l_oSourceBoxIdentifier, l_ui32SourceBoxOutputIdentifier);

					// Now redirect the link to the newly created copy of the box in the scenario
					l_rLink->setSource(l_mIdentifierCorrespondence[l_oSourceBoxIdentifier], l_ui32SourceBoxOutputIdentifier);
				}
				m_pScenario->releaseIdentifierList(identifierList);
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

	OV_ERROR_UNLESS_K(
		!this->isHoldingResources(),
		"Trying to configure a scheduler with non-empty resources",
		ErrorType::BadCall,
		SchedulerInitialization_Failed
	);

	m_pScenario = &m_rPlayer.getRuntimeScenarioManager().getScenario(m_oScenarioIdentifier);

	OV_ERROR_UNLESS_K(
		m_pScenario,
		"Failed to find scenario with id " << m_oScenarioIdentifier.toString(),
		ErrorType::ResourceNotFound,
		SchedulerInitialization_Failed
	);

	OV_ERROR_UNLESS_K(
		m_pScenario->getNextBoxIdentifier(OV_UndefinedIdentifier) != OV_UndefinedIdentifier,
		"Cannot initialize scheduler with an empty scenario",
		ErrorType::BadCall,
		SchedulerInitialization_Failed
	);

	CBoxSettingModifierVisitor l_oBoxSettingModifierVisitor(&this->getKernelContext().getConfigurationManager());

	OV_ERROR_UNLESS_K(
		m_pScenario->acceptVisitor(l_oBoxSettingModifierVisitor),
		"Failed to set box settings visitor for scenario with id " << m_oScenarioIdentifier.toString(),
		ErrorType::Internal,
		SchedulerInitialization_Failed
	);


	{
		CIdentifier* identifierList = nullptr;
		size_t nbElems = 0;
		m_pScenario->getBoxIdentifierList(&identifierList, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const CIdentifier boxIdentifier = identifierList[i];
			const IBox* l_pBox = m_pScenario->getBoxDetails(boxIdentifier);
			OV_ERROR_UNLESS_K(
				!m_pScenario->hasNeedsUpdateBox() || !this->getConfigurationManager().expandAsBoolean("${Kernel_AbortPlayerWhenBoxNeedsUpdate}", false),
				"Box [" << l_pBox->getName() << "] with class identifier [" << boxIdentifier.toString() << "] should be updated",
				ErrorType::Internal,
				SchedulerInitialization_Failed
				);

			OV_ERROR_UNLESS_K(
				l_pBox->getAlgorithmClassIdentifier() != OVP_ClassId_BoxAlgorithm_Metabox,
				"Not expanded metabox with id [" << l_pBox->getAttributeValue(OVP_AttributeId_Metabox_Identifier) << "] detected in the scenario",
				ErrorType::Internal,
				SchedulerInitialization_Failed
				);

			const IPluginObjectDesc* l_pBoxDesc = this->getPluginManager().getPluginObjectDescCreating(l_pBox->getAlgorithmClassIdentifier());

			OV_ERROR_UNLESS_K(
				!(l_pBox->hasAttribute(OV_AttributeId_Box_Disabled) &&
				this->getConfigurationManager().expandAsBoolean("${Kernel_AbortPlayerWhenBoxIsDisabled}", false)),
				"Disabled box [" << l_pBox->getName() << "] with class identifier [" << boxIdentifier.toString() << "] detected in the scenario",
				ErrorType::Internal,
				SchedulerInitialization_Failed
				);

			if (!l_pBox->hasAttribute(OV_AttributeId_Box_Disabled))
			{
				OV_ERROR_UNLESS_K(
					l_pBoxDesc != nullptr,
					"Failed to create runtime box [" << l_pBox->getName() << "] with class identifier [" << boxIdentifier.toString() << "]",
					ErrorType::BadResourceCreation,
					SchedulerInitialization_Failed
					);

				CSimulatedBox* l_pSimulatedBox = new CSimulatedBox(this->getKernelContext(), *this);
				l_pSimulatedBox->setScenarioIdentifier(m_oScenarioIdentifier);
				l_pSimulatedBox->setBoxIdentifier(boxIdentifier);


				int l_iPriority = 0;
				try
				{
					if (l_pBox->hasAttribute(OV_AttributeId_Box_Priority))
					{
						l_iPriority = std::stoi(l_pBox->getAttributeValue(OV_AttributeId_Box_Priority).toASCIIString());
					}
				}
				catch (const std::exception&)
				{
					l_iPriority = 0;
				}

				m_vSimulatedBox[std::make_pair(-l_iPriority, boxIdentifier)] = l_pSimulatedBox;
				m_vSimulatedBoxChrono[boxIdentifier].reset(static_cast<uint32>(m_ui64Frequency));
			}
		}
		m_pScenario->releaseIdentifierList(identifierList);
	}


	bool l_bBoxInitialization = true;
	for (map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); ++itSimulatedBox)
	{
		if (auto l_pSimulatedBox = itSimulatedBox->second)
		{
			this->getLogManager() << LogLevel_Trace << "Scheduled box : id = " << itSimulatedBox->first.second << " priority = " << -itSimulatedBox->first.first << " name = " << l_pSimulatedBox->getName() << "\n";
			if (!translateException(
				[&]() {
					return l_pSimulatedBox->initialize();
				},
				std::bind(&CScheduler::handleException, this, l_pSimulatedBox, "Box initialization", std::placeholders::_1))
			)
			{
				l_bBoxInitialization = false;

				// return as soon as possible
				// no need to keep on initializing boxes if a box failed to initialize
				break;
			}
		}
	}

	m_ui64Steps=0;
	m_ui64CurrentTime=0;

	m_oBenchmarkChrono.reset((System::uint32)m_ui64Frequency);

	return (l_bBoxInitialization ? SchedulerInitialization_Success : SchedulerInitialization_Failed);
}

boolean CScheduler::uninitialize(void)
{
	this->getLogManager() << LogLevel_Trace << "Scheduler uninitialize\n";

	bool l_bBoxUninitialization = true;
	for (map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); ++itSimulatedBox)
	{
		if (auto l_pSimulatedBox = itSimulatedBox->second)
		{
			if (!translateException(
				[&]() {
					return l_pSimulatedBox->uninitialize();
				},
				std::bind(&CScheduler::handleException, this, l_pSimulatedBox, "Box uninitialization", std::placeholders::_1))
			)
			{
				// do not break here because we want to try to
				// at least uninitialize other resources properly
				l_bBoxUninitialization = false;
			}
		}
	}

	for (map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); ++itSimulatedBox)
	{
		delete itSimulatedBox->second;
	}
	m_vSimulatedBox.clear();

	m_pScenario=NULL;

	return l_bBoxUninitialization;
}

//___________________________________________________________________//
//                                                                   //

boolean CScheduler::loop(void)
{
	OV_ERROR_UNLESS_KRF(
		this->isHoldingResources(),
		"Trying to use an uninitialized scheduler",
		ErrorType::BadCall
	);

	bool l_bBoxProcessing = true;
	m_oBenchmarkChrono.stepIn();
	for (map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin(); itSimulatedBox!=m_vSimulatedBox.end(); ++itSimulatedBox)
	{
		CSimulatedBox* l_pSimulatedBox=itSimulatedBox->second;

		System::CChrono& l_rSimulatedBoxChrono=m_vSimulatedBoxChrono[itSimulatedBox->first.second];

		IBox* l_pBox=m_pScenario->getBoxDetails(itSimulatedBox->first.second);

		OV_ERROR_UNLESS_KRF(
			l_pBox,
			"Unable to get box details for box with id " << itSimulatedBox->first.second.toString(),
			ErrorType::ResourceNotFound
		);

		l_rSimulatedBoxChrono.stepIn();

		if (!translateException(
				[&]() {
					return this->processBox(l_pSimulatedBox, itSimulatedBox->first.second);
				},
				std::bind(&CScheduler::handleException, this, l_pSimulatedBox, "Box processing", std::placeholders::_1))
		)
		{
			l_bBoxProcessing = false;

			// break here because we do not want to keep on processing if one
			// box fails
			break;
		}

		l_rSimulatedBoxChrono.stepOut();

		if (l_rSimulatedBoxChrono.hasNewEstimation())
		{
			//IBox* l_pBox=m_pScenario->getBoxDetails(itSimulatedBox->first.second);
			l_pBox->addAttribute(OV_AttributeId_Box_ComputationTimeLastSecond, "");
			l_pBox->setAttributeValue(OV_AttributeId_Box_ComputationTimeLastSecond, CIdentifier(l_rSimulatedBoxChrono.getTotalStepInDuration()).toString());
		}
	}
	m_oBenchmarkChrono.stepOut();

	if ((m_ui64Steps%m_ui64Frequency)==0)
	{
		this->getLogManager() << LogLevel_Debug
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Scheduler" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "elapsed time" << LogColor_PopStateBit << "> "
			<< m_ui64Steps/m_ui64Frequency << "s\n";
	}

	if (m_oBenchmarkChrono.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Scheduler" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "processor use" << LogColor_PopStateBit << "> "
			<< m_oBenchmarkChrono.getStepInPercentage() << "%\n";
	}

	m_ui64Steps++;

	m_ui64CurrentTime=m_ui64Steps*ITimeArithmetics::sampleCountToTime(m_ui64Frequency, 1LL);

	return l_bBoxProcessing;
}

boolean CScheduler::processBox(CSimulatedBox* simulatedBox, const CIdentifier& boxIdentifier)
{
	if (simulatedBox)
	{
		OV_ERROR_UNLESS_KRF(
			simulatedBox->processClock(),
			"Process clock failed for box with id " << boxIdentifier.toString(),
			ErrorType::Internal
		);
		if (simulatedBox->isReadyToProcess())
		{
			OV_ERROR_UNLESS_KRF(
				simulatedBox->process(),
				"Process failed for box with id " << boxIdentifier.toString(),
				ErrorType::Internal
			);
		}

		//if the box is muted we still have to erase chunks that arrives at the input
		map < uint32, list < CChunk > >& l_rSimulatedBoxInput=m_vSimulatedBoxInput[boxIdentifier];
		map < uint32, list < CChunk > >::iterator itSimulatedBoxInput;
		for (itSimulatedBoxInput=l_rSimulatedBoxInput.begin(); itSimulatedBoxInput!=l_rSimulatedBoxInput.end(); ++itSimulatedBoxInput)
		{
			list < CChunk >& l_rSimulatedBoxInputChunkList=itSimulatedBoxInput->second;
			list < CChunk >::iterator itSimulatedBoxInputChunkList;
			for (itSimulatedBoxInputChunkList=l_rSimulatedBoxInputChunkList.begin(); itSimulatedBoxInputChunkList!=l_rSimulatedBoxInputChunkList.end(); ++itSimulatedBoxInputChunkList)
			{
				OV_ERROR_UNLESS_KRF(
					simulatedBox->processInput(itSimulatedBoxInput->first, *itSimulatedBoxInputChunkList),
					"Process failed for box with id " << boxIdentifier.toString() << " on input " << itSimulatedBoxInput->first,
					ErrorType::Internal
				);

				if (simulatedBox->isReadyToProcess())
				{
					OV_ERROR_UNLESS_KRF(
						simulatedBox->process(),
						"Process failed for box with id " << boxIdentifier.toString(),
						ErrorType::Internal
					);
				}
			}
			l_rSimulatedBoxInputChunkList.clear();
		}
	}

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
	if(l_pBox->hasAttribute(OV_AttributeId_Box_Disabled))
	{
		return true;
	}
	OV_ERROR_UNLESS_KRF(
		l_pBox,
		"Tried to send data chunk with invalid box identifier " << rBoxIdentifier.toString(),
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		ui32InputIndex < l_pBox->getInputCount(),
		"Tried to send data chunk with invalid input index " << ui32InputIndex << " for box identifier" << rBoxIdentifier.toString(),
		ErrorType::OutOfBound
	);

	map < pair < int32, CIdentifier >, CSimulatedBox* >::iterator itSimulatedBox=m_vSimulatedBox.begin();
	while(itSimulatedBox!=m_vSimulatedBox.end() && itSimulatedBox->first.second != rBoxIdentifier)
	{
		++itSimulatedBox;
	}

	OV_ERROR_UNLESS_KRF(
		itSimulatedBox != m_vSimulatedBox.end(),
		"Tried to send data chunk with invalid simulated box identifier " << rBoxIdentifier.toString(),
		ErrorType::ResourceNotFound
	);
	CSimulatedBox* l_pSimulatedBox=itSimulatedBox->second;

	// use a fatal here because failing to meet this invariant
	// means there is a bug in the scheduler implementation
	OV_FATAL_UNLESS_K(
		l_pSimulatedBox,
		"Null box found for id " << rBoxIdentifier.toString(),
		ErrorType::BadValue
	);

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

void CScheduler::handleException(const CSimulatedBox* box, const char* errorHint, const std::exception& exception)
{
	CIdentifier l_oTargetBoxIdentifier = OV_UndefinedIdentifier;
	box->getBoxIdentifier(l_oTargetBoxIdentifier);

	box->getLogManager() << LogLevel_Error << "Exception caught in box\n";
	box->getLogManager() << LogLevel_Error << "  [name:" << box->getName() << "]\n";
	box->getLogManager() << LogLevel_Error << "  [class identifier:" << l_oTargetBoxIdentifier << "]\n";
	box->getLogManager() << LogLevel_Error << "  [hint: " << (errorHint ? errorHint : "no hint") << "]\n";
	box->getLogManager() << LogLevel_Error << "  [cause:" << exception.what() << "]\n";

	OV_ERROR_KRV("Caught exception: " << exception.what(), ErrorType::ExceptionCaught);
}

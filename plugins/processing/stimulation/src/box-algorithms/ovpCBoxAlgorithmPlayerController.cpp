#include "ovpCBoxAlgorithmPlayerController.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Stimulation;

bool CBoxAlgorithmPlayerController::initialize()
{
	m_stimulationID = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_actionID      = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_pStreamDecoder = &this->getAlgorithmManager().
							  getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStimulationSet.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	return true;
}

bool CBoxAlgorithmPlayerController::uninitialize()
{
	op_pStimulationSet.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	if (m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder = nullptr;
	}

	return true;
}

bool CBoxAlgorithmPlayerController::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmPlayerController::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		ip_pMemoryBuffer = boxContext.getInputChunk(0, i);
		m_pStreamDecoder->process();
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader)) { }
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			IStimulationSet* l_pStimulationSet = op_pStimulationSet;
			for (size_t j = 0; j < l_pStimulationSet->getStimulationCount(); ++j)
			{
				if (l_pStimulationSet->getStimulationIdentifier(j) == m_stimulationID)
				{
					this->getLogManager() << LogLevel_Trace << "Received stimulation ["
							<< this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, m_stimulationID) <<
							"] causing action ["
							<< this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_PlayerAction, m_actionID) << "]\n";

					bool res = false;
					if (m_actionID == OV_TypeId_PlayerAction_Play) { res = this->getPlayerContext().play(); }
					if (m_actionID == OV_TypeId_PlayerAction_Stop) { res = this->getPlayerContext().stop(); }
					if (m_actionID == OV_TypeId_PlayerAction_Pause) { res = this->getPlayerContext().pause(); }
					if (m_actionID == OV_TypeId_PlayerAction_Forward) { res = this->getPlayerContext().forward(); }

					OV_ERROR_UNLESS_KRF(res,
										"Failed to request player action [" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_PlayerAction,
											m_actionID) << "]",
										OpenViBE::Kernel::ErrorType::BadConfig);
				}
			}
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd)) { }

		boxContext.markInputAsDeprecated(0, i);
	}

	// ...

	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	// l_rStaticBoxContext.getSettingCount();

	// boxContext.getInputChunkCount()
	// boxContext.getInputChunk(i, )
	// boxContext.getOutputChunk(i, )

	return true;
}

#include "ovpCBoxAlgorithmStimulationListener.h"
#include <openvibe/ovITimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Tools;

bool CBoxAlgorithmStimulationListener::initialize()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStimulationDecoder.push_back(new OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationListener>(*this, i));
	}

	m_eLogLevel = static_cast<ELogLevel>((uint64_t)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));

	return true;
}

bool CBoxAlgorithmStimulationListener::uninitialize()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStimulationDecoder[i]->uninitialize();
		delete m_vStimulationDecoder[i];
	}
	m_vStimulationDecoder.clear();

	return true;
}

bool CBoxAlgorithmStimulationListener::processInput(const uint32_t ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationListener::process()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext    = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		for (uint32_t j = 0; j < l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			m_vStimulationDecoder[i]->decode(j);
			if (m_vStimulationDecoder[i]->isHeaderReceived()) { }
			if (m_vStimulationDecoder[i]->isBufferReceived())
			{
				const IStimulationSet* op_pStimulationSet = m_vStimulationDecoder[i]->getOutputStimulationSet();

				CString l_sInputName;
				l_rStaticBoxContext.getInputName(i, l_sInputName);
				for (uint64_t k = 0; k < op_pStimulationSet->getStimulationCount(); k++)
				{
					this->getLogManager() << m_eLogLevel
							<< "For input " << i << " with name " << l_sInputName
							<< " got stimulation " << op_pStimulationSet->getStimulationIdentifier(k)
							<< "[" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, op_pStimulationSet->getStimulationIdentifier(k)) << "]"
							<< " at date " << time64(op_pStimulationSet->getStimulationDate(k))
							<< " and duration " << time64(op_pStimulationSet->getStimulationDuration(k))
							<< "\n";

					OV_WARNING_UNLESS_K(op_pStimulationSet->getStimulationDate(k) >= l_rDynamicBoxContext.getInputChunkStartTime(i, j) && op_pStimulationSet->getStimulationDate(k) <= l_rDynamicBoxContext.getInputChunkEndTime(i, j),
										"Invalid out of range date [" << time64(op_pStimulationSet->getStimulationDate(k)) << "] (expected value between [" << time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << "] and [" << time64(l_rDynamicBoxContext.getInputChunkEndTime(i, j)) << "])");
				}
				/*
				if(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) > 234 && op_pStimulationSet->getStimulationCount()==0)
				{
								this->getLogManager() << LogLevel_Info
							<< "Chunk is empty at ["
							<< time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << ", " << time64(l_rDynamicBoxContext.getInputChunkEndTime(i, j)) << "]\n";
				}
				*/
			}
			if (m_vStimulationDecoder[i]->isEndReceived()) { }
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}

#include "ovpCBoxAlgorithmStimulationListener.h"
#include <openvibe/ovTimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Tools;

bool CBoxAlgorithmStimulationListener::initialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	for (uint32_t i = 0; i < nInput; i++)
	{
		m_vStimulationDecoder.push_back(new OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationListener>(*this, i));
	}

	m_eLogLevel = ELogLevel(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0)));

	return true;
}

bool CBoxAlgorithmStimulationListener::uninitialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	for (uint32_t i = 0; i < nInput; i++)
	{
		m_vStimulationDecoder[i]->uninitialize();
		delete m_vStimulationDecoder[i];
	}
	m_vStimulationDecoder.clear();

	return true;
}

bool CBoxAlgorithmStimulationListener::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationListener::process()
{
	const IBox& staticBoxContext = this->getStaticBoxContext();
	IBoxIO& boxContext           = this->getDynamicBoxContext();
	const size_t nInput          = this->getStaticBoxContext().getInputCount();

	for (uint32_t i = 0; i < nInput; i++)
	{
		for (uint32_t j = 0; j < boxContext.getInputChunkCount(i); j++)
		{
			m_vStimulationDecoder[i]->decode(j);
			if (m_vStimulationDecoder[i]->isHeaderReceived()) { }
			if (m_vStimulationDecoder[i]->isBufferReceived())
			{
				const IStimulationSet* op_pStimulationSet = m_vStimulationDecoder[i]->getOutputStimulationSet();

				CString inputName;
				staticBoxContext.getInputName(i, inputName);
				for (uint64_t k = 0; k < op_pStimulationSet->getStimulationCount(); k++)
				{
					this->getLogManager() << m_eLogLevel
							<< "For input " << i << " with name " << inputName
							<< " got stimulation " << op_pStimulationSet->getStimulationIdentifier(k)
							<< "[" << this->getTypeManager().getEnumerationEntryNameFromValue(
								OV_TypeId_Stimulation, op_pStimulationSet->getStimulationIdentifier(k)) << "]"
							<< " at date " << time64(op_pStimulationSet->getStimulationDate(k))
							<< " and duration " << time64(op_pStimulationSet->getStimulationDuration(k))
							<< "\n";

					OV_WARNING_UNLESS_K(
						op_pStimulationSet->getStimulationDate(k) >= boxContext.getInputChunkStartTime(i, j) && op_pStimulationSet->getStimulationDate(k) <=
						boxContext.getInputChunkEndTime(i, j),
						"Invalid out of range date [" << time64(op_pStimulationSet->getStimulationDate(k)) << "] (expected value between [" << time64(boxContext
							.getInputChunkStartTime(i, j)) << "] and [" << time64(boxContext.getInputChunkEndTime(i, j)) << "])");
				}
				/*
				if(TimeArithmetics::timeToSeconds(boxContext.getInputChunkStartTime(i, j)) > 234 && op_pStimulationSet->getStimulationCount()==0)
				{
					this->getLogManager() << LogLevel_Info << "Chunk is empty at [" << time64(boxContext.getInputChunkStartTime(i, j)) << ", " 
										  << time64(boxContext.getInputChunkEndTime(i, j)) << "]\n";
				}
				*/
			}
			if (m_vStimulationDecoder[i]->isEndReceived()) { }
			boxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}

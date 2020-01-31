#include "ovpCBoxAlgorithmStimulationListener.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Tools;

bool CBoxAlgorithmStimulationListener::initialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	for (size_t i = 0; i < nInput; ++i)
	{
		m_stimulationDecoders.push_back(new Toolkit::TStimulationDecoder<CBoxAlgorithmStimulationListener>(*this, i));
	}

	m_logLevel = ELogLevel(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0)));

	return true;
}

bool CBoxAlgorithmStimulationListener::uninitialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	for (size_t i = 0; i < nInput; ++i)
	{
		m_stimulationDecoders[i]->uninitialize();
		delete m_stimulationDecoders[i];
	}
	m_stimulationDecoders.clear();

	return true;
}

bool CBoxAlgorithmStimulationListener::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationListener::process()
{
	const IBox& staticBoxContext = this->getStaticBoxContext();
	IBoxIO& boxContext           = this->getDynamicBoxContext();
	const size_t nInput          = this->getStaticBoxContext().getInputCount();

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
		{
			m_stimulationDecoders[i]->decode(j);
			if (m_stimulationDecoders[i]->isHeaderReceived()) { }
			if (m_stimulationDecoders[i]->isBufferReceived())
			{
				const IStimulationSet* op_stimulationSet = m_stimulationDecoders[i]->getOutputStimulationSet();

				CString inputName;
				staticBoxContext.getInputName(i, inputName);
				for (size_t k = 0; k < op_stimulationSet->getStimulationCount(); ++k)
				{
					this->getLogManager() << m_logLevel
							<< "For input " << i << " with name " << inputName
							<< " got stimulation " << op_stimulationSet->getStimulationIdentifier(k)
							<< "[" << this->getTypeManager().getEnumerationEntryNameFromValue(
								OV_TypeId_Stimulation, op_stimulationSet->getStimulationIdentifier(k)) << "]"
							<< " at date " << time64(op_stimulationSet->getStimulationDate(k))
							<< " and duration " << time64(op_stimulationSet->getStimulationDuration(k))
							<< "\n";

					OV_WARNING_UNLESS_K(
						op_stimulationSet->getStimulationDate(k) >= boxContext.getInputChunkStartTime(i, j) && op_stimulationSet->getStimulationDate(k) <=
						boxContext.getInputChunkEndTime(i, j),
						"Invalid out of range date [" << time64(op_stimulationSet->getStimulationDate(k)) << "] (expected value between [" << time64(boxContext
							.getInputChunkStartTime(i, j)) << "] and [" << time64(boxContext.getInputChunkEndTime(i, j)) << "])");
				}
			}
			if (m_stimulationDecoders[i]->isEndReceived()) { }
			boxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}

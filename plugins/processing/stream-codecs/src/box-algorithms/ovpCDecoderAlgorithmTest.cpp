#include "ovpCDecoderAlgorithmTest.h"

#include "../algorithms/decoders/ovpCExperimentInfoDecoder.h"
#include "../algorithms/decoders/ovpCFeatureVectorDecoder.h"
#include "../algorithms/decoders/ovpCSignalDecoder.h"
#include "../algorithms/decoders/ovpCSpectrumDecoder.h"
#include "../algorithms/decoders/ovpCStimulationDecoder.h"
#include "../algorithms/decoders/ovpCChannelLocalisationDecoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace StreamCodecs;

ILogManager& operator <<(ILogManager& rLogManager, IMatrix& rMatrix)
{
	rLogManager << "Matrix :\n";
	rLogManager << " | Dimension count : " << rMatrix.getDimensionCount() << "\n";
	for (size_t i = 0; i < rMatrix.getDimensionCount(); ++i)
	{
		rLogManager << " |   Dimension size " << i << " : " << rMatrix.getDimensionSize(i) << "\n";
		for (size_t j = 0; j < rMatrix.getDimensionSize(i); ++j)
		{
			rLogManager << " |     Dimension label " << i << " " << j << " : " << rMatrix.getDimensionLabel(i, j) << "\n";
		}
	}
	return rLogManager;
}


ILogManager& operator <<(ILogManager& rLogManager, IStimulationSet& rStimulationSet)
{
	rLogManager << "Stimulation set :\n";
	rLogManager << " | Number of elements : " << rStimulationSet.getStimulationCount() << "\n";
	for (size_t i = 0; i < rStimulationSet.getStimulationCount(); ++i)
	{
		rLogManager << " |   Stimulation " << i << " : "
				<< "id=" << rStimulationSet.getStimulationIdentifier(i) << " "
				<< "date=" << rStimulationSet.getStimulationDate(i) << " "
				<< "duration=" << rStimulationSet.getStimulationDuration(i) << "\n";
	}
	return rLogManager;
}


bool CDecoderAlgorithmTest::initialize()
{
	m_decoder[0] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ExperimentInfoDecoder));
	m_decoder[1] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_FeatureVectorDecoder));
	m_decoder[2] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalDecoder));
	m_decoder[3] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SpectrumDecoder));
	m_decoder[4] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationDecoder));
	m_decoder[5] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StreamedMatrixDecoder));
	m_decoder[6] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelLocalisationDecoder));

	for (size_t i = 0; i < 7; ++i)
	{
		m_decoder[i]->initialize();
		ip_buffer[i].initialize(m_decoder[i]->getInputParameter(OVP_Algorithm_EBMLDecoder_InputParameterId_MemoryBufferToDecode));
	}

	return true;
}

bool CDecoderAlgorithmTest::uninitialize()
{
	for (size_t i = 0; i < 7; ++i)
	{
		ip_buffer[i].uninitialize();
		m_decoder[i]->uninitialize();
		getAlgorithmManager().releaseAlgorithm(*m_decoder[i]);
		m_decoder[i] = nullptr;
	}

	return true;
}

bool CDecoderAlgorithmTest::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CDecoderAlgorithmTest::process()
{
	IBoxIO& boxContext  = getDynamicBoxContext();
	const size_t nInput = getStaticBoxContext().getInputCount();

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
		{
			ip_buffer[i] = boxContext.getInputChunk(i, j);
			m_decoder[i]->process();

			if (m_decoder[i]->isOutputTriggerActive(OVP_Algorithm_EBMLDecoder_OutputTriggerId_ReceivedHeader))
			{
				{
					TParameterHandler<IMatrix*> handler(m_decoder[i]->getOutputParameter(OVP_Algorithm_StreamedMatrixDecoder_OutputParameterId_Matrix));
					if (handler.exists()) { OV_WARNING_K(*handler); }
				}

				{
					TParameterHandler<IMatrix*> handler(m_decoder[i]->getOutputParameter(OVP_Algorithm_SpectrumDecoder_OutputParameterId_FrequencyAbscissa));
					if (handler.exists()) { OV_WARNING_K(*handler); }
				}

				{
					TParameterHandler<uint64_t> handler(m_decoder[i]->getOutputParameter(OVP_Algorithm_SignalDecoder_OutputParameterId_Sampling));
					if (handler.exists()) { OV_WARNING_K(handler); }
				}
			}

			if (m_decoder[i]->isOutputTriggerActive(OVP_Algorithm_EBMLDecoder_OutputTriggerId_ReceivedBuffer))
			{
				{
					TParameterHandler<IStimulationSet*> handler(m_decoder[i]->getOutputParameter(OVP_Algorithm_StimulationDecoder_OutputParameterId_StimulationSet));
					if (handler.exists()) { getLogManager() << LogLevel_Warning << *handler << "\n"; }
				}
			}

			boxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}

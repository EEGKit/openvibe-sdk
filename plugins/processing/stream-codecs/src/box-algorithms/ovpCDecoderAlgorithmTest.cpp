#include "ovpCDecoderAlgorithmTest.h"

#include "../algorithms/decoders/ovpCExperimentInformationDecoder.h"
#include "../algorithms/decoders/ovpCFeatureVectorDecoder.h"
#include "../algorithms/decoders/ovpCSignalDecoder.h"
#include "../algorithms/decoders/ovpCSpectrumDecoder.h"
#include "../algorithms/decoders/ovpCStimulationDecoder.h"
#include "../algorithms/decoders/ovpCChannelLocalisationDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace StreamCodecs;

ILogManager& operator <<(ILogManager& rLogManager, IMatrix& rMatrix)
{
	rLogManager << "Matrix :\n";
	rLogManager << " | Dimension count : " << rMatrix.getDimensionCount() << "\n";
	for (uint32_t i = 0; i < rMatrix.getDimensionCount(); i++)
	{
		rLogManager << " |   Dimension size " << i << " : " << rMatrix.getDimensionSize(i) << "\n";
		for (uint32_t j = 0; j < rMatrix.getDimensionSize(i); j++)
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
	for (uint64_t i = 0; i < rStimulationSet.getStimulationCount(); i++)
	{
		rLogManager << " |   Stimulation " << i << " : "
				<< "id=" << rStimulationSet.getStimulationIdentifier(i) << " "
				<< "date=" << rStimulationSet.getStimulationDate(i) << " "
				<< "duration=" << rStimulationSet.getStimulationDuration(i) << "\n";
	}
	return rLogManager;
}

CDecoderAlgorithmTest::CDecoderAlgorithmTest() {}

CDecoderAlgorithmTest::~CDecoderAlgorithmTest() {}

bool CDecoderAlgorithmTest::initialize()
{
	m_pStreamDecoder[0] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder));
	m_pStreamDecoder[1] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_FeatureVectorStreamDecoder));
	m_pStreamDecoder[2] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamDecoder[3] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SpectrumStreamDecoder));
	m_pStreamDecoder[4] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStreamDecoder[5] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder));
	m_pStreamDecoder[6] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelLocalisationStreamDecoder));

	for (uint32_t i = 0; i < 7; i++)
	{
		m_pStreamDecoder[i]->initialize();
		ip_pMemoryBuffer[i].initialize(m_pStreamDecoder[i]->getInputParameter(OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode));
	}

	return true;
}

bool CDecoderAlgorithmTest::uininitialize()
{
	for (uint32_t i = 0; i < 7; i++)
	{
		ip_pMemoryBuffer[i].uninitialize();
		m_pStreamDecoder[i]->uninitialize();
		getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder[i]);
		m_pStreamDecoder[i] = nullptr;
	}

	return true;
}

bool CDecoderAlgorithmTest::processInput(const uint32_t index)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CDecoderAlgorithmTest::process()
{
	IBoxIO& boxContext    = getDynamicBoxContext();
	const IBox& l_rStaticBoxContext = getStaticBoxContext();

	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		for (uint32_t j = 0; j < boxContext.getInputChunkCount(i); j++)
		{
			ip_pMemoryBuffer[i] = boxContext.getInputChunk(i, j);
			m_pStreamDecoder[i]->process();

			if (m_pStreamDecoder[i]->isOutputTriggerActive(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				{
					TParameterHandler<IMatrix*> l_oHandle(m_pStreamDecoder[i]->getOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
					if (l_oHandle.exists()) { OV_WARNING_K(*l_oHandle); }
				}

				{
					TParameterHandler<IMatrix*> l_oHandle(m_pStreamDecoder[i]->getOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));
					if (l_oHandle.exists()) { OV_WARNING_K(*l_oHandle); }
				}

				{
					TParameterHandler<uint64_t> l_oHandle(m_pStreamDecoder[i]->getOutputParameter(OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
					if (l_oHandle.exists()) { OV_WARNING_K(l_oHandle); }
				}
			}

			if (m_pStreamDecoder[i]->isOutputTriggerActive(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				{
					TParameterHandler<IStimulationSet*> l_oHandle(m_pStreamDecoder[i]->getOutputParameter(OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
					if (l_oHandle.exists())
					{
						getLogManager() << LogLevel_Warning << *l_oHandle << "\n";
					}
				}
			}

			boxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}

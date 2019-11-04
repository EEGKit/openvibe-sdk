#include "ovpCEncoderAlgorithmTest.h"

#include "../algorithms/encoders/ovpCExperimentInformationEncoder.h"
#include "../algorithms/encoders/ovpCFeatureVectorEncoder.h"
#include "../algorithms/encoders/ovpCSignalEncoder.h"
#include "../algorithms/encoders/ovpCSpectrumEncoder.h"
#include "../algorithms/encoders/ovpCStimulationEncoder.h"
#include "../algorithms/encoders/ovpCChannelLocalisationEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CEncoderAlgorithmTest::initialize()
{
	m_pStreamEncoder[0] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder));
	m_pStreamEncoder[1] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_FeatureVectorStreamEncoder));
	m_pStreamEncoder[2] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder[3] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SpectrumStreamEncoder));
	m_pStreamEncoder[4] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationStreamEncoder));
	m_pStreamEncoder[5] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pStreamEncoder[6] = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder));

	for (uint32_t i = 0; i < 7; ++i)
	{
		m_pStreamEncoder[i]->initialize();
		op_pMemoryBuffer[i].initialize(m_pStreamEncoder[i]->getOutputParameter(OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	}

	m_matrix1 = new CMatrix();
	m_matrix1->setDimensionCount(2);
	m_matrix1->setDimensionSize(0, 16);
	m_matrix1->setDimensionSize(1, 16);
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_matrix1);

	m_matrix2 = new CMatrix();
	m_matrix2->setDimensionCount(1);
	m_matrix2->setDimensionSize(0, 16);
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_matrix2);

	m_matrix3 = new CMatrix();
	m_matrix3->setDimensionCount(2);
	m_matrix3->setDimensionSize(0, 4);
	m_matrix3->setDimensionLabel(0, 0, "C3");
	m_matrix3->setDimensionLabel(0, 1, "Cz");
	m_matrix3->setDimensionLabel(0, 2, "C4");
	m_matrix3->setDimensionLabel(0, 3, "Pz");
	m_matrix3->setDimensionSize(1, 3);
	m_matrix3->setDimensionLabel(1, 0, "x");
	m_matrix3->setDimensionLabel(1, 1, "y");
	m_matrix3->setDimensionLabel(1, 2, "z");
	OpenViBEToolkit::Tools::Matrix::clearContent(*m_matrix3);

	m_pStimulationSet = new CStimulationSet();

	uint64_t m_samplingRate = 16;

	m_pStreamEncoder[1]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_matrix1);
	m_pStreamEncoder[2]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_matrix1);
	m_pStreamEncoder[2]->getInputParameter(OVP_Algorithm_SignalStreamEncoder_InputParameterId_Sampling)->setValue(&m_samplingRate);
	m_pStreamEncoder[3]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_matrix1);
	m_pStreamEncoder[3]->getInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa)->setValue(&m_matrix2);
	m_pStreamEncoder[4]->getInputParameter(OVP_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet)->setValue(&m_pStimulationSet);
	m_pStreamEncoder[5]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_matrix2);
	m_pStreamEncoder[6]->getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)->setValue(&m_matrix3);

	m_hasSentHeader = false;
	m_startTime  = 0;
	m_endTime    = 0;

	return true;
}

bool CEncoderAlgorithmTest::uninitialize()
{
	delete m_pStimulationSet;
	delete m_matrix3;
	delete m_matrix2;
	delete m_matrix1;

	for (uint32_t i = 0; i < 7; ++i)
	{
		op_pMemoryBuffer[i].uninitialize();
		m_pStreamEncoder[i]->uninitialize();
		getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder[i]);
		m_pStreamEncoder[i] = nullptr;
	}

	return true;
}

bool CEncoderAlgorithmTest::processClock(IMessageClock& /*messageClock*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CEncoderAlgorithmTest::process()
{
	IBoxIO& boxContext               = getDynamicBoxContext();
	const IBox& l_rStaticBoxContext  = getStaticBoxContext();
	IPlayerContext& l_rPlayerContext = getPlayerContext();

	if (!m_hasSentHeader)
	{
		m_startTime = 0;
		m_endTime   = 0;
		for (uint32_t i = 0; i < l_rStaticBoxContext.getOutputCount(); ++i)
		{
			op_pMemoryBuffer[i] = boxContext.getOutputChunk(i);
			m_pStreamEncoder[i]->process(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader);
		}
		m_hasSentHeader = true;
	}
	else
	{
		for (uint32_t i = 0; i < l_rStaticBoxContext.getOutputCount(); ++i)
		{
			op_pMemoryBuffer[i] = boxContext.getOutputChunk(i);
			m_pStreamEncoder[i]->process(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer);
		}
	}

	for (uint32_t i = 0; i < l_rStaticBoxContext.getOutputCount(); ++i) { boxContext.markOutputAsReadyToSend(i, m_startTime, m_endTime); }

	m_startTime = m_endTime;
	m_endTime   = l_rPlayerContext.getCurrentTime();

	return true;
}

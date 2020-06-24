#include "ovpCBoxAlgorithmSimpleDSP.h"

#include <system/ovCTime.h>

#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;
using namespace std;

bool CBoxAlgorithmSimpleDSP::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	m_variables = new double*[boxContext.getInputCount()];

	OV_ERROR_UNLESS_KRF(m_variables, "Failed to allocate arrays of floats for [" << boxContext.getInputCount() << "] inputs",
						Kernel::ErrorType::BadAlloc);

	const CString equation = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_parser               = new CEquationParser(*this, m_variables, boxContext.getInputCount());

	OV_ERROR_UNLESS_KRF(m_parser, "Failed to create equation parser", Kernel::ErrorType::BadAlloc);

	OV_ERROR_UNLESS_KRF(m_parser->compileEquation(equation.toASCIIString()), "Failed to compile equation [" << equation << "]",
						Kernel::ErrorType::Internal);

	m_equationType  = m_parser->getTreeCategory();
	m_equationParam = m_parser->getTreeParameter();

	CIdentifier streamType;
	boxContext.getOutputType(0, streamType);

	OV_ERROR_UNLESS_KRF(this->getTypeManager().isDerivedFromStream(streamType, OV_TypeId_StreamedMatrix),
						"Invalid output stream [" << streamType.str() << "] (expected stream must derive from OV_TypeId_StreamedMatrix)",
						Kernel::ErrorType::Internal);

	if (streamType == OV_TypeId_StreamedMatrix)
	{
		m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixEncoder));
		m_encoder->initialize();
		for (size_t i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* decoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixDecoder));
			decoder->initialize();
			m_decoders.push_back(decoder);
		}
	}
	else if (streamType == OV_TypeId_FeatureVector)
	{
		m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorEncoder));
		m_encoder->initialize();
		for (size_t i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* decoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorDecoder));
			decoder->initialize();
			m_decoders.push_back(decoder);
		}
	}
	else if (streamType == OV_TypeId_Signal)
	{
		m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalEncoder));
		m_encoder->initialize();
		for (size_t i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* decoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalDecoder));
			decoder->initialize();
			TParameterHandler<uint64_t> ip_sampling(m_encoder->getInputParameter(OVP_GD_Algorithm_SignalEncoder_InputParameterId_Sampling));
			TParameterHandler<uint64_t> op_sampling(decoder->getOutputParameter(OVP_GD_Algorithm_SignalDecoder_OutputParameterId_Sampling));
			ip_sampling.setReferenceTarget(op_sampling);
			m_decoders.push_back(decoder);
		}
	}
	else if (streamType == OV_TypeId_Spectrum)
	{
		m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumEncoder));
		m_encoder->initialize();
		for (size_t i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* decoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumDecoder));
			decoder->initialize();
			TParameterHandler<CMatrix*> op_CenterBands(m_encoder->getInputParameter(OVP_GD_Algorithm_SpectrumEncoder_InputParameterId_FrequencyAbscissa));
			TParameterHandler<CMatrix*> ip_CenterBands(decoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_FrequencyAbscissa));
			ip_CenterBands.setReferenceTarget(op_CenterBands);
			decoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_Sampling)->setReferenceTarget(
				m_encoder->getInputParameter(OVP_GD_Algorithm_SpectrumEncoder_InputParameterId_Sampling));
			m_decoders.push_back(decoder);
		}
	}
	else
	{
		OV_ERROR_KRF("Type [name=" << this->getTypeManager().getTypeName(streamType) << ":id=" << streamType.str() << "] not yet implemented",
					 Kernel::ErrorType::NotImplemented);
	}

	m_checkDates = this->getConfigurationManager().expandAsBoolean("${Plugin_SignalProcessing_SimpleDSP_CheckChunkDates}", true);
	getLogManager() << LogLevel_Trace << (m_checkDates ? "Checking chunk dates..." : "Not checking chunk dates !") << "\n";

	return true;
}

bool CBoxAlgorithmSimpleDSP::uninitialize()
{
	for (auto& d : m_decoders)
	{
		d->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*d);
	}
	m_decoders.clear();

	if (m_encoder)
	{
		m_encoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_encoder);
		m_encoder = nullptr;
	}

	delete m_parser;
	m_parser = nullptr;

	delete [] m_variables;
	m_variables = nullptr;

	return true;
}

bool CBoxAlgorithmSimpleDSP::processInput(const size_t /*index*/)
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();
	const size_t nInput            = this->getStaticBoxContext().getInputCount();

	if (boxContext.getInputChunkCount(0) == 0) { return true; }

	const CTime tStart = boxContext.getInputChunkStartTime(0, 0);
	const CTime tEnd   = boxContext.getInputChunkEndTime(0, 0);
	for (size_t i = 1; i < nInput; ++i)
	{
		if (boxContext.getInputChunkCount(i) == 0) { return true; }
		if (m_checkDates)
		{
			OV_ERROR_UNLESS_KRF(tStart == boxContext.getInputChunkStartTime(i, 0) || tEnd == boxContext.getInputChunkEndTime(i, 0),
								"Invalid chunk dates (disable this error check by setting Plugin_SignalProcessing_SimpleDSP_CheckChunkDates to false)",
								Kernel::ErrorType::BadInput);
		}
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmSimpleDSP::process()
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();
	const size_t nInput            = this->getStaticBoxContext().getInputCount();

	size_t nHeader = 0;
	size_t nBuffer = 0;
	size_t nEnd    = 0;

	TParameterHandler<CMatrix*> ip_matrix(m_encoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<IMemoryBuffer*> op_buffer(m_encoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_matrices.clear();

	op_buffer = boxContext.getOutputChunk(0);
	for (size_t i = 0; i < nInput; ++i)
	{
		TParameterHandler<const IMemoryBuffer*> ip_buffer(
			m_decoders[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<CMatrix*> op_matrix(m_decoders[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputParameterId_Matrix));
		ip_buffer = boxContext.getInputChunk(i, 0);
		m_decoders[i]->process();
		if (m_decoders[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedHeader))
		{
			if (i != 0)
			{
				OV_ERROR_UNLESS_KRF(m_matrices[0]->getSize() == op_matrix->getSize(),
									"Invalid matrix dimension [" << m_matrices[0]->getSize() << "] (expected value = "
									<< op_matrix->getSize() <<")", Kernel::ErrorType::BadValue);
			}
			nHeader++;
		}
		if (m_decoders[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedBuffer)) { nBuffer++; }
		if (m_decoders[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedEnd)) { nEnd++; }
		m_matrices.push_back(op_matrix);
		boxContext.markInputAsDeprecated(i, 0);
	}

	OV_ERROR_UNLESS_KRF((!nHeader || nHeader == nInput) && (!nBuffer || nBuffer == nInput) && (!nEnd || nEnd == nInput),
						"Invalid stream structure", Kernel::ErrorType::BadValue);

	if (nHeader)
	{
		Toolkit::Matrix::copyDescription(*ip_matrix, *m_matrices[0]);
		m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeHeader);
	}
	if (nBuffer)
	{
		this->evaluate();
		m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeBuffer);
	}
	if (nEnd) { m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeEnd); }

	if (nHeader || nBuffer || nEnd) { boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, 0), boxContext.getInputChunkEndTime(0, 0)); }

	return true;
}

void CBoxAlgorithmSimpleDSP::evaluate()
{
	const IBox& boxContext = this->getStaticBoxContext();

	for (size_t i = 0; i < boxContext.getInputCount(); ++i) { m_variables[i] = m_matrices[i]->getBuffer(); }

	TParameterHandler<CMatrix*> ip_pMatrix(m_encoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	double* buffer    = ip_pMatrix->getBuffer();
	double* bufferEnd = ip_pMatrix->getBuffer() + ip_pMatrix->getSize();

	while (buffer != bufferEnd)
	{
		*buffer = m_parser->executeEquation();
		for (size_t i = 0; i < boxContext.getInputCount(); ++i) { m_variables[i]++; }
		buffer++;
	}
}

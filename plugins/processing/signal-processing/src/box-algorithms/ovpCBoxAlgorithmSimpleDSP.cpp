#include "ovpCBoxAlgorithmSimpleDSP.h"

#include <system/ovCTime.h>

#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace std;

CBoxAlgorithmSimpleDSP::CBoxAlgorithmSimpleDSP() {}

bool CBoxAlgorithmSimpleDSP::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();
	uint32_t i;

	m_Variable = new double*[boxContext.getInputCount()];

	OV_ERROR_UNLESS_KRF(m_Variable, "Failed to allocate arrays of floats for [" << boxContext.getInputCount() << "] inputs",
						OpenViBE::Kernel::ErrorType::BadAlloc);

	const CString l_sEquation = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_Parser                  = new CEquationParser(*this, m_Variable, boxContext.getInputCount());

	OV_ERROR_UNLESS_KRF(m_Parser, "Failed to create equation parser", OpenViBE::Kernel::ErrorType::BadAlloc);

	OV_ERROR_UNLESS_KRF(m_Parser->compileEquation(l_sEquation.toASCIIString()), "Failed to compile equation [" << l_sEquation << "]",
						OpenViBE::Kernel::ErrorType::Internal);

	m_EquationType         = m_Parser->getTreeCategory();
	m_SpecialEquationParam = m_Parser->getTreeParameter();

	CIdentifier l_oStreamType;
	boxContext.getOutputType(0, l_oStreamType);

	OV_ERROR_UNLESS_KRF(this->getTypeManager().isDerivedFromStream(l_oStreamType, OV_TypeId_StreamedMatrix),
						"Invalid output stream [" << l_oStreamType.toString() << "] (expected stream must derive from OV_TypeId_StreamedMatrix)",
						OpenViBE::Kernel::ErrorType::Internal);

	if (l_oStreamType == OV_TypeId_StreamedMatrix)
	{
		m_Encoder = &this->getAlgorithmManager().getAlgorithm(
			this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
		m_Encoder->initialize();
		for (i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* l_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
			l_pStreamDecoder->initialize();
			m_Decoder.push_back(l_pStreamDecoder);
		}
	}
	else if (l_oStreamType == OV_TypeId_FeatureVector)
	{
		m_Encoder = &this->getAlgorithmManager().getAlgorithm(
			this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
		m_Encoder->initialize();
		for (i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* l_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
			l_pStreamDecoder->initialize();
			m_Decoder.push_back(l_pStreamDecoder);
		}
	}
	else if (l_oStreamType == OV_TypeId_Signal)
	{
		m_Encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
		m_Encoder->initialize();
		for (i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* l_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
			l_pStreamDecoder->initialize();
			TParameterHandler<uint64_t> ip_ui64SamplingRate(
				m_Encoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
			TParameterHandler<uint64_t> op_ui64SamplingRate(
				l_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
			ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);
			m_Decoder.push_back(l_pStreamDecoder);
		}
	}
	else if (l_oStreamType == OV_TypeId_Spectrum)
	{
		m_Encoder = &this->getAlgorithmManager().getAlgorithm(
			this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
		m_Encoder->initialize();
		for (i = 0; i < boxContext.getInputCount(); ++i)
		{
			IAlgorithmProxy* l_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(
				this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
			l_pStreamDecoder->initialize();
			TParameterHandler<IMatrix*> op_ui64CenterFrequencyBands(
				m_Encoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa));
			TParameterHandler<IMatrix*> ip_ui64CenterFrequencyBands(
				l_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));
			ip_ui64CenterFrequencyBands.setReferenceTarget(op_ui64CenterFrequencyBands);
			l_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate)->setReferenceTarget(
				m_Encoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate));
			m_Decoder.push_back(l_pStreamDecoder);
		}
	}
	else
	{
		OV_ERROR_KRF("Type [name=" << this->getTypeManager().getTypeName(l_oStreamType) << ":id=" << l_oStreamType.toString() << "] not yet implemented",
					 OpenViBE::Kernel::ErrorType::NotImplemented);
	}

	m_CheckChunkDates = this->getConfigurationManager().expandAsBoolean("${Plugin_SignalProcessing_SimpleDSP_CheckChunkDates}", true);
	this->getLogManager() << LogLevel_Trace << (m_CheckChunkDates ? "Checking chunk dates..." : "Not checking chunk dates !") << "\n";

	return true;
}

bool CBoxAlgorithmSimpleDSP::uninitialize()
{
	for (auto it = m_Decoder.begin(); it != m_Decoder.end(); ++it)
	{
		(*it)->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(**it);
	}
	m_Decoder.clear();

	if (m_Encoder)
	{
		m_Encoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_Encoder);
		m_Encoder = nullptr;
	}

	delete m_Parser;
	m_Parser = nullptr;

	delete [] m_Variable;
	m_Variable = nullptr;

	return true;
}

bool CBoxAlgorithmSimpleDSP::processInput(const uint32_t /*index*/)
{
	IDynamicBoxContext& boxContext = this->getDynamicBoxContext();
	const uint32_t nInput          = this->getStaticBoxContext().getInputCount();

	if (boxContext.getInputChunkCount(0) == 0) { return true; }

	const uint64_t tStart = boxContext.getInputChunkStartTime(0, 0);
	const uint64_t tEnd   = boxContext.getInputChunkEndTime(0, 0);
	for (uint32_t i = 1; i < nInput; ++i)
	{
		if (boxContext.getInputChunkCount(i) == 0) { return true; }
		if (m_CheckChunkDates)
		{
			OV_ERROR_UNLESS_KRF(tStart == boxContext.getInputChunkStartTime(i, 0) || tEnd == boxContext.getInputChunkEndTime(i, 0),
								"Invalid chunk dates (disable this error check by setting Plugin_SignalProcessing_SimpleDSP_CheckChunkDates to false)",
								OpenViBE::Kernel::ErrorType::BadInput);
		}
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmSimpleDSP::process()
{
	IDynamicBoxContext& boxContext = this->getDynamicBoxContext();
	const uint32_t nInput          = this->getStaticBoxContext().getInputCount();

	uint32_t nHeader = 0;
	uint32_t nBuffer = 0;
	uint32_t nEnd    = 0;

	TParameterHandler<IMatrix*> ip_pMatrix(m_Encoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	TParameterHandler<IMemoryBuffer*> op_pMemoryBuffer(m_Encoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_Matrix.clear();

	op_pMemoryBuffer = boxContext.getOutputChunk(0);
	for (uint32_t i = 0; i < nInput; ++i)
	{
		TParameterHandler<const IMemoryBuffer*> ip_pMemoryBuffer(m_Decoder[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<IMatrix*> op_pMatrix(m_Decoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
		ip_pMemoryBuffer = boxContext.getInputChunk(i, 0);
		m_Decoder[i]->process();
		if (m_Decoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			if (i != 0)
			{
				OV_ERROR_UNLESS_KRF(m_Matrix[0]->getBufferElementCount() == op_pMatrix->getBufferElementCount(),
									"Invalid matrix dimension [" << m_Matrix[0]->getBufferElementCount() << "] (expected value = " << op_pMatrix->
									getBufferElementCount() <<")",
									OpenViBE::Kernel::ErrorType::BadValue);
			}
			nHeader++;
		}
		if (m_Decoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer)) { nBuffer++; }
		if (m_Decoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd)) { nEnd++; }
		m_Matrix.push_back(op_pMatrix);
		boxContext.markInputAsDeprecated(i, 0);
	}

	OV_ERROR_UNLESS_KRF((!nHeader || nHeader == nInput) &&
						(!nBuffer || nBuffer == nInput) &&
						(!nEnd || nEnd == nInput),
						"Invalid stream structure", OpenViBE::Kernel::ErrorType::BadValue);

	if (nHeader)
	{
		OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_pMatrix, *m_Matrix[0]);
		m_Encoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
	}
	if (nBuffer)
	{
		this->evaluate();
		m_Encoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
	}
	if (nEnd) { m_Encoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd); }

	if (nHeader || nBuffer || nEnd) { boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, 0), boxContext.getInputChunkEndTime(0, 0)); }

	return true;
}

void CBoxAlgorithmSimpleDSP::evaluate()
{
	const IBox& boxContext = this->getStaticBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputCount(); ++i) { m_Variable[i] = m_Matrix[i]->getBuffer(); }

	TParameterHandler<IMatrix*> ip_pMatrix(m_Encoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	double* buffer    = ip_pMatrix->getBuffer();
	double* bufferEnd = ip_pMatrix->getBuffer() + ip_pMatrix->getBufferElementCount();

	while (buffer != bufferEnd)
	{
		*buffer = m_Parser->executeEquation();

		for (uint32_t i = 0; i < boxContext.getInputCount(); ++i) { m_Variable[i]++; }

		buffer++;
	}

#if 0
	switch (m_EquationType)
	{
		//The equation is not a special one, we have to execute the whole stack of function calls
		case OP_USERDEF:
			//for every samples
			for (uint64_t i = 0; i < m_matrixBufferSize; ++i)
			{
				m_f64Variable = buffer[i];
				m_matrixBuffer[i] = m_Parser->executeEquation();
			}
			break;
		case OP_X2: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = buffer[i] * buffer[i]; } break;
		case OP_NONE: System::Memory::copy(m_matrixBuffer, buffer, m_matrixBufferSize * sizeof(double)); break;
		case OP_ABS: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = abs(buffer[i]); } break;
		case OP_ACOS: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = acos(buffer[i]); } break;
		case OP_ASIN: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = asin(buffer[i]); } break;
		case OP_ATAN: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = atan(buffer[i]); } break;
		case OP_CEIL: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = ceil(buffer[i]); } break;
		case OP_COS: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = cos(buffer[i]); } break;
		case OP_EXP: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = exp(buffer[i]); } break;
		case OP_FLOOR: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = floor(buffer[i]); } break;
		case OP_LOG: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = log(buffer[i]); } break;
		case OP_LOG10: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = log10(buffer[i]); } break;
		case OP_SIN: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = sin(buffer[i]); } break;
		case OP_SQRT: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = sqrt(buffer[i]); } break;
		case OP_TAN: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = tan(buffer[i]); } break;
		case OP_ADD: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = buffer[i] + m_SpecialEquationParam; } break;
		case OP_MUL: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = buffer[i] * m_SpecialEquationParam; } break;
		case OP_DIV: for (uint64_t i = 0; i < m_matrixBufferSize; ++i) { m_matrixBuffer[i] = buffer[i] / m_SpecialEquationParam; } break;
	}
#endif
}

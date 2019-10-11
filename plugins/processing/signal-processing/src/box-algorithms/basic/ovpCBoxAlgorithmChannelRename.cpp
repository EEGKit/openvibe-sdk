#include "ovpCBoxAlgorithmChannelRename.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmChannelRename::initialize()
{
	std::vector<CString> tokens;
	const CString settingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const uint32_t tokenCount  = split(settingValue, OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(tokens),
									   OV_Value_EnumeratedStringSeparator);

	m_names.clear();
	for (uint32_t i = 0; i < tokenCount; ++i) { m_names.push_back(tokens[i].toASCIIString()); }

	this->getStaticBoxContext().getOutputType(0, m_typeID);

	if (m_typeID == OV_TypeId_Signal)
	{
		m_decoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_encoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix || m_typeID == OV_TypeId_TimeFrequency)
	{
		m_decoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_encoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		m_decoder = new OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_encoder = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else { OV_ERROR_KRF("Incompatible stream type", ErrorType::BadConfig); }

	ip_Matrix = m_encoder.getInputMatrix();
	op_Matrix = m_decoder.getOutputMatrix();

	m_encoder.getInputMatrix().setReferenceTarget(m_decoder.getOutputMatrix());

	if (m_typeID == OV_TypeId_Signal) { m_encoder.getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate()); }

	if (m_typeID == OV_TypeId_Spectrum)
	{
		m_encoder.getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate());
		m_encoder.getInputFrequencyAbcissa().setReferenceTarget(m_decoder.getOutputFrequencyAbcissa());
	}

	return true;
}

bool CBoxAlgorithmChannelRename::uninitialize()
{
	m_decoder.uninitialize();
	m_encoder.uninitialize();

	return true;
}

bool CBoxAlgorithmChannelRename::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmChannelRename::process()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(0); ++chunk)
	{
		m_decoder.decode(chunk);
		if (m_decoder.isHeaderReceived())
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_Matrix, *op_Matrix);
			for (uint32_t channel = 0; channel < ip_Matrix->getDimensionSize(0) && channel < m_names.size(); ++channel)
			{
				ip_Matrix->setDimensionLabel(0, channel, m_names[channel].c_str());
			}
			m_encoder.encodeHeader();
		}
		if (m_decoder.isBufferReceived()) { m_encoder.encodeBuffer(); }
		if (m_decoder.isEndReceived()) { m_encoder.encodeEnd(); }

		dynamicBoxContext.markOutputAsReadyToSend(0, dynamicBoxContext.getInputChunkStartTime(0, chunk), dynamicBoxContext.getInputChunkEndTime(0, chunk));
		dynamicBoxContext.markInputAsDeprecated(0, chunk);
	}

	return true;
}

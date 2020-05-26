#include "ovpCBoxAlgorithmChannelRename.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

bool CBoxAlgorithmChannelRename::initialize()
{
	std::vector<CString> tokens;
	const CString setting = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const size_t nToken   = split(setting, Toolkit::String::TSplitCallback<std::vector<CString>>(tokens), OV_Value_EnumeratedStringSeparator);

	m_names.clear();
	for (size_t i = 0; i < nToken; ++i) { m_names.push_back(tokens[i].toASCIIString()); }

	this->getStaticBoxContext().getOutputType(0, m_typeID);

	if (m_typeID == OV_TypeId_Signal)
	{
		m_decoder = new Toolkit::TSignalDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_encoder = new Toolkit::TSignalEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix || m_typeID == OV_TypeId_TimeFrequency)
	{
		m_decoder = new Toolkit::TStreamedMatrixDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_encoder = new Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		m_decoder = new Toolkit::TSpectrumDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_encoder = new Toolkit::TSpectrumEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else { OV_ERROR_KRF("Incompatible stream type", Kernel::ErrorType::BadConfig); }

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
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t chunk = 0; chunk < boxContext.getInputChunkCount(0); ++chunk)
	{
		m_decoder.decode(chunk);
		if (m_decoder.isHeaderReceived())
		{
			Toolkit::Matrix::copyDescription(*ip_Matrix, *op_Matrix);
			for (size_t channel = 0; channel < ip_Matrix->getDimensionSize(0) && channel < m_names.size(); ++channel)
			{
				ip_Matrix->setDimensionLabel(0, channel, m_names[channel].c_str());
			}
			m_encoder.encodeHeader();
		}
		if (m_decoder.isBufferReceived()) { m_encoder.encodeBuffer(); }
		if (m_decoder.isEndReceived()) { m_encoder.encodeEnd(); }

		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, chunk), boxContext.getInputChunkEndTime(0, chunk));
		boxContext.markInputAsDeprecated(0, chunk);
	}

	return true;
}

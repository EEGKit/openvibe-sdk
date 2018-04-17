#include "ovpCBoxAlgorithmChannelRename.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

bool CBoxAlgorithmChannelRename::initialize(void)
{
	std::vector<CString> tokens;
	CString settingValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	uint32 tokenCount = OpenViBEToolkit::Tools::String::split(settingValue, OpenViBEToolkit::Tools::String::TSplitCallback < std::vector < CString > > (tokens), OV_Value_EnumeratedStringSeparator);

	m_ChannelNames.clear();
	for (uint32_t i = 0; i < tokenCount; i++)
	{
		m_ChannelNames.push_back(tokens[i].toASCIIString());
	}

	this->getStaticBoxContext().getOutputType(0, m_TypeIdentifier);

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_StreamDecoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_StreamEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix || m_TypeIdentifier == OV_TypeId_TimeFrequency)
	{
		m_StreamDecoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_StreamEncoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else if (m_TypeIdentifier == OV_TypeId_Spectrum)
	{
		m_StreamDecoder = new OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmChannelRename>(*this, 0);
		m_StreamEncoder = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmChannelRename>(*this, 0);
	}
	else
	{
		OV_ERROR_KRF("Incompatible stream type", ErrorType::BadConfig);
	}

	ip_Matrix = m_StreamEncoder.getInputMatrix();
	op_Matrix = m_StreamDecoder.getOutputMatrix();

	m_StreamEncoder.getInputMatrix().setReferenceTarget(m_StreamDecoder.getOutputMatrix());

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_StreamEncoder.getInputSamplingRate().setReferenceTarget(m_StreamDecoder.getOutputSamplingRate());
	}

	return true;
}

bool CBoxAlgorithmChannelRename::uninitialize(void)
{
	m_StreamDecoder.uninitialize();
	m_StreamEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmChannelRename::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmChannelRename::process(void)
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for(uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(0); chunk++)
	{
		m_StreamDecoder.decode(chunk);
		if(m_StreamDecoder.isHeaderReceived())
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_Matrix, *op_Matrix);
			for (uint32_t channel = 0; channel < ip_Matrix->getDimensionSize(0) && channel < m_ChannelNames.size(); channel++)
			{
				ip_Matrix->setDimensionLabel(0, channel, m_ChannelNames[channel].c_str());
			}
			m_StreamEncoder.encodeHeader();
		}
		if(m_StreamDecoder.isBufferReceived())
		{
			m_StreamEncoder.encodeBuffer();
		}
		if (m_StreamDecoder.isEndReceived())
		{
			m_StreamEncoder.encodeEnd();
		}

		dynamicBoxContext.markOutputAsReadyToSend(0, dynamicBoxContext.getInputChunkStartTime(0, chunk), dynamicBoxContext.getInputChunkEndTime(0, chunk));
		dynamicBoxContext.markInputAsDeprecated(0, chunk);
	}

	return true;
}

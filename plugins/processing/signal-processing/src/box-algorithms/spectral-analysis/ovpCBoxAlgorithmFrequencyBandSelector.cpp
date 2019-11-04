#include "ovpCBoxAlgorithmFrequencyBandSelector.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

#include <vector>
#include <string>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)<(b)?(b):(a))

namespace
{
	std::vector<std::string> split(const std::string& sString, const char c)
	{
		std::vector<std::string> result;
		size_t i = 0;
		while (i < sString.length())
		{
			size_t j = i;
			while (j < sString.length() && sString[j] != c) { j++; }
			if (i != j) { result.push_back(std::string(sString, i, j - i)); }
			i = j + 1;
		}
		return result;
	}
} // namespace

bool CBoxAlgorithmFrequencyBandSelector::initialize()
{
	const CString settingValue       = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	std::vector<std::string> setting = split(settingValue.toASCIIString(), OV_Value_EnumeratedStringSeparator);
	bool hadError                    = false;
	CString errorMsg;
	m_vSelected.clear();
	for (auto it = setting.begin(); it != setting.end(); ++it)
	{
		bool good                             = true;
		std::vector<std::string> settingRange = split(*it, OV_Value_RangeStringSeparator);
		if (settingRange.size() == 1)
		{
			try
			{
				double l_dValue = std::stod(settingRange[0].c_str());
				m_vSelected.push_back(std::pair<double, double>(l_dValue, l_dValue));
			}
			catch (const std::exception&) { good = false; }
		}
		else if (settingRange.size() == 2)
		{
			try
			{
				double low  = std::stod(settingRange[0].c_str());
				double high = std::stod(settingRange[1].c_str());
				m_vSelected.push_back(std::pair<double, double>(min(low, high), max(low, high)));
			}
			catch (const std::exception&) { good = false; }
		}

		if (!good)
		{
			errorMsg = CString("Invalid frequency band [") + it->c_str() + "]";
			hadError = true;
		}
	}

	m_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
	op_pBands.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));

	m_pStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix));
	ip_pFrequencyAbscissa.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	ip_pFrequencyAbscissa.setReferenceTarget(op_pBands);
	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Sampling)->setReferenceTarget(
		m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Sampling));

	ip_pMatrix = &m_oMatrix;
	op_pMatrix = &m_oMatrix;

	OV_ERROR_UNLESS_KRF(!hadError || !m_vSelected.empty(), errorMsg, OpenViBE::Kernel::ErrorType::BadSetting);

	return true;
}

bool CBoxAlgorithmFrequencyBandSelector::uninitialize()
{
	op_pMemoryBuffer.uninitialize();
	ip_pFrequencyAbscissa.uninitialize();
	ip_pMatrix.uninitialize();

	m_pStreamEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	m_pStreamEncoder = nullptr;

	op_pBands.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	m_pStreamDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
	m_pStreamDecoder = nullptr;

	return true;
}

bool CBoxAlgorithmFrequencyBandSelector::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmFrequencyBandSelector::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		ip_pMemoryBuffer = boxContext.getInputChunk(0, i);
		op_pMemoryBuffer = boxContext.getOutputChunk(0);
		m_pStreamDecoder->process();
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_vSelectionFactor.clear();
			for (uint32_t j = 0; j < ip_pFrequencyAbscissa->getDimensionSize(0); ++j)
			{
				double frequencyAbscissa = ip_pFrequencyAbscissa->getBuffer()[j];
				const bool selected      = std::any_of(m_vSelected.begin(), m_vSelected.end(), [frequencyAbscissa](const BandRange& currentBandRange)
				{
					return currentBandRange.first <= frequencyAbscissa && frequencyAbscissa <= currentBandRange.second;
				});
				m_vSelectionFactor.push_back(selected ? 1. : 0.);
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			uint32_t offset = 0;
			for (uint32_t j = 0; j < m_oMatrix.getDimensionSize(0); ++j)
			{
				for (uint32_t k = 0; k < m_oMatrix.getDimensionSize(1); ++k)
				{
					m_oMatrix.getBuffer()[offset] = m_vSelectionFactor[k] * m_oMatrix.getBuffer()[offset];
					offset++;
				}
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd)) { m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeEnd); }

		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}

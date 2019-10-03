#include "ovpCBoxAlgorithmChannelSelector.h"

#include <system/ovCMemory.h>

#include <climits>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace OpenViBEToolkit;

namespace
{
	uint32_t FindChannel(const IMatrix& rMatrix, const CString& rChannel, const CIdentifier& rMatchMethodIdentifier, const uint32_t uiStart = 0)
	{
		uint32_t result         = std::numeric_limits<uint32_t>::max();
		const uint32_t nChannel = rMatrix.getDimensionSize(0);

		if (rMatchMethodIdentifier == OVP_TypeId_MatchMethod_Name)
		{
			for (uint32_t i = uiStart; i < rMatrix.getDimensionSize(0); i++)
			{
				if (Tools::String::isAlmostEqual(rMatrix.getDimensionLabel(0, i), rChannel, false)) { result = i; }
			}
		}
		else if (rMatchMethodIdentifier == OVP_TypeId_MatchMethod_Index)
		{
			try
			{
				const int value = std::stoi(rChannel.toASCIIString());

				if (value < 0)
				{
					uint32_t index = uint32_t(- value - 1); // => makes it 0-indexed !
					if (index < nChannel)
					{
						index = nChannel - index - 1; // => reverses index
						if (uiStart <= index) { result = index; }
					}
				}
				if (value > 0)
				{
					const uint32_t index = uint32_t(value - 1); // => makes it 0-indexed !
					if (index < nChannel) { if (uiStart <= index) { result = index; } }
				}
			}
			catch (const std::exception&)
			{
				// catch block intentionnaly left blank
			}
		}
		else if (rMatchMethodIdentifier == OVP_TypeId_MatchMethod_Smart)
		{
			if (result == std::numeric_limits<uint32_t>::max()) { result = FindChannel(rMatrix, rChannel, OVP_TypeId_MatchMethod_Name, uiStart); }
			if (result == std::numeric_limits<uint32_t>::max()) { result = FindChannel(rMatrix, rChannel, OVP_TypeId_MatchMethod_Index, uiStart); }
		}

		return result;
	}
} // namespace

bool CBoxAlgorithmChannelSelector::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	CIdentifier typeID;
	boxContext.getOutputType(0, typeID);

	m_pDecoder = nullptr;
	m_pEncoder = nullptr;

	if (typeID == OV_TypeId_Signal)
	{
		TSignalEncoder<CBoxAlgorithmChannelSelector>* l_pEncoder = new TSignalEncoder<CBoxAlgorithmChannelSelector>;
		TSignalDecoder<CBoxAlgorithmChannelSelector>* l_pDecoder = new TSignalDecoder<CBoxAlgorithmChannelSelector>;
		l_pEncoder->initialize(*this, 0);
		l_pDecoder->initialize(*this, 0);
		l_pEncoder->getInputSamplingRate().setReferenceTarget(l_pDecoder->getOutputSamplingRate());
		m_pDecoder      = l_pDecoder;
		m_pEncoder      = l_pEncoder;
		m_pInputMatrix  = l_pDecoder->getOutputMatrix();
		m_pOutputMatrix = l_pEncoder->getInputMatrix();
	}
	else if (typeID == OV_TypeId_Spectrum)
	{
		TSpectrumEncoder<CBoxAlgorithmChannelSelector>* l_pEncoder = new TSpectrumEncoder<CBoxAlgorithmChannelSelector>;
		TSpectrumDecoder<CBoxAlgorithmChannelSelector>* l_pDecoder = new TSpectrumDecoder<CBoxAlgorithmChannelSelector>;
		l_pEncoder->initialize(*this, 0);
		l_pDecoder->initialize(*this, 0);
		l_pEncoder->getInputFrequencyAbscissa().setReferenceTarget(l_pDecoder->getOutputFrequencyAbscissa());
		l_pEncoder->getInputSamplingRate().setReferenceTarget(l_pDecoder->getOutputSamplingRate());

		m_pDecoder      = l_pDecoder;
		m_pEncoder      = l_pEncoder;
		m_pInputMatrix  = l_pDecoder->getOutputMatrix();
		m_pOutputMatrix = l_pEncoder->getInputMatrix();
	}
	else if (typeID == OV_TypeId_StreamedMatrix)
	{
		TStreamedMatrixEncoder<CBoxAlgorithmChannelSelector>* l_pEncoder = new TStreamedMatrixEncoder<CBoxAlgorithmChannelSelector>;
		TStreamedMatrixDecoder<CBoxAlgorithmChannelSelector>* l_pDecoder = new TStreamedMatrixDecoder<CBoxAlgorithmChannelSelector>;
		l_pEncoder->initialize(*this, 0);
		l_pDecoder->initialize(*this, 0);
		m_pDecoder      = l_pDecoder;
		m_pEncoder      = l_pEncoder;
		m_pInputMatrix  = l_pDecoder->getOutputMatrix();
		m_pOutputMatrix = l_pEncoder->getInputMatrix();
	}
	else { OV_ERROR_KRF("Invalid input type [" << typeID.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput); }

	m_vLookup.clear();
	return true;
}

bool CBoxAlgorithmChannelSelector::uninitialize()
{
	if (m_pDecoder)
	{
		m_pDecoder->uninitialize();
		delete m_pDecoder;
	}
	if (m_pEncoder)
	{
		m_pEncoder->uninitialize();
		delete m_pEncoder;
	}

	return true;
}

bool CBoxAlgorithmChannelSelector::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmChannelSelector::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		m_pDecoder->decode(i);
		if (m_pDecoder->isHeaderReceived())
		{
			CString settingValue             = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			const uint64_t selectionMethodID = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
			const uint64_t matchMethodID     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

			if (selectionMethodID == OVP_TypeId_SelectionMethod_Select_EEG)
			{
				// ______________________________________________________________________________________________________________________________________________________
				//
				// Collects channels with names corresponding to EEG
				// ______________________________________________________________________________________________________________________________________________________
				//

				CString eegChannelNames = this->getConfigurationManager().expand("${Box_ChannelSelector_EEGChannelNames}");

				std::vector<CString> token;
				const uint32_t nToken = split(eegChannelNames, OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(token),
											  OV_Value_EnumeratedStringSeparator);

				for (uint32_t j = 0; j < m_pInputMatrix->getDimensionSize(0); j++)
				{
					for (uint32_t k = 0; k < nToken; k++)
					{
						if (Tools::String::isAlmostEqual(m_pInputMatrix->getDimensionLabel(0, j), token[k], false)) { m_vLookup.push_back(j); }
					}
				}
			}
			else
			{

				// ______________________________________________________________________________________________________________________________________________________
				//
				// Splits the channel list in order to build up the look up table
				// The look up table is later used to fill in the matrix content
				// ______________________________________________________________________________________________________________________________________________________
				//

				std::vector<CString> l_sToken;
				const uint32_t nToken = split(settingValue, OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(l_sToken),
											  OV_Value_EnumeratedStringSeparator);
				for (uint32_t j = 0; j < nToken; j++)
				{
					std::vector<CString> l_sSubToken;

					// Checks if the token is a range
					if (split(l_sToken[j], OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(l_sSubToken),
							  OV_Value_RangeStringSeparator) == 2)
					{
						// Finds the first & second part of the range (only index based)
						uint32_t rangeStartIdx = FindChannel(*m_pInputMatrix, l_sSubToken[0], OVP_TypeId_MatchMethod_Index);
						uint32_t rangeEndIdx   = FindChannel(*m_pInputMatrix, l_sSubToken[1], OVP_TypeId_MatchMethod_Index);

						// When first or second part is not found but associated token is empty, don't consider this as an error
						if (rangeStartIdx == std::numeric_limits<uint32_t>::max() && l_sSubToken[0] == CString("")) { rangeStartIdx = 0; }
						if (rangeEndIdx == std::numeric_limits<uint32_t>::max() && l_sSubToken[1] == CString(""))
						{
							rangeEndIdx = m_pInputMatrix->getDimensionSize(0) - 1;
						}

						// After these corections, if either first or second token were not found, or if start index is greater than start index, consider this an error and invalid range
						OV_ERROR_UNLESS_KRF(
							rangeStartIdx != std::numeric_limits<uint32_t>::max() && rangeEndIdx != std::numeric_limits<uint32_t>::max() && rangeStartIdx <=
							rangeEndIdx,
							"Invalid channel range [" << l_sToken[j] << "] - splitted as [" << l_sSubToken[0] << "][" << l_sSubToken[1] << "]",
							OpenViBE::Kernel::ErrorType::BadSetting);

						// The range is valid so selects all the channels in this range
						this->getLogManager() << LogLevel_Debug << "For range [" << l_sToken[j] << "] :\n";
						for (uint32_t k = rangeStartIdx; k <= rangeEndIdx; k++)
						{
							m_vLookup.push_back(k);
							this->getLogManager() << LogLevel_Debug << "  Selected channel [" << k + 1 << "]\n";
						}
					}
					else
					{
						// This is not a range, so we can consider the whole token as a single token name
						uint32_t found = false;
						uint32_t index = std::numeric_limits<uint32_t>::max();

						// Looks for all the channels with this name
						while ((index = FindChannel(*m_pInputMatrix, l_sToken[j], matchMethodID, index + 1)) != std::numeric_limits<uint32_t>::max())
						{
							found = true;
							m_vLookup.push_back(index);
							this->getLogManager() << LogLevel_Debug << "Selected channel [" << index + 1 << "]\n";
						}

						OV_ERROR_UNLESS_KRF(found, "Invalid channel [" << l_sToken[j] << "]", OpenViBE::Kernel::ErrorType::BadSetting);
					}
				}

				// ______________________________________________________________________________________________________________________________________________________
				//
				// When selection method is set to reject
				// We have to revert the selection building up a new look up table and replacing the old one
				// ______________________________________________________________________________________________________________________________________________________
				//

				if (selectionMethodID == OVP_TypeId_SelectionMethod_Reject)
				{
					std::vector<uint32_t> inversedLookup;
					for (uint32_t j = 0; j < m_pInputMatrix->getDimensionSize(0); j++)
					{
						bool selected = false;
						for (uint32_t k = 0; k < m_vLookup.size(); k++) { selected |= (m_vLookup[k] == j); }
						if (!selected) { inversedLookup.push_back(j); }
					}
					m_vLookup = inversedLookup;
				}
			}

			// ______________________________________________________________________________________________________________________________________________________
			//
			// Now we have the exact topology of the output matrix :)
			// ______________________________________________________________________________________________________________________________________________________
			//

			OV_ERROR_UNLESS_KRF(!m_vLookup.empty(), "No channel selected", OpenViBE::Kernel::ErrorType::BadConfig);

			m_pOutputMatrix->setDimensionCount(2);
			m_pOutputMatrix->setDimensionSize(0, uint32_t(m_vLookup.size()));
			m_pOutputMatrix->setDimensionSize(1, m_pInputMatrix->getDimensionSize(1));
			Tools::Matrix::clearContent(*m_pOutputMatrix);
			for (uint32_t j = 0; j < m_vLookup.size(); j++)
			{
				if (m_vLookup[j] < m_pInputMatrix->getDimensionSize(0))
				{
					m_pOutputMatrix->setDimensionLabel(0, j, m_pInputMatrix->getDimensionLabel(0, m_vLookup[j]));
				}
				else { m_pOutputMatrix->setDimensionLabel(0, j, "Missing channel"); }
			}
			for (uint32_t j = 0; j < m_pInputMatrix->getDimensionSize(1); j++)
			{
				m_pOutputMatrix->setDimensionLabel(1, j, m_pInputMatrix->getDimensionLabel(1, j));
			}

			m_pEncoder->encodeHeader();
		}

		if (m_pDecoder->isBufferReceived())
		{
			// ______________________________________________________________________________________________________________________________________________________
			//
			// When a buffer is received, just copy the channel content depending on the look up table
			// ______________________________________________________________________________________________________________________________________________________
			//

			const uint32_t nSample = m_pOutputMatrix->getDimensionSize(1);
			for (uint32_t j = 0; j < m_vLookup.size(); j++)
			{
				if (m_vLookup[j] < m_pInputMatrix->getDimensionSize(0))
				{
					System::Memory::copy(m_pOutputMatrix->getBuffer() + j * nSample, m_pInputMatrix->getBuffer() + m_vLookup[j] * nSample,
										 nSample * sizeof(double));
				}
			}
			m_pEncoder->encodeBuffer();
		}
		if (m_pDecoder->isEndReceived()) { m_pEncoder->encodeEnd(); }
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}

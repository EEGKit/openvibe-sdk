#include "ovpCBoxAlgorithmChannelSelector.h"

#include <system/ovCMemory.h>

#include <cstdio>
#include <limits>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace OpenViBEToolkit;

namespace
{
	uint32_t _find_channel_(const IMatrix& rMatrix, const CString& rChannel, const CIdentifier& rMatchMethodIdentifier, uint32_t uiStart = 0)
	{
		uint32_t i;
		uint32_t l_ui32Result       = std::numeric_limits<uint32_t>::max();
		uint32_t l_ui32ChannelCount = rMatrix.getDimensionSize(0);

		if (rMatchMethodIdentifier == OVP_TypeId_MatchMethod_Name)
		{
			for (i = uiStart; i < rMatrix.getDimensionSize(0); i++)
			{
				if (Tools::String::isAlmostEqual(rMatrix.getDimensionLabel(0, i), rChannel, false))
				{
					l_ui32Result = i;
				}
			}
		}
		else if (rMatchMethodIdentifier == OVP_TypeId_MatchMethod_Index)
		{
			try
			{
				int value = std::stoi(rChannel.toASCIIString());

				if (value < 0)
				{
					uint32_t l_ui32Index = uint32_t(- value - 1); // => makes it 0-indexed !
					if (l_ui32Index < l_ui32ChannelCount)
					{
						l_ui32Index = l_ui32ChannelCount - l_ui32Index - 1; // => reverses index
						if (uiStart <= l_ui32Index)
						{
							l_ui32Result = l_ui32Index;
						}
					}
				}
				if (value > 0)
				{
					uint32_t l_ui32Index = uint32_t(value - 1); // => makes it 0-indexed !
					if (l_ui32Index < l_ui32ChannelCount)
					{
						if (uiStart <= l_ui32Index)
						{
							l_ui32Result = l_ui32Index;
						}
					}
				}
			}
			catch (const std::exception&)
			{
				// catch block intentionnaly left blank
			}
		}
		else if (rMatchMethodIdentifier == OVP_TypeId_MatchMethod_Smart)
		{
			if (l_ui32Result == std::numeric_limits<uint32_t>::max()) l_ui32Result = _find_channel_(rMatrix, rChannel, OVP_TypeId_MatchMethod_Name, uiStart);
			if (l_ui32Result == std::numeric_limits<uint32_t>::max()) l_ui32Result = _find_channel_(rMatrix, rChannel, OVP_TypeId_MatchMethod_Index, uiStart);
		}

		return l_ui32Result;
	}
};

bool CBoxAlgorithmChannelSelector::initialize()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();

	CIdentifier l_oTypeIdentifier;
	l_rStaticBoxContext.getOutputType(0, l_oTypeIdentifier);

	m_pDecoder = NULL;
	m_pEncoder = NULL;

	if (l_oTypeIdentifier == OV_TypeId_Signal)
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
	else if (l_oTypeIdentifier == OV_TypeId_Spectrum)
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
	else if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
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
	else
	{
		OV_ERROR_KRF("Invalid input type [" << l_oTypeIdentifier.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput);
	}

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

bool CBoxAlgorithmChannelSelector::processInput(uint32_t ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmChannelSelector::process()
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();
	for (uint32_t i = 0; i < l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_pDecoder->decode(i);
		if (m_pDecoder->isHeaderReceived())
		{
			CString l_sSettingValue                = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			uint64_t l_ui64SelectionMethodIdentifier = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
			uint64_t l_ui64MatchMethodIdentifier     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

			if (l_ui64SelectionMethodIdentifier == OVP_TypeId_SelectionMethod_Select_EEG)
			{
				// ______________________________________________________________________________________________________________________________________________________
				//
				// Collects channels with names corresponding to EEG
				// ______________________________________________________________________________________________________________________________________________________
				//

				CString l_sEEGChannelNames = this->getConfigurationManager().expand("${Box_ChannelSelector_EEGChannelNames}");

				std::vector<CString> l_sToken;
				uint32_t l_ui32TokenCount = split(l_sEEGChannelNames, OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(l_sToken), OV_Value_EnumeratedStringSeparator);

				for (uint32_t j = 0; j < m_pInputMatrix->getDimensionSize(0); j++)
				{
					for (uint32_t k = 0; k < l_ui32TokenCount; k++)
					{
						if (Tools::String::isAlmostEqual(m_pInputMatrix->getDimensionLabel(0, j), l_sToken[k], false))
						{
							m_vLookup.push_back(j);
						}
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
				uint32_t l_ui32TokenCount = split(l_sSettingValue, OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(l_sToken), OV_Value_EnumeratedStringSeparator);
				for (uint32_t j = 0; j < l_ui32TokenCount; j++)
				{
					std::vector<CString> l_sSubToken;

					// Checks if the token is a range
					if (split(l_sToken[j], OpenViBEToolkit::Tools::String::TSplitCallback<std::vector<CString>>(l_sSubToken), OV_Value_RangeStringSeparator) == 2)
					{
						// Finds the first & second part of the range (only index based)
						uint32_t l_ui32RangeStartIndex = _find_channel_(*m_pInputMatrix, l_sSubToken[0], OVP_TypeId_MatchMethod_Index);
						uint32_t l_ui32RangeEndIndex   = _find_channel_(*m_pInputMatrix, l_sSubToken[1], OVP_TypeId_MatchMethod_Index);

						// When first or second part is not found but associated token is empty, don't consider this as an error
						if (l_ui32RangeStartIndex == std::numeric_limits<uint32_t>::max() && l_sSubToken[0] == CString("")) l_ui32RangeStartIndex = 0;
						if (l_ui32RangeEndIndex == std::numeric_limits<uint32_t>::max() && l_sSubToken[1] == CString("")) l_ui32RangeEndIndex = m_pInputMatrix->getDimensionSize(0) - 1;

						// After these corections, if either first or second token were not found, or if start index is greater than start index, consider this an error and invalid range
						OV_ERROR_UNLESS_KRF(
							l_ui32RangeStartIndex != std::numeric_limits<uint32_t>::max() && l_ui32RangeEndIndex != std::numeric_limits<uint32_t>::max() && l_ui32RangeStartIndex <= l_ui32RangeEndIndex,
							"Invalid channel range [" << l_sToken[j] << "] - splitted as [" << l_sSubToken[0] << "][" << l_sSubToken[1] << "]",
							OpenViBE::Kernel::ErrorType::BadSetting
						);

						// The range is valid so selects all the channels in this range
						this->getLogManager() << LogLevel_Debug << "For range [" << l_sToken[j] << "] :\n";
						for (uint32_t k = l_ui32RangeStartIndex; k <= l_ui32RangeEndIndex; k++)
						{
							m_vLookup.push_back(k);
							this->getLogManager() << LogLevel_Debug << "  Selected channel [" << k + 1 << "]\n";
						}
					}
					else
					{
						// This is not a range, so we can consider the whole token as a single token name
						uint32_t l_bFound    = false;
						uint32_t l_ui32Index = std::numeric_limits<uint32_t>::max();

						// Looks for all the channels with this name
						while ((l_ui32Index = _find_channel_(*m_pInputMatrix, l_sToken[j], l_ui64MatchMethodIdentifier, l_ui32Index + 1)) != std::numeric_limits<uint32_t>::max())
						{
							l_bFound = true;
							m_vLookup.push_back(l_ui32Index);
							this->getLogManager() << LogLevel_Debug << "Selected channel [" << l_ui32Index + 1 << "]\n";
						}

						OV_ERROR_UNLESS_KRF(
							l_bFound,
							"Invalid channel [" << l_sToken[j] << "]",
							OpenViBE::Kernel::ErrorType::BadSetting
						);
					}
				}

				// ______________________________________________________________________________________________________________________________________________________
				//
				// When selection method is set to reject
				// We have to revert the selection building up a new look up table and replacing the old one
				// ______________________________________________________________________________________________________________________________________________________
				//

				if (l_ui64SelectionMethodIdentifier == OVP_TypeId_SelectionMethod_Reject)
				{
					std::vector<uint32_t> l_vInversedLookup;
					for (uint32_t j = 0; j < m_pInputMatrix->getDimensionSize(0); j++)
					{
						bool l_bSelected = false;
						for (uint32_t k = 0; k < m_vLookup.size(); k++)
						{
							l_bSelected |= (m_vLookup[k] == j);
						}
						if (!l_bSelected)
						{
							l_vInversedLookup.push_back(j);
						}
					}
					m_vLookup = l_vInversedLookup;
				}
			}

			// ______________________________________________________________________________________________________________________________________________________
			//
			// Now we have the exact topology of the output matrix :)
			// ______________________________________________________________________________________________________________________________________________________
			//

			OV_ERROR_UNLESS_KRF(
				!m_vLookup.empty(),
				"No channel selected",
				OpenViBE::Kernel::ErrorType::BadConfig
			);

			m_pOutputMatrix->setDimensionCount(2);
			m_pOutputMatrix->setDimensionSize(0, m_vLookup.size());
			m_pOutputMatrix->setDimensionSize(1, m_pInputMatrix->getDimensionSize(1));
			Tools::Matrix::clearContent(*m_pOutputMatrix);
			for (uint32_t j = 0; j < m_vLookup.size(); j++)
			{
				if (m_vLookup[j] < m_pInputMatrix->getDimensionSize(0))
				{
					m_pOutputMatrix->setDimensionLabel(0, j, m_pInputMatrix->getDimensionLabel(0, m_vLookup[j]));
				}
				else
				{
					m_pOutputMatrix->setDimensionLabel(0, j, "Missing channel");
				}
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

			uint32_t l_ui32SampleCount = m_pOutputMatrix->getDimensionSize(1);
			for (uint32_t j = 0; j < m_vLookup.size(); j++)
			{
				if (m_vLookup[j] < m_pInputMatrix->getDimensionSize(0))
				{
					System::Memory::copy(
						m_pOutputMatrix->getBuffer() + j * l_ui32SampleCount,
						m_pInputMatrix->getBuffer() + m_vLookup[j] * l_ui32SampleCount,
						l_ui32SampleCount * sizeof(double));
				}
			}
			m_pEncoder->encodeBuffer();
		}
		if (m_pDecoder->isEndReceived())
		{
			m_pEncoder->encodeEnd();
		}
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}

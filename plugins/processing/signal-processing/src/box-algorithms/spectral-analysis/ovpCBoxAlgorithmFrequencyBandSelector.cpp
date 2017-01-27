#include "ovpCBoxAlgorithmFrequencyBandSelector.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

#include <vector>
#include <string>
#include <cstdio>

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)<(b)?(b):(a))

namespace
{
	std::vector < std::string > split(const std::string& sString, const char c)
	{
		std::vector < std::string > l_vResult;
		std::string::size_type i=0;
		std::string::size_type j=0;
		while(i<sString.length())
		{
			j=i;
			while(j<sString.length() && sString[j]!=c)
			{
				j++;
			}
			if(i!=j)
			{
				l_vResult.push_back(std::string(sString, i, j-i));
			}
			i=j+1;
		}
		return l_vResult;
	}
};

boolean CBoxAlgorithmFrequencyBandSelector::initialize(void)
{
	CString l_sSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	std::vector < std::string > l_vSetting=::split(l_sSettingValue.toASCIIString(), OV_Value_EnumeratedStringSeparator);
	std::vector < std::string > l_vSettingRange;
	std::vector < std::string >::const_iterator it;
	boolean l_bHadError = false;
	CString l_sErrorMessage;
	m_vSelected.clear();
	for(it=l_vSetting.begin(); it!=l_vSetting.end(); ++it)
	{
		boolean l_bGood=true;
		l_vSettingRange=::split(*it, OV_Value_RangeStringSeparator);
		if(l_vSettingRange.size() == 1)
		{
			try
			{
				double l_dValue = std::stod(l_vSettingRange[0].c_str());
				m_vSelected.push_back(std::pair < float64, float64 >(l_dValue, l_dValue));
			}
			catch(const std::exception&)
			{
				l_bGood=false;
			}
		}
		else if(l_vSettingRange.size() == 2)
		{
			try
			{
				double l_dLowValue = std::stod(l_vSettingRange[0].c_str());
				double l_dHighValue = std::stod(l_vSettingRange[1].c_str());
				m_vSelected.push_back(std::pair < float64, float64 >(min(l_dLowValue, l_dHighValue), max(l_dLowValue, l_dHighValue)));
			}
			catch(const std::exception&)
			{
				l_bGood=false;
			}
		}

		if(!l_bGood)
		{
			l_sErrorMessage = CString("Invalid frequency band [") + it->c_str() + "]";
			l_bHadError=true;
		}
	}

	m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
	op_pBands.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix));
	ip_pBands.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	ip_pBands.setReferenceTarget(op_pBands);
	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate));

	ip_pMatrix=&m_oMatrix;
	op_pMatrix=&m_oMatrix;

	OV_ERROR_UNLESS_KRF(
		!l_bHadError || !m_vSelected.empty(),
		l_sErrorMessage,
		OpenViBE::Kernel::ErrorType::BadSetting
	);

	return true;
}

boolean CBoxAlgorithmFrequencyBandSelector::uninitialize(void)
{
	op_pMemoryBuffer.uninitialize();
	ip_pBands.uninitialize();
	ip_pMatrix.uninitialize();

	m_pStreamEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	m_pStreamEncoder=NULL;

	op_pBands.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	m_pStreamDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
	m_pStreamDecoder=NULL;

	return true;
}

boolean CBoxAlgorithmFrequencyBandSelector::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmFrequencyBandSelector::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		m_pStreamDecoder->process();
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_vSelectionFactor.clear();
			for(uint32 j=0; j<ip_pBands->getDimensionSize(0); j++)
			{
				float64 f64Center=ip_pBands->getBuffer()[j];
				bool bSelected = std::any_of(m_vSelected.begin(), m_vSelected.end(), [f64Center](const BandRange& currentBandRange)
					{
						return currentBandRange.first <= f64Center
								&& f64Center <= currentBandRange.second;
					});
				m_vSelectionFactor.push_back(bSelected ? 1. : 0.);
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			uint32 l_ui32Offset=0;
			for(uint32 j=0; j<m_oMatrix.getDimensionSize(0); j++)
			{
				for(uint32 k=0; k<m_oMatrix.getDimensionSize(1); k++)
				{
					m_oMatrix.getBuffer()[l_ui32Offset]=m_vSelectionFactor[k]*m_oMatrix.getBuffer()[l_ui32Offset];
					l_ui32Offset++;
				}
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if(m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeEnd);
		}

		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	return true;
}

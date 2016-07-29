#include "ovpCBoxAlgorithmTemporalFilter.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

namespace
{
	#define FilterType_BandPass 0x19E89DDBD1FF7FF5
	#define FilterType_BandStop 0x41C34DD7F70E4043
	#define FilterType_HighPass 0x2F6771959F9F15B9
	#define FilterType_LowPass  0x0C33A568E2BD46D2

	typedef Dsp::SmoothedFilterDesign < Dsp::Butterworth::Design::BandPass < 32 >, 1, Dsp::DirectFormII > CButterworthBandPass;
	typedef Dsp::SmoothedFilterDesign < Dsp::Butterworth::Design::BandStop < 32 >, 1, Dsp::DirectFormII > CButterworthBandStop;
	typedef Dsp::SmoothedFilterDesign < Dsp::Butterworth::Design::HighPass < 32 >, 1, Dsp::DirectFormII > CButterworthHighPass;
	typedef Dsp::SmoothedFilterDesign < Dsp::Butterworth::Design::LowPass < 32 >, 1, Dsp::DirectFormII > CButterworthLowPass;

	Dsp::Filter* createButterworthFilter(uint64 ui64FilterType, uint64 ui64SmoothingSampleCount)
	{
		switch(ui64FilterType)
		{
			case FilterType_BandPass: return new CButterworthBandPass(static_cast<int>(ui64SmoothingSampleCount));
			case FilterType_BandStop: return new CButterworthBandStop(static_cast<int>(ui64SmoothingSampleCount));
			case FilterType_HighPass: return new CButterworthHighPass(static_cast<int>(ui64SmoothingSampleCount));
			case FilterType_LowPass: return new CButterworthLowPass(static_cast<int>(ui64SmoothingSampleCount));
			default:
				break;
		}
		return NULL;
	}

	boolean getButterworthParameters(Dsp::Params& rParameters, uint64 ui64SamplingRate, uint64 ui64FilterType, uint64 ui64Order, float64 f64LowCutFrequency, float64 f64HighCutFrequency, float64 f64BandPassRipple)
	{
		rParameters[0]=static_cast<double>(ui64SamplingRate);
		rParameters[1]=static_cast<double>(ui64Order);
		switch(ui64FilterType)
		{
			case FilterType_BandPass:
			case FilterType_BandStop:
				rParameters[2]=.5*(f64HighCutFrequency+f64LowCutFrequency);
				rParameters[3]=1.*(f64HighCutFrequency-f64LowCutFrequency);
				break;
			case FilterType_HighPass:
				rParameters[2]=f64LowCutFrequency;
				break;
			case FilterType_LowPass:
				rParameters[2]=f64HighCutFrequency;
				break;
			default:
				return false;
		}
		return true;
	}

	typedef Dsp::SmoothedFilterDesign < Dsp::ChebyshevI::Design::BandPass < 4 >, 1, Dsp::DirectFormII > CChebyshevBandPass;
	typedef Dsp::SmoothedFilterDesign < Dsp::ChebyshevI::Design::BandStop < 4 >, 1, Dsp::DirectFormII > CChebyshevBandStop;
	typedef Dsp::SmoothedFilterDesign < Dsp::ChebyshevI::Design::HighPass < 4 >, 1, Dsp::DirectFormII > CChebyshevHighPass;
	typedef Dsp::SmoothedFilterDesign < Dsp::ChebyshevI::Design::LowPass < 4 >, 1, Dsp::DirectFormII > CChebyshevLowPass;

	/*
	Dsp::Filter* createChebishevFilter(uint64 ui64FilterType, uint64 ui64SmoothingSampleCount)
	{
		switch(ui64FilterType)
		{
			case FilterType_BandPass: return new CChebyshevBandPass(static_cast<int>(ui64SmoothingSampleCount));
			case FilterType_BandStop: return new CChebyshevBandStop(static_cast<int>(ui64SmoothingSampleCount));
			case FilterType_HighPass: return new CChebyshevHighPass(static_cast<int>(ui64SmoothingSampleCount));
			case FilterType_LowPass: return new CChebyshevLowPass(static_cast<int>(ui64SmoothingSampleCount));
			default:
				break;
		}
		return NULL;
	}

	boolean getChebishevParameters(Dsp::Params& rParameters, uint64 ui64FilterType, uint64 ui64SamplingRate, uint64 ui64Order, float64 f64LowCutFrequency, float64 f64HighCutFrequency, float64 f64BandPassRipple)
	{
		rParameters[0]=static_cast<int>(ui64SamplingRate);
		rParameters[1]=static_cast<int>(ui64Order);
		switch(ui64FilterType)
		{
			case FilterType_BandPass:
			case FilterType_BandStop:
				rParameters[2]=.5*(f64HighCutFrequency+f64LowCutFrequency);
				rParameters[3]=1.*(f64HighCutFrequency-f64LowCutFrequency);
				rParameters[4]=f64BandPassRipple;
				break;
			case FilterType_HighPass:
				rParameters[2]=f64HighCutFrequency; // TO CHECK : f64LowCutFrequency ?
				rParameters[3]=f64BandPassRipple;
				break;
			case FilterType_LowPass:
				rParameters[2]=f64HighCutFrequency;
				rParameters[3]=f64LowCutFrequency;  // TO CHECK : f64BandPassRipple ?
				break;
			default:
				return false;
		}
		return true;
	}
	*/

	typedef boolean (*fpGetParameters_t)(Dsp::Params& rParameters, uint64 ui64FilterType, uint64 ui64SamplingRate, uint64 ui64Order, float64 f64LowCutFrequency, float64 f64HighCutFrequency, float64 f64BandPassRipple);
	typedef Dsp::Filter* (*fpCreateFilter_t)(uint64 ui64FilterType, uint64 ui64SmoothingSampleCount);
}

boolean CBoxAlgorithmTemporalFilter::initialize(void)
{
	m_ui64FilterMethod=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64FilterType=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	int64 l_i64FilterOrder=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_f64LowCutFrequency=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_f64HighCutFrequency=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	if(l_i64FilterOrder < 1)
	{
		this->getLogManager() << LogLevel_Error << "Filter order can not be inferior to 1.\n";
		return false;
	}
	m_ui64FilterOrder = static_cast<uint64>(l_i64FilterOrder);

	if(m_ui64FilterType == OVP_TypeId_FilterType_LowPass)
	{
		if(m_f64HighCutFrequency <= 0)
		{
			this->getLogManager() << LogLevel_Error << "High cut-off frequency can not be negative.\n";
			return false;
		}
	}
	else if(m_ui64FilterType == OVP_TypeId_FilterType_HighPass)
	{
		if(m_f64LowCutFrequency <= 0)
		{
			this->getLogManager() << LogLevel_Error << "Low cut-off frequency can not be negative.\n";
			return false;
		}
	}
	else if(m_ui64FilterType == OVP_TypeId_FilterType_BandPass || m_ui64FilterType == FilterType_BandStop)
	{
		if(m_f64LowCutFrequency < 0)
		{
			this->getLogManager() << LogLevel_Error << "Low cut-off frequency can not be negative.\n";
			return false;
		}
		if(m_f64HighCutFrequency <= 0)
		{
			this->getLogManager() << LogLevel_Error << "High cut-off frequency can not be negative.\n";
			return false;
		}
		if(m_f64HighCutFrequency <= m_f64LowCutFrequency)
		{
			this->getLogManager() << LogLevel_Warning << "High cut-off frequency can not be inferior or equal to low cut-off frequency.\n";
			return false;
		}
	}
	else
	{
		this->getLogManager() << LogLevel_Warning << "Unknown filter type [" << m_ui64FilterType << "].\n";
		return false;
	}

	m_oDecoder.initialize(*this, 0);
	m_oEncoder.initialize(*this, 0);

	m_oEncoder.getInputSamplingRate().setReferenceTarget(m_oDecoder.getOutputSamplingRate());
	m_oEncoder.getInputMatrix().setReferenceTarget(m_oDecoder.getOutputMatrix());

	return true;
}

boolean CBoxAlgorithmTemporalFilter::uninitialize(void)
{
	std::vector < Dsp::Filter* >::iterator it;

	for(it=m_vFilter.begin(); it!=m_vFilter.end(); ++it)
	{
		delete *it;
	}
	m_vFilter.clear();

	/*std::vector < Dsp::Filter* >::iterator it2;

	for(it2=m_vFilter2.begin(); it2!=m_vFilter2.end(); it2++)
	{
		delete *it2;
	}
	m_vFilter2.clear();*/

	m_oDecoder.uninitialize();
	m_oEncoder.uninitialize();

	return true;
}

boolean CBoxAlgorithmTemporalFilter::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmTemporalFilter::process(void)
{
	std::vector < Dsp::Filter* >::iterator it;
	//std::vector < Dsp::Filter* >::iterator it2;

	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j;

	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	// l_rStaticBoxContext.getSettingCount();

	// l_rDynamicBoxContext.getInputChunkCount()
	// l_rDynamicBoxContext.getInputChunk(i, )
	// l_rDynamicBoxContext.getOutputChunk(i, )

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_oDecoder.decode(i);

		uint32 l_ui32ChannelCount=m_oDecoder.getOutputMatrix()->getDimensionSize(0);
		uint32 l_ui32SampleCount=m_oDecoder.getOutputMatrix()->getDimensionSize(1);

		if(m_oDecoder.isHeaderReceived())
		{
			if(m_ui64FilterType != OVP_TypeId_FilterType_LowPass) // verification for high-pass, band-pass and band-stop filters
			{
				if(m_f64LowCutFrequency > m_oDecoder.getOutputSamplingRate()*.5)
				{
					this->getLogManager() << LogLevel_Error << "Low cut-off frequency (" << m_f64LowCutFrequency << " Hz) is above Nyquist criteria (sampling rate is " << m_oDecoder.getOutputSamplingRate() << " Hz), can not proceed !\n";
					return false;
				}
			}
			if(m_ui64FilterType != OVP_TypeId_FilterType_HighPass) // verification for low-pass, band-pass and band-stop filters
			{
				if(m_f64HighCutFrequency > m_oDecoder.getOutputSamplingRate()*.5)
				{
					this->getLogManager() << LogLevel_Error << "High cut-off frequency (" << m_f64HighCutFrequency << " Hz) is above Nyquist criteria (sampling rate is " << m_oDecoder.getOutputSamplingRate() << " Hz), can not proceed !\n";
					return false;
				}
			}

			for(it=m_vFilter.begin(); it!=m_vFilter.end(); ++it)
			{
				delete *it;
			}
			m_vFilter.clear();

			/*for(it2=m_vFilter2.begin(); it2!=m_vFilter2.end(); it2++)
			{
				delete *it2;
			}
			m_vFilter2.clear();*/

			fpGetParameters_t fpGetParameters=NULL;
			fpCreateFilter_t fpCreateFilter=NULL;
			if(m_ui64FilterMethod == OVP_TypeId_FilterMethod_Butterworth.toUInteger()) // Butterworth
			{
				fpGetParameters = getButterworthParameters;
				fpCreateFilter = createButterworthFilter;
			}
			else if(m_ui64FilterMethod == OVP_TypeId_FilterMethod_Chebyshev.toUInteger()) // Chebyshev
			{
#if 1
				this->getLogManager() << LogLevel_Error << "This method is not yet implemented\n";
				return false;
#else
				fpGetParameters = getChebishevParameters;
				fpCreateFilter = createChebishevFilter;
#endif
			}
			else if(m_ui64FilterMethod == OVP_TypeId_FilterMethod_YuleWalker.toUInteger()) // YuleWalker
			{
#if 1
				this->getLogManager() << LogLevel_Error << "This method is not yet implemented\n";
				return false;
#else
				fpGetParameters = getYuleWalkerParameters;
				fpCreateFilter = createYuleWalkerFilter;
#endif
			}
			else
			{
				this->getLogManager() << LogLevel_Error << "Unidentified filter method [" << m_ui64FilterMethod << "]\n";
				return false;
			}

			if (m_ui64FilterType == OVP_TypeId_FilterType_HighPass.toUInteger())
			{
				this->getLogManager() << LogLevel_Trace << "Low cut frequency of the High pass filter : " << m_f64LowCutFrequency << "Hz\n";
			}
			if (m_ui64FilterType == OVP_TypeId_FilterType_LowPass.toUInteger())
			{
				this->getLogManager() << LogLevel_Trace << "High cut frequency of the Low pass filter : " << m_f64HighCutFrequency << "Hz\n";
			}

			uint64 l_ui64SamplingRate=m_oDecoder.getOutputSamplingRate();
			uint64 l_ui64SmoothingSampleCount=100 * l_ui64SamplingRate;
			Dsp::Params l_oFilterParameters;
			(*fpGetParameters)(l_oFilterParameters, l_ui64SamplingRate, m_ui64FilterType, m_ui64FilterOrder, m_f64LowCutFrequency, m_f64HighCutFrequency, m_f64BandPassRipple);

			for(j=0; j<l_ui32ChannelCount; j++)
			{
				Dsp::Filter* l_pFilter=(*fpCreateFilter)(m_ui64FilterType, l_ui64SmoothingSampleCount);
				l_pFilter->setParams(l_oFilterParameters);
				m_vFilter.push_back(l_pFilter);
				/*Dsp::Filter* l_pFilter2=(*fpCreateFilter)(m_ui64FilterType, l_ui64SmoothingSampleCount);
				l_pFilter2->setParams(l_oFilterParameters);
				m_vFilter2.push_back(l_pFilter2);*/
			}

			m_oEncoder.encodeHeader();
		}
		if(m_oDecoder.isBufferReceived())
		{
			float64* l_pBuffer=m_oDecoder.getOutputMatrix()->getBuffer();

			//"french cook" to reduce transient for bandpass and highpass filters
			if(m_vFirstSample.size() == 0)
			{
				m_vFirstSample.resize(l_ui32ChannelCount,0); //initialization to 0
				if(m_ui64FilterType == OVP_TypeId_FilterType_BandPass || m_ui64FilterType == OVP_TypeId_FilterType_HighPass)
				{
					for(j=0; j<l_ui32ChannelCount; j++)
					{
						//m_vFirstSample[j] = l_pBuffer[j*l_ui32SampleCount+0]; //first value of the signal = DC offset
						for(uint32 k=0; k<l_ui32SampleCount; k++)
						{
							m_vFirstSample[j] += l_pBuffer[j*l_ui32SampleCount+k];
						}
						m_vFirstSample[j] /= l_ui32SampleCount; //average of first values of the signal = DC offset
					}
				}
			}

			for(j=0; j<l_ui32ChannelCount; j++)
			{
				//for bandpass and highpass filters, suppression of the value m_vFirstSample = DC offset
				//otherwise, no treatment, since m_vFirstSample = 0
				for(uint32 k=0; k<l_ui32SampleCount; k++)
				{
					l_pBuffer[k] -= m_vFirstSample[j];
				}

				if(m_vFilter[j])
				{
					m_vFilter[j]->process(l_ui32SampleCount, &l_pBuffer);
				}
				l_pBuffer+=l_ui32SampleCount;
			}
			m_oEncoder.encodeBuffer();
		}
		if(m_oDecoder.isEndReceived())
		{
			m_oEncoder.encodeEnd();
		}
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}

#if 0
//zero-phase filtering, with two different filters
void CBoxAlgorithmTemporalFilter::filtfilt2 (Dsp::Filter* pFilter1, Dsp::Filter* pFilter2, OpenViBE::uint32 SampleCount, OpenViBE::float64* pBuffer)
{
	uint32 j;

	//1rst filtering
	pFilter1->process(SampleCount, &pBuffer);

	//reversal of the buffer
	for(j=0; j<SampleCount/2; j++)
	{
		float64 l_f64TemporalVar = pBuffer[j];
		pBuffer[j] = pBuffer[SampleCount-1-j];
		pBuffer[SampleCount-1-j] = l_f64TemporalVar;
	}

	//2nd filtering
	pFilter2->process(SampleCount, &pBuffer);

	//reversal of the buffer
	for(j=0; j<SampleCount/2; j++)
	{
		float64 l_f64TemporalVar = pBuffer[j];
		pBuffer[j] = pBuffer[SampleCount-1-j];
		pBuffer[SampleCount-1-j] = l_f64TemporalVar;
	}
}

//zero-phase filtering, with mirror signals on the edges
void CBoxAlgorithmTemporalFilter::filtfilt2mirror (Dsp::Filter* pFilter1, Dsp::Filter* pFilter2, OpenViBE::uint32 SampleCount, OpenViBE::float64* pBuffer)
{
	uint32 j;

	//construction of mirror signals
	uint32 l_ui32TransientLength = std::min( 3*(pFilter1->getPoleZeros().size()-1) , SampleCount/2 );
	std::vector<float64> l_vBuffer;
	l_vBuffer.resize(SampleCount+2*l_ui32TransientLength);

	for(j=0; j<l_ui32TransientLength; j++)
	{
		l_vBuffer[j] = 2*pBuffer[0]-pBuffer[l_ui32TransientLength-j];
	}
	for(j=0; j<SampleCount; j++)
	{
		l_vBuffer[j+l_ui32TransientLength] = pBuffer[j];
	}
	for(j=0; j<l_ui32TransientLength; j++)
	{
		l_vBuffer[j+l_ui32TransientLength+SampleCount] = 2*pBuffer[SampleCount-1]-pBuffer[SampleCount-1-j-1];
	}
	SampleCount+=2*l_ui32TransientLength;

	float64* pBufferTemp;
	pBufferTemp = &l_vBuffer[0];

	//zero-phase filtering
	filtfilt2 (pFilter1, pFilter2, SampleCount, pBufferTemp);

	//central part of the buffer
	for(j=0; j<SampleCount-2*l_ui32TransientLength; j++)
	{
		pBuffer[j] = pBufferTemp[j+l_ui32TransientLength];
	}
}
#endif

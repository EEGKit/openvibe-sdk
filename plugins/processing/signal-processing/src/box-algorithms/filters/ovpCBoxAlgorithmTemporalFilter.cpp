#include "ovpCBoxAlgorithmTemporalFilter.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

namespace
{
#define FilterType_BandPass 0x19E89DDBD1FF7FF5
#define FilterType_BandStop 0x41C34DD7F70E4043
#define FilterType_HighPass 0x2F6771959F9F15B9
#define FilterType_LowPass  0x0C33A568E2BD46D2

	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass<32>, 1, Dsp::DirectFormII> CButterworthBandPass;
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandStop<32>, 1, Dsp::DirectFormII> CButterworthBandStop;
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::HighPass<32>, 1, Dsp::DirectFormII> CButterworthHighPass;
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::LowPass<32>, 1, Dsp::DirectFormII> CButterworthLowPass;

	std::shared_ptr<Dsp::Filter> createButterworthFilter(const uint64_t filterType, const uint64_t nSmoothingSample)
	{
		switch (filterType)
		{
			case FilterType_BandPass: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthBandPass>(int(nSmoothingSample)));
			case FilterType_BandStop: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthBandStop>(int(nSmoothingSample)));
			case FilterType_HighPass: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthHighPass>(int(nSmoothingSample)));
			case FilterType_LowPass: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthLowPass>(int(nSmoothingSample)));
			default:
				break;
		}
		return nullptr;
	}

	bool getButterworthParameters(Dsp::Params& parameters, const uint64_t samplingRate, const uint64_t filterType, const uint64_t order, const double lowCutFrequency, const double highCutFrequency, const double /*bandPassRipple*/)
	{
		parameters[0] = double(samplingRate);
		parameters[1] = double(order);
		switch (filterType)
		{
			case FilterType_BandPass:
			case FilterType_BandStop:
				parameters[2] = .5 * (highCutFrequency + lowCutFrequency);
				parameters[3] = 1. * (highCutFrequency - lowCutFrequency);
				break;
			case FilterType_HighPass:
				parameters[2] = lowCutFrequency;
				break;
			case FilterType_LowPass:
				parameters[2] = highCutFrequency;
				break;
			default:
				return false;
		}
		return true;
	}

	/*
	typedef Dsp::SmoothedFilterDesign<Dsp::ChebyshevI::Design::BandPass<4>, 1, Dsp::DirectFormII> CChebyshevBandPass;
	typedef Dsp::SmoothedFilterDesign<Dsp::ChebyshevI::Design::BandStop<4>, 1, Dsp::DirectFormII> CChebyshevBandStop;
	typedef Dsp::SmoothedFilterDesign<Dsp::ChebyshevI::Design::HighPass<4>, 1, Dsp::DirectFormII> CChebyshevHighPass;
	typedef Dsp::SmoothedFilterDesign<Dsp::ChebyshevI::Design::LowPass<4>, 1, Dsp::DirectFormII> CChebyshevLowPass;
		std::shared_ptr < Dsp::Filter > createChebishevFilter(uint64_t ui64FilterType, uint64_t ui64SmoothingSampleCount)
		{
			switch(ui64FilterType)
			{
				case FilterType_BandPass: return std::shared_ptr < Dsp::Filter >(new CChebyshevBandPass(int(ui64SmoothingSampleCount)));
				case FilterType_BandStop: return std::shared_ptr < Dsp::Filter >(new CChebyshevBandStop(int(ui64SmoothingSampleCount)));
				case FilterType_HighPass: return std::shared_ptr < Dsp::Filter >(new CChebyshevHighPass(int(ui64SmoothingSampleCount)));
				case FilterType_LowPass: return std::shared_ptr < Dsp::Filter >(new CChebyshevLowPass(int(ui64SmoothingSampleCount)));
				default:
					break;
			}
			return NULL;
		}
	
		bool getChebishevParameters(Dsp::Params& rParameters, uint64_t ui64FilterType, uint64_t ui64SamplingRate, uint64_t ui64Order, double f64LowCutFrequency, double f64HighCutFrequency, double f64BandPassRipple)
		{
			rParameters[0]=int(ui64SamplingRate);
			rParameters[1]=int(ui64Order);
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

	typedef bool (*fpGetParameters_t)(Dsp::Params& rParameters, uint64_t ui64FilterType, uint64_t ui64SamplingRate, uint64_t ui64Order,
									  double f64LowCutFrequency, double f64HighCutFrequency, double f64BandPassRipple);
	typedef std::shared_ptr<Dsp::Filter> (*fpCreateFilter_t)(uint64_t ui64FilterType, uint64_t ui64SmoothingSampleCount);
}  // namespace

bool CBoxAlgorithmTemporalFilter::initialize()
{
	m_filterMethod         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_filterType           = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	const int64_t fildterOrder = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_lowCutFrequency       = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_highCutFrequency      = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	OV_ERROR_UNLESS_KRF(fildterOrder >= 1, "Invalid filter order [" << fildterOrder << "] (expected value >= 1)", OpenViBE::Kernel::ErrorType::BadSetting);

	m_filterOrder = uint64_t(fildterOrder);

	if (m_filterType == OVP_TypeId_FilterType_LowPass)
	{
		OV_ERROR_UNLESS_KRF(m_highCutFrequency > 0, "Invalid high cut-off frequency [" << m_highCutFrequency << "] (expected value > 0)",
							OpenViBE::Kernel::ErrorType::BadSetting);
	}
	else if (m_filterType == OVP_TypeId_FilterType_HighPass)
	{
		OV_ERROR_UNLESS_KRF(m_lowCutFrequency > 0, "Invalid low cut-off frequency [" << m_lowCutFrequency << "] (expected value > 0)",
							OpenViBE::Kernel::ErrorType::BadSetting);
	}
	else if (m_filterType == OVP_TypeId_FilterType_BandPass || m_filterType == FilterType_BandStop)
	{
		OV_ERROR_UNLESS_KRF(m_lowCutFrequency >= 0, "Invalid low cut-off frequency [" << m_lowCutFrequency << "] (expected value >= 0)",
							OpenViBE::Kernel::ErrorType::BadSetting);

		OV_ERROR_UNLESS_KRF(m_highCutFrequency > 0, "Invalid high cut-off frequency [" << m_highCutFrequency << "] (expected value > 0)",
							OpenViBE::Kernel::ErrorType::BadSetting);

		OV_ERROR_UNLESS_KRF(m_highCutFrequency > m_lowCutFrequency,
							"Invalid cut-off frequencies [" << m_lowCutFrequency << "," << m_highCutFrequency <<
							"] (expected low frequency < high frequency)", OpenViBE::Kernel::ErrorType::BadSetting);
	}
	else { OV_ERROR_KRF("Invalid filter type [" << m_filterType << "]", OpenViBE::Kernel::ErrorType::BadSetting); }

	m_oDecoder.initialize(*this, 0);
	m_oEncoder.initialize(*this, 0);

	m_oEncoder.getInputSamplingRate().setReferenceTarget(m_oDecoder.getOutputSamplingRate());
	m_oEncoder.getInputMatrix().setReferenceTarget(m_oDecoder.getOutputMatrix());

	return true;
}

bool CBoxAlgorithmTemporalFilter::uninitialize()
{
	m_oDecoder.uninitialize();
	m_oEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmTemporalFilter::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTemporalFilter::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();
	uint32_t j;

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		m_oDecoder.decode(i);

		const uint32_t nChannel = m_oDecoder.getOutputMatrix()->getDimensionSize(0);
		const uint32_t nSample  = m_oDecoder.getOutputMatrix()->getDimensionSize(1);

		if (m_oDecoder.isHeaderReceived())
		{
			if (m_filterType != OVP_TypeId_FilterType_LowPass) // verification for high-pass, band-pass and band-stop filters
			{
				OV_ERROR_UNLESS_KRF(m_lowCutFrequency <= m_oDecoder.getOutputSamplingRate()*.5,
									"Invalid low cut-off frequency [" << m_lowCutFrequency <<
									"] (expected value must meet nyquist criteria for sampling rate " << m_oDecoder.getOutputSamplingRate() << ")",
									OpenViBE::Kernel::ErrorType::BadConfig);
			}
			if (m_filterType != OVP_TypeId_FilterType_HighPass) // verification for low-pass, band-pass and band-stop filters
			{
				OV_ERROR_UNLESS_KRF(m_highCutFrequency <= m_oDecoder.getOutputSamplingRate()*.5,
									"Invalid high cut-off frequency [" << m_highCutFrequency <<
									"] (expected value must meet nyquist criteria for sampling rate " << m_oDecoder.getOutputSamplingRate() << ")",
									OpenViBE::Kernel::ErrorType::BadConfig);
			}

			m_filters.clear();
			//m_vFilter2.clear();

			fpGetParameters_t fpGetParameters = NULL;
			fpCreateFilter_t fpCreateFilter   = NULL;
			if (m_filterMethod == OVP_TypeId_FilterMethod_Butterworth.toUInteger()) // Butterworth
			{
				fpGetParameters = getButterworthParameters;
				fpCreateFilter  = createButterworthFilter;
			}
			else if (m_filterMethod == OVP_TypeId_FilterMethod_Chebyshev.toUInteger()) // Chebyshev
			{
#if 1
				OV_ERROR_KRF("Chebyshev method not implemented", OpenViBE::Kernel::ErrorType::NotImplemented);
#else
				fpGetParameters = getChebishevParameters;
				fpCreateFilter = createChebishevFilter;
#endif
			}
			else if (m_filterMethod == OVP_TypeId_FilterMethod_YuleWalker.toUInteger()) // YuleWalker
			{
#if 1
				OV_ERROR_KRF("YuleWalker method not implemented", OpenViBE::Kernel::ErrorType::NotImplemented);
				return false;
#else
				fpGetParameters = getYuleWalkerParameters;
				fpCreateFilter = createYuleWalkerFilter;
#endif
			}
			else { OV_ERROR_KRF("Invalid filter method [" << m_filterMethod << "]", OpenViBE::Kernel::ErrorType::BadSetting); }

			if (m_filterType == OVP_TypeId_FilterType_HighPass.toUInteger())
			{
				this->getLogManager() << LogLevel_Debug << "Low cut frequency of the High pass filter : " << m_lowCutFrequency << "Hz\n";
			}
			if (m_filterType == OVP_TypeId_FilterType_LowPass.toUInteger())
			{
				this->getLogManager() << LogLevel_Debug << "High cut frequency of the Low pass filter : " << m_highCutFrequency << "Hz\n";
			}

			const uint64_t samplingRate         = m_oDecoder.getOutputSamplingRate();
			const uint64_t nSmoothingSample = 100 * samplingRate;
			Dsp::Params params;
			(*fpGetParameters)(params, samplingRate, m_filterType, m_filterOrder, m_lowCutFrequency, m_highCutFrequency, m_bandPassRipple);

			for (j = 0; j < nChannel; ++j)
			{
				std::shared_ptr<Dsp::Filter> l_pFilter = (*fpCreateFilter)(m_filterType, nSmoothingSample);
				l_pFilter->setParams(params);
				m_filters.push_back(l_pFilter);
				/*std::shared_ptr < Dsp::Filter > l_pFilter2=(*fpCreateFilter)(m_filterType, l_ui64SmoothingSampleCount);
				l_pFilter2->setParams(l_oFilterParameters);
				m_vFilter2.push_back(l_pFilter2);*/
			}

			m_oEncoder.encodeHeader();
		}
		if (m_oDecoder.isBufferReceived())
		{
			double* buffer = m_oDecoder.getOutputMatrix()->getBuffer();

			//"french cook" to reduce transient for bandpass and highpass filters
			if (m_firstSamples.empty())
			{
				m_firstSamples.resize(nChannel, 0); //initialization to 0
				if (m_filterType == OVP_TypeId_FilterType_BandPass || m_filterType == OVP_TypeId_FilterType_HighPass)
				{
					for (j = 0; j < nChannel; ++j)
					{
						m_firstSamples[j] = buffer[j * nSample]; //first value of the signal = DC offset
					}
				}
			}

			for (j = 0; j < nChannel; ++j)
			{
				//for bandpass and highpass filters, suppression of the value m_firstSamples = DC offset
				//otherwise, no treatment, since m_firstSamples = 0
				for (uint32_t k = 0; k < nSample; ++k) { buffer[k] -= m_firstSamples[j]; }

				if (m_filters[j]) { m_filters[j]->process(nSample, &buffer); }
				buffer += nSample;
			}
			m_oEncoder.encodeBuffer();
		}
		if (m_oDecoder.isEndReceived()) { m_oEncoder.encodeEnd(); }
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}

#if 0
//zero-phase filtering, with two different filters
void CBoxAlgorithmTemporalFilter::filtfilt2(std::shared_ptr < Dsp::Filter > pFilter1, std::shared_ptr < Dsp::Filter > pFilter2, uint32_t SampleCount, double* buffer)
{
	uint32_t j;

	//1rst filtering
	pFilter1->process(SampleCount, &buffer);

	//reversal of the buffer
	for (j=0; j<SampleCount/2; ++j)
	{
		double l_f64TemporalVar = buffer[j];
		buffer[j] = buffer[SampleCount-1-j];
		buffer[SampleCount-1-j] = l_f64TemporalVar;
	}

	//2nd filtering
	pFilter2->process(SampleCount, &buffer);

	//reversal of the buffer
	for (j=0; j<SampleCount/2; ++j)
	{
		double l_f64TemporalVar = buffer[j];
		buffer[j] = buffer[SampleCount-1-j];
		buffer[SampleCount-1-j] = l_f64TemporalVar;
	}
}

//zero-phase filtering, with mirror signals on the edges
void CBoxAlgorithmTemporalFilter::filtfilt2mirror (Dsp::Filter* pFilter1, Dsp::Filter* pFilter2, uint32_t SampleCount, double* buffer)
{
	uint32_t j;

	//construction of mirror signals
	uint32_t l_ui32TransientLength = std::min( 3*(pFilter1->getPoleZeros().size()-1) , SampleCount/2 );
	std::vector<double> l_vBuffer;
	l_vBuffer.resize(SampleCount+2*l_ui32TransientLength);

	for (j=0; j<l_ui32TransientLength; ++j)
	{
		l_vBuffer[j] = 2*buffer[0]-buffer[l_ui32TransientLength-j];
	}
	for (j=0; j<SampleCount; ++j)
	{
		l_vBuffer[j+l_ui32TransientLength] = buffer[j];
	}
	for (j=0; j<l_ui32TransientLength; ++j)
	{
		l_vBuffer[j+l_ui32TransientLength+SampleCount] = 2*buffer[SampleCount-1]-buffer[SampleCount-1-j-1];
	}
	SampleCount+=2*l_ui32TransientLength;

	double* pBufferTemp;
	pBufferTemp = &l_vBuffer[0];

	//zero-phase filtering
	filtfilt2 (pFilter1, pFilter2, SampleCount, pBufferTemp);

	//central part of the buffer
	for (j=0; j<SampleCount-2*l_ui32TransientLength; ++j)
	{
		buffer[j] = pBufferTemp[j+l_ui32TransientLength];
	}
}
#endif

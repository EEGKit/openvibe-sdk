#include "ovpCBoxAlgorithmTemporalFilter.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

namespace
{
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass<32>, 1, Dsp::DirectFormII> CButterworthBandPass;
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandStop<32>, 1, Dsp::DirectFormII> CButterworthBandStop;
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::HighPass<32>, 1, Dsp::DirectFormII> CButterworthHighPass;
	typedef Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::LowPass<32>, 1, Dsp::DirectFormII> CButterworthLowPass;

	std::shared_ptr<Dsp::Filter> createButterworthFilter(const size_t type, const size_t nSmooth)
	{
		switch (type)
		{
			case BandPass: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthBandPass>(int(nSmooth)));
			case BandStop: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthBandStop>(int(nSmooth)));
			case HighPass: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthHighPass>(int(nSmooth)));
			case LowPass: return std::static_pointer_cast<Dsp::Filter>(std::make_shared<CButterworthLowPass>(int(nSmooth)));
			default:
				break;
		}
		return nullptr;
	}

	bool getButterworthParameters(Dsp::Params& parameters, const size_t frequency, const size_t type, const size_t order,
								  const double lowCut, const double highCut, const double /*ripple*/)
	{
		parameters[0] = double(frequency);
		parameters[1] = double(order);
		switch (type)
		{
			case BandPass:
			case BandStop:
				parameters[2] = .5 * (highCut + lowCut);
				parameters[3] = 1. * (highCut - lowCut);
				break;
			case HighPass:
				parameters[2] = lowCut;
				break;
			case LowPass:
				parameters[2] = highCut;
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
		std::shared_ptr < Dsp::Filter > createChebishevFilter(size_t type, size_t nSmooth)
		{
			switch(type)
			{
				case EFilterType::BandPass: return std::shared_ptr < Dsp::Filter >(new CChebyshevBandPass(int(nSmooth)));
				case EFilterType::BandStop: return std::shared_ptr < Dsp::Filter >(new CChebyshevBandStop(int(nSmooth)));
				case EFilterType::HighPass: return std::shared_ptr < Dsp::Filter >(new CChebyshevHighPass(int(nSmooth)));
				case EFilterType::LowPass: return std::shared_ptr < Dsp::Filter >(new CChebyshevLowPass(int(nSmooth)));
				default:
					break;
			}
			return NULL;
		}
	
		bool getChebishevParameters(Dsp::Params& params, size_t type, size_t sampling, size_t order, double lowCut, double highCut, double ripple)
		{
			params[0]=int(sampling);
			params[1]=int(order);
			switch(type)
			{
				case EFilterType::BandPass:
				case EFilterType::BandStop:
					params[2]=.5*(highCut+lowCut);
					params[3]=1.*(highCut-lowCut);
					params[4]=ripple;
					break;
				case EFilterType::HighPass:
					params[2]=highCut; // TO CHECK : lowCut ?
					params[3]=ripple;
					break;
				case EFilterType::LowPass:
					params[2]=highCut;
					params[3]=lowCut;  // TO CHECK : ripple ?
					break;
				default:
					return false;
			}
			return true;
		}
		*/

	typedef bool (*fpGetParameters_t)(Dsp::Params& params, size_t type, size_t sampling, size_t order, double lowCut, double highCut, double ripple);
	typedef std::shared_ptr<Dsp::Filter> (*fpCreateFilter_t)(size_t type, size_t nSmooth);
}  // namespace

bool CBoxAlgorithmTemporalFilter::initialize()
{
	m_method            = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));	// cast Needed for x32
	m_type              = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));	// cast Needed for x32
	const int64_t order = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_lowCut            = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_highCut           = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	OV_ERROR_UNLESS_KRF(order >= 1, "Invalid filter order [" << order << "] (expected value >= 1)", ErrorType::BadSetting);

	m_order = size_t(order);

	if (m_type == LowPass)
	{
		OV_ERROR_UNLESS_KRF(m_highCut > 0, "Invalid high cut-off frequency [" << m_highCut << "] (expected value > 0)", ErrorType::BadSetting);
	}
	else if (m_type == HighPass)
	{
		OV_ERROR_UNLESS_KRF(m_lowCut > 0, "Invalid low cut-off frequency [" << m_lowCut << "] (expected value > 0)", ErrorType::BadSetting);
	}
	else if (m_type == BandPass || m_type == BandStop)
	{
		OV_ERROR_UNLESS_KRF(m_lowCut >= 0, "Invalid low cut-off frequency [" << m_lowCut << "] (expected value >= 0)", ErrorType::BadSetting);
		OV_ERROR_UNLESS_KRF(m_highCut > 0, "Invalid high cut-off frequency [" << m_highCut << "] (expected value > 0)", ErrorType::BadSetting);
		OV_ERROR_UNLESS_KRF(m_highCut > m_lowCut,
							"Invalid cut-off frequencies [" << m_lowCut << "," << m_highCut << "] (expected low frequency < high frequency)",
							ErrorType::BadSetting);
	}
	else { OV_ERROR_KRF("Invalid filter type [" << m_type << "]", ErrorType::BadSetting); }

	m_decoder.initialize(*this, 0);
	m_encoder.initialize(*this, 0);

	m_encoder.getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate());
	m_encoder.getInputMatrix().setReferenceTarget(m_decoder.getOutputMatrix());

	return true;
}

bool CBoxAlgorithmTemporalFilter::uninitialize()
{
	m_decoder.uninitialize();
	m_encoder.uninitialize();

	return true;
}

bool CBoxAlgorithmTemporalFilter::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTemporalFilter::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	size_t j;

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		m_decoder.decode(i);

		const size_t nChannel = m_decoder.getOutputMatrix()->getDimensionSize(0);
		const size_t nSample  = m_decoder.getOutputMatrix()->getDimensionSize(1);

		if (m_decoder.isHeaderReceived())
		{
			if (m_type != LowPass) // verification for high-pass, band-pass and band-stop filters
			{
				OV_ERROR_UNLESS_KRF(m_lowCut <= m_decoder.getOutputSamplingRate()*.5,
									"Invalid low cut-off frequency [" << m_lowCut << "] (expected value must meet nyquist criteria for sampling rate "
									<< m_decoder.getOutputSamplingRate() << ")", ErrorType::BadConfig);
			}
			if (m_type != HighPass) // verification for low-pass, band-pass and band-stop filters
			{
				OV_ERROR_UNLESS_KRF(m_highCut <= m_decoder.getOutputSamplingRate()*.5,
									"Invalid high cut-off frequency [" << m_highCut << "] (expected value must meet nyquist criteria for sampling rate "
									<< m_decoder.getOutputSamplingRate() << ")", ErrorType::BadConfig);
			}

			m_filters.clear();
			//m_vFilter2.clear();

			fpGetParameters_t fpGetParameters;
			fpCreateFilter_t fpCreateFilter;
			if (m_method == Butterworth) // Butterworth
			{
				fpGetParameters = getButterworthParameters;
				fpCreateFilter  = createButterworthFilter;
			}
			else if (m_method == Chebyshev) // Chebyshev
			{
				OV_ERROR_KRF("Chebyshev method not implemented", ErrorType::NotImplemented);
				//fpGetParameters = getChebishevParameters;
				//fpCreateFilter = createChebishevFilter;
			}
			else if (m_method == YuleWalker) // YuleWalker
			{
				OV_ERROR_KRF("YuleWalker method not implemented", ErrorType::NotImplemented);
				//fpGetParameters = getYuleWalkerParameters;
				//fpCreateFilter = createYuleWalkerFilter;
			}
			else { OV_ERROR_KRF("Invalid filter method [" << m_method << "]", ErrorType::BadSetting); }

			if (m_type == HighPass) { this->getLogManager() << LogLevel_Debug << "Low cut frequency of the High pass filter : " << m_lowCut << "Hz\n"; }
			if (m_type == LowPass) { this->getLogManager() << LogLevel_Debug << "High cut frequency of the Low pass filter : " << m_highCut << "Hz\n"; }

			const size_t frequency = m_decoder.getOutputSamplingRate();
			const size_t nSmooth   = 100 * frequency;
			Dsp::Params params;
			(*fpGetParameters)(params, frequency, m_type, m_order, m_lowCut, m_highCut, m_ripple);

			for (j = 0; j < nChannel; ++j)
			{
				std::shared_ptr<Dsp::Filter> filter = (*fpCreateFilter)(m_type, nSmooth);
				filter->setParams(params);
				m_filters.push_back(filter);
				/*std::shared_ptr < Dsp::Filter > filter2=(*fpCreateFilter)(m_type, nSmoothingSample);
				l_pFilter2->setParams(filterParameters);
				m_vFilter2.push_back(filter2);*/
			}

			m_encoder.encodeHeader();
		}
		if (m_decoder.isBufferReceived())
		{
			double* buffer = m_decoder.getOutputMatrix()->getBuffer();

			//"french cook" to reduce transient for bandpass and highpass filters
			if (m_firstSamples.empty())
			{
				m_firstSamples.resize(nChannel, 0); //initialization to 0
				if (m_type == BandPass || m_type == HighPass)
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
				for (size_t k = 0; k < nSample; ++k) { buffer[k] -= m_firstSamples[j]; }

				if (m_filters[j]) { m_filters[j]->process(int(nSample), &buffer); }
				buffer += nSample;
			}
			m_encoder.encodeBuffer();
		}
		if (m_decoder.isEndReceived()) { m_encoder.encodeEnd(); }
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}

/*
//zero-phase filtering, with two different filters
void CBoxAlgorithmTemporalFilter::filtfilt2(std::shared_ptr < Dsp::Filter > pFilter1, std::shared_ptr < Dsp::Filter > pFilter2, size_t SampleCount, double* buffer)
{
	size_t j;

	//1rst filtering
	pFilter1->process(SampleCount, &buffer);

	//reversal of the buffer
	for (j=0; j<SampleCount/2; ++j)
	{
		double temporalVar = buffer[j];
		buffer[j] = buffer[SampleCount-1-j];
		buffer[SampleCount-1-j] = temporalVar;
	}

	//2nd filtering
	pFilter2->process(SampleCount, &buffer);

	//reversal of the buffer
	for (j=0; j<SampleCount/2; ++j)
	{
		double temporalVar = buffer[j];
		buffer[j] = buffer[SampleCount-1-j];
		buffer[SampleCount-1-j] = temporalVar;
	}
}

//zero-phase filtering, with mirror signals on the edges
void CBoxAlgorithmTemporalFilter::filtfilt2mirror (Dsp::Filter* pFilter1, Dsp::Filter* pFilter2, size_t SampleCount, double* buffer)
{
	size_t j;

	//construction of mirror signals
	size_t transientLength = std::min( 3*(pFilter1->getPoleZeros().size()-1), SampleCount/2 );
	std::vector<double> tmp;
	tmp.resize(SampleCount+2*transientLength);

	for (j=0; j<transientLength; ++j)
	{
		tmp[j] = 2*buffer[0]-buffer[transientLength-j];
	}
	for (j=0; j<SampleCount; ++j)
	{
		tmp[j+transientLength] = buffer[j];
	}
	for (j=0; j<transientLength; ++j)
	{
		tmp[j+transientLength+SampleCount] = 2*buffer[SampleCount-1]-buffer[SampleCount-1-j-1];
	}
	SampleCount+=2*transientLength;

	double* res;
	res = &tmp[0];

	//zero-phase filtering
	filtfilt2 (pFilter1, pFilter2, SampleCount, res);

	//central part of the buffer
	for (j=0; j<SampleCount-2*transientLength; ++j)
	{
		buffer[j] = res[j+transientLength];
	}
}
*/

#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdint>

#include <r8brain/CDSPResampler.h>

namespace Common
{
	namespace Resampler
	{
		typedef enum
		{
			ResamplerStoreMode_ChannelWise,
			ResamplerStoreMode_SampleWise,
		} EResamplerStoreMode;

		template <class TFloat, EResamplerStoreMode TStoreMode>
		class TResampler
		{
		public:

			class ICallback
			{
			public:

				virtual ~ICallback() { }
				virtual void processResampler(const TFloat* pSample, const size_t nChannel) const = 0;
			};

		private:

			TResampler(const TResampler<TFloat, TStoreMode>&) = default;

		public:

			/* 
			 * Constructor, with default values for real-time processing.
			 */
			TResampler()
			{
				this->clear();
				switch (TStoreMode)
				{
					case ResamplerStoreMode_ChannelWise:
						m_fpResample = &TResampler<TFloat, TStoreMode>::resampleChannelWise;
						m_fpResampleDirect = &TResampler<TFloat, TStoreMode>::resampleChannelWise;
						break;
					case ResamplerStoreMode_SampleWise:
						m_fpResample = &TResampler<TFloat, TStoreMode>::resampleSampleWise;
						m_fpResampleDirect = &TResampler<TFloat, TStoreMode>::resampleSampleWise;
						break;
					default:
						assert(false);
				}
			}

			virtual ~TResampler() { this->clear(); }

			void clear()
			{
				for (size_t j = 0; j < m_resamplers.size(); ++j) { delete m_resamplers[j]; }
				m_resamplers.clear();

				m_nChannel                     = 0;
				m_iSampling                    = 0;
				m_oSampling                    = 0;
				m_nFractionalDelayFilterSample = 6;
				m_iMaxNSampleIn                = 1024;
				m_transitionBandPercent        = 45;
				m_stopBandAttenuation          = 49;
			}

			/* 
			 * Specifies the number of samples (taps) each fractional delay filter should have.
			 * This must be an even value. To achieve a higher resampling precision, the oversampling 
			 * should be used in the first place instead of using a higher FractionalDelayFilterSampleCount 
			 * value. The lower this value is the lower the signal-to-noise performance of the interpolator
			 * will be. Each FractionalDelayFilterSampleCount decrease by 2 decreases SNR by approximately
			 * 12 to 14 decibels.
			 * Between 6 and 30, and default = 6.
			 *
			 * For real-time processing, n = 6.
			 */
			bool setFractionalDelayFilterSampleCount(const int n = 6)
			{
				if (n < 6 || 30 < n || n % 2 == 1) { return false; }	// false if odd value
				m_nFractionalDelayFilterSample = n;
				return true;
			}

			/* 
			 * Maximal planned length of the input buffer (in samples) that will be passed to the resampler.
			 * The resampler relies on this value as it allocates intermediate buffers. 
			 * Input buffers longer than this value should never be supplied to the resampler. 
			 * Note that the resampler may use the input buffer itself for intermediate sample data storage.
			 */
			bool setMaxNSampleIn(const int n = 8)
			{
				if (n < 8 || 2048 < n) { return false; }
				m_iMaxNSampleIn = n;
				return true;
			}

			/* 
			 * Transition band, in percent of the spectral space of the input signal (or the output signal
			 * if downsampling is performed) between filter's -3 dB point and the Nyquist frequency. 
			 * Between 0.5% and 45%, and default = 10.
			 *
			 * For real-time processing, n = 45.
			 */
			bool setTransitionBand(const double n = 10)
			{
				if (n < r8b::CDSPFIRFilter::getLPMinTransBand() || r8b::CDSPFIRFilter::getLPMaxTransBand() < n) { return false; }
				m_transitionBandPercent = n;
				return true;
			}

			/* 
			 * Stop-band attenuation in decibel.
			 * The general formula is 6.02 * Bits + 40, where "Bits" is the bit resolution (e.g. 16, 24),
			 * "40" is an added resolution for stationary signals, this value can be decreased to 20 to 10
			 * if the signal being resampled is mostly non-stationary (e.g. impulse response).
			 * Between 49 and 218 decibels, and default is given by the general formula.
			 *
			 * For real-time processing, n = 49.
			 */
			bool setStopBandAttenuation(const double n = 49)
			{
				if (n < r8b::CDSPFIRFilter::getLPMinAtten() || r8b::CDSPFIRFilter::getLPMaxAtten() < n) { return false; }
				m_stopBandAttenuation = n;
				return true;
			}

			/* 
			 * This fonction initializes the vector of Resampler, using the number of channels, the input and the output sampling rates.
			 */
			bool reset(const size_t nChannel, const size_t iSampling, const size_t oSampling)
			{
				if (nChannel == 0 || iSampling == 0 || oSampling == 0) { return false; }
				m_iSampling = iSampling;
				m_oSampling = oSampling;

				for (size_t i = 0; i < m_resamplers.size(); ++i) { delete m_resamplers[i]; }
				m_resamplers.clear();
				m_resamplers.resize(nChannel);

				const double in                  = double(iSampling), out = double(oSampling);
				const double stopBandAttenuation = m_stopBandAttenuation == 0
													   ? std::min(6.02 * m_nFractionalDelayFilterSample + 40, r8b::CDSPFIRFilter::getLPMaxAtten())
													   : m_stopBandAttenuation;

				for (size_t j = 0; j < nChannel; ++j)
				{
					switch (m_nFractionalDelayFilterSample) // it defines iFractionalDelayPositionCount 
					{
						case 6:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<6, 11>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 8:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<8, 17>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 10:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<10, 23>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 12:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<12, 41>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 14:
							//stopBandAttenuation = 109.56;
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<14, 67>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 16:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<16, 97>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 18:
							//stopBandAttenuation = 136.45;
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<18, 137>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 20:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<20, 211>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 22:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<22, 353>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 24:
							//stopBandAttenuation = 180.15;
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<24, 673>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 26:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<26, 1051>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 28:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<28, 1733>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 30:
							m_resamplers[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<30, 2833>>(
								in, out, m_iMaxNSampleIn, m_transitionBandPercent, stopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						default:
							return false;
					}
				}

				return true;
			}

			/*
			 * The latency, in samples, which is present in the output signal.
			 * This value is usually zero if the DSP processor "consumes" the latency
			 * automatically. (from CDSPProcessor.h)
			 */
			virtual int getLatency() const { return m_resamplers[0]->getLatency(); }

			/*
			 * Fractional latency, in samples, which is present in the output
			 * signal. This value is usually zero if a linear-phase filtering is used.
			 * With minimum-phase filters in use, this value can be non-zero even if
			 * the getLatency() function returns zero. (from CDSPProcessor.h)
			 */
			virtual double getLatencyFrac() const { return m_resamplers[0]->getLatencyFrac(); }

			/*
			 * The cumulative number of samples that should be passed to *this
			 * object before the actual output starts. This value includes latencies
			 * induced by all processors which run after *this processor in chain.
			 * @param NextInLen The number of input samples required before the output
			 * starts on the next resampling step. (from CDSPProcessor.h)
			 */
			virtual int getInLenBeforeOutStart(const int nextInLen) const { return m_resamplers[0]->getInLenBeforeOutStart(nextInLen); }

			/*
			 * The maximal length of the output buffer required when processing
			 * the "MaxInLen" number of input samples.
			 * @param MaxInLen The number of samples planned to process at once, at
			 * most. (from CDSPProcessor.h)
			 */
			virtual int getMaxOutLen(const int maxInLen) const { return m_resamplers[0]->getMaxOutLen(maxInLen); }

			double getBuiltInLatency() const { return (m_iSampling != 0) ? (1.0 * m_resamplers[0]->getInLenBeforeOutStart(0) / m_iSampling) : 0.0; }

			size_t resample(const ICallback& callback, const TFloat* iSample, const size_t nInSample)
			{
				return (this->*m_fpResample)(callback, iSample, nInSample);
			}

			size_t downsample(const ICallback& callback, const TFloat* iSample, const size_t nInSample)
			{
				return (this->*m_fpResample)(callback, iSample, nInSample);
			}

		private:

			/*
			 * This function resamples the signal assuming the input samples are ordered this way :
			 *  - sample 1 of channel 1, sample 1 of channel 2, ..., sample 1 of channel nChannel,
			 *  - sample 2 of channel 1, sample 2 of channel 2, ..., sample 2 of channel nChannel,
			 *  - ...
			 *  - sample nInSample of channel 1, sample nInSample of channel 2, ..., sample nInSample of channel nChannel,
			 *
			 * This is convenient for resampling at the acquisition level.
			 */
			size_t resampleChannelWise(const ICallback& callback, const TFloat* iSample, const size_t nInSample)
			{
				int nI              = 0;
				bool isFirstChannel = true;

				std::vector<double> iBuffers(nInSample);
				std::vector<TFloat> oBuffers;

				for (size_t j = 0; j < m_nChannel; ++j)
				{
					for (size_t k = 0; k < nInSample; ++k) { iBuffers[k] = double(iSample[k * m_nChannel + j]); }

					double* resamplerOutputBuffer;
					nI = m_resamplers[j]->process(&iBuffers[0], int(nInSample), resamplerOutputBuffer);

					if (isFirstChannel)
					{
						oBuffers.resize(nI * m_nChannel);
						isFirstChannel = false;
					}

					for (int k = 0; k < nI; ++k) { oBuffers[k * m_nChannel + j] = TFloat(resamplerOutputBuffer[k]); }
				}

				for (int k = 0; k < nI; ++k) { callback.processResampler(&oBuffers[k * m_nChannel], m_nChannel); }

				return nI;
			}

			/*
			 * This function resamples the signal assuming the input samples are ordered this way :
			 *  - sample 1 of channel 1, sample 2 of channel 1, ..., sample nInSample of channel 1,
			 *  - sample 1 of channel 2, sample 2 of channel 2, ..., sample nInSample of channel 2,
			 *  - ...
			 *  - sample 1 of channel nChannel, sample 2 of channel nChannel, ..., sample nInSample of channel nChannel,
			 *
			 * This is convenient for resampling at the signal-processing level.
			 */
			size_t resampleSampleWise(const ICallback& callback, const TFloat* iSample, const size_t nInSample)
			{
				int nI              = 0;
				bool isFirstChannel = true;

				std::vector<double> iBuffers(nInSample);
				std::vector<TFloat> oBuffers;

				for (size_t j = 0; j < m_nChannel; ++j)
				{
					for (size_t k = 0; k < nInSample; ++k) { iBuffers[k] = double(iSample[j * nInSample + k]); }

					double* resamplerOutputBuffer;
					nI = m_resamplers[j]->process(&iBuffers[0], int(nInSample), resamplerOutputBuffer);

					if (isFirstChannel)
					{
						oBuffers.resize(nI * m_nChannel);
						isFirstChannel = false;
					}

					for (int k = 0; k < nI; ++k) { oBuffers[k * m_nChannel + j] = TFloat(resamplerOutputBuffer[k]); }
				}

				for (int k = 0; k < nI; ++k) { callback.processResampler(&oBuffers[k * m_nChannel], m_nChannel); }

				return nI;
			}

		protected:

			/*
			 * Trivial channel wise callback implementation
			 */
			class CCallbackChannelWise final : public ICallback
			{
			public:
				CCallbackChannelWise(TFloat* oSample) : m_pOutputSample(oSample) { }

				void processResampler(const TFloat* sample, const size_t nChannel) const override
				{
					for (size_t i = 0; i < nChannel; ++i)
					{
						*m_pOutputSample = *sample;
						++m_pOutputSample;
						++sample;
					}
				}

				mutable TFloat* m_pOutputSample;
			};

			/*
			 * Trivial sample wise callback implementation
			 */
			class CCallbackSampleWise final : public ICallback
			{
			public:
				CCallbackSampleWise(TFloat* oSample, const size_t nOutSample) : m_OutputSample(oSample), m_NOutputSample(nOutSample) { }

				void processResampler(const TFloat* sample, const size_t nChannel) const override
				{
					for (size_t i = 0; i < nChannel; ++i) { m_OutputSample[i * m_NOutputSample + m_OutputSampleIdx] = sample[i]; }
					m_OutputSampleIdx++;
					m_OutputSampleIdx %= m_NOutputSample;
				}

				TFloat* const m_OutputSample;
				size_t m_NOutputSample           = 0;
				mutable size_t m_OutputSampleIdx = 0;
			};

			size_t resampleChannelWise(TFloat* oSample, const TFloat* iSample, const size_t nInSample, const size_t /*nOutSample*/)
			{
				return this->resample(CCallbackChannelWise(oSample), iSample, nInSample);
			}

			size_t resampleSampleWise(TFloat* oSample, const TFloat* iSample, const size_t nInSample, const size_t nOutSample)
			{
				return this->resample(CCallbackSampleWise(oSample, nOutSample), iSample, nInSample);
			}

		public:

			size_t resample(TFloat* oSample, const TFloat* iSample, const size_t nInSample, const size_t nOutSample = 1)
			{
				return (this->*m_fpResampleDirect)(oSample, iSample, nInSample, nOutSample);
			}

			size_t downsample(TFloat* oSample, const TFloat* iSample, const size_t nInSample, const size_t nOutSample = 1)
			{
				return resample(oSample, iSample, nInSample, nOutSample);
			}

		protected:

			size_t m_nChannel  = 0;
			size_t m_iSampling = 0;
			size_t m_oSampling = 0;

			int m_nFractionalDelayFilterSample = 6;
			int m_iMaxNSampleIn                = 1024;
			double m_transitionBandPercent     = 45;
			double m_stopBandAttenuation       = 49;

			std::vector<r8b::CDSPProcessor*> m_resamplers;

			size_t (TResampler<TFloat, TStoreMode>::*m_fpResample)(const ICallback& callback, const TFloat* iSample, const size_t nInSample);
			size_t (TResampler<TFloat, TStoreMode>::*m_fpResampleDirect)(TFloat* oSample, const TFloat* iSample, const size_t nInSample, const size_t nOutSample);
		};

		typedef TResampler<float, ResamplerStoreMode_SampleWise> CResamplerSf;
		typedef TResampler<float, ResamplerStoreMode_ChannelWise> CResamplerCf;
		typedef TResampler<double, ResamplerStoreMode_SampleWise> CResamplerSd;
		typedef TResampler<double, ResamplerStoreMode_ChannelWise> CResamplerCd;

		typedef TResampler<float, ResamplerStoreMode_SampleWise> CDownsamplerSf;
		typedef TResampler<float, ResamplerStoreMode_ChannelWise> CDownsamplerCf;
		typedef TResampler<double, ResamplerStoreMode_SampleWise> CDownsamplerSd;
		typedef TResampler<double, ResamplerStoreMode_ChannelWise> CDownsamplerCd;
	} // namespace Resampler
} // namespace Common

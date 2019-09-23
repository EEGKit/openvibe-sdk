#pragma once

#include <vector>
#include <cmath>
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

		template <class TFloat, EResamplerStoreMode eStoreMode>
		class TResampler
		{
		public:

			class ICallback
			{
			public:

				virtual ~ICallback() { }
				virtual void processResampler(const TFloat* pSample, size_t nChannel) const = 0;
			};

		private:

			TResampler(const TResampler<TFloat, eStoreMode>&);

		public:

			/* 
			 * Constructor, with default values for real-time processing.
			 */
			TResampler()
			{
				this->clear();
				switch (eStoreMode)
				{
					case ResamplerStoreMode_ChannelWise:
						m_fpResample = &TResampler<TFloat, eStoreMode>::resample_channel_wise;
						m_fpResampleDirect = &TResampler<TFloat, eStoreMode>::resample_channel_wise;
						break;
					case ResamplerStoreMode_SampleWise:
						m_fpResample = &TResampler<TFloat, eStoreMode>::resample_sample_wise;
						m_fpResampleDirect = &TResampler<TFloat, eStoreMode>::resample_sample_wise;
						break;
					default:
						assert(false);
				}
			}

			~TResampler() { this->clear(); }

			void clear()
			{
				for (size_t j = 0; j < m_vResampler.size(); j++) { delete m_vResampler[j]; }
				m_vResampler.clear();

				nChannel                            = 0;
				m_iSamplingRate             = 0;
				m_oSamplingRate            = 0;
				m_nFractionalDelayFilterSample = 6;
				m_iMaxInputSampleCount              = 1024;
				m_transitionBandPercent        = 45;
				m_stopBandAttenuation            = 49;
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
			bool setFractionalDelayFilterSampleCount(int n = 6)
			{
				if (n < 6 || 30 < n) { return false; }
				if (n % 2 == 1) { return false; }	// false if odd value
				m_nFractionalDelayFilterSample = n;
				return true;
			}

			/* 
			 * Maximal planned length of the input buffer (in samples) that will be passed to the resampler.
			 * The resampler relies on this value as it allocates intermediate buffers. 
			 * Input buffers longer than this value should never be supplied to the resampler. 
			 * Note that the resampler may use the input buffer itself for intermediate sample data storage.
			 */
			bool setMaxInputSampleCount(int n = 8)
			{
				if (n < 8 || 2048 < n) { return false; }
				m_iMaxInputSampleCount = n;
				return true;
			}

			/* 
			 * Transition band, in percent of the spectral space of the input signal (or the output signal
			 * if downsampling is performed) between filter's -3 dB point and the Nyquist frequency. 
			 * Between 0.5% and 45%, and default = 10.
			 *
			 * For real-time processing, n = 45.
			 */
			bool setTransitionBand(double n = 10)
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
			bool setStopBandAttenuation(double n = 49)
			{
				if (n < r8b::CDSPFIRFilter::getLPMinAtten() || r8b::CDSPFIRFilter::getLPMaxAtten() < n) { return false; }
				m_stopBandAttenuation = n;
				return true;
			}

			/* 
			 * This fonction initializes the vector of Resampler, using the number of channels, the input and the output sampling rates.
			 */
			bool reset(size_t nChannel, uint32_t iSampleRate, uint32_t oSampleRate)
			{
				if (nChannel == 0) { return false; }
				if (iSampleRate == 0) { return false; }
				if (oSampleRate == 0) { return false; }
				nChannel                 = nChannel;
				m_iSamplingRate  = iSampleRate;
				m_oSamplingRate = oSampleRate;

				for (size_t i = 0; i < m_vResampler.size(); i++) { delete m_vResampler[i]; }
				m_vResampler.clear();
				m_vResampler.resize(nChannel);

				double l_f64StopBandAttenuation = m_stopBandAttenuation == 0
												  ? std::min(6.02 * m_nFractionalDelayFilterSample + 40, r8b::CDSPFIRFilter::getLPMaxAtten())
												  : m_stopBandAttenuation;

				for (size_t j = 0; j < nChannel; j++)
				{
					switch (m_nFractionalDelayFilterSample) // it defines iFractionalDelayPositionCount 
					{
						case 6:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<6, 11>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 8:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<8, 17>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 10:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<10, 23>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 12:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<12, 41>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 14:
							//l_f64StopBandAttenuation = 109.56;
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<14, 67>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 16:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<16, 97>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 18:
							//l_f64StopBandAttenuation = 136.45;
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<18, 137>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 20:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<20, 211>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 22:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<22, 353>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 24:
							//l_f64StopBandAttenuation = 180.15;
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<24, 673>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 26:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<26, 1051>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 28:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<28, 1733>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
								r8b::EDSPFilterPhaseResponse(0), false);
							break;

						case 30:
							m_vResampler[j] = new r8b::CDSPResampler<r8b::CDSPFracInterpolator<30, 2833>>(
								iSampleRate, oSampleRate, m_iMaxInputSampleCount, m_transitionBandPercent, l_f64StopBandAttenuation,
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
			virtual int getLatency() const { return m_vResampler[0]->getLatency(); }

			/*
			 * Fractional latency, in samples, which is present in the output
			 * signal. This value is usually zero if a linear-phase filtering is used.
			 * With minimum-phase filters in use, this value can be non-zero even if
			 * the getLatency() function returns zero. (from CDSPProcessor.h)
			 */
			virtual double getLatencyFrac() const { return m_vResampler[0]->getLatencyFrac(); }

			/*
			 * The cumulative number of samples that should be passed to *this
			 * object before the actual output starts. This value includes latencies
			 * induced by all processors which run after *this processor in chain.
			 * @param NextInLen The number of input samples required before the output
			 * starts on the next resampling step. (from CDSPProcessor.h)
			 */
			virtual int getInLenBeforeOutStart(const int NextInLen) const { return m_vResampler[0]->getInLenBeforeOutStart(NextInLen); }

			/*
			 * The maximal length of the output buffer required when processing
			 * the "MaxInLen" number of input samples.
			 * @param MaxInLen The number of samples planned to process at once, at
			 * most. (from CDSPProcessor.h)
			 */
			virtual int getMaxOutLen(const int MaxInLen) const { return m_vResampler[0]->getMaxOutLen(MaxInLen); }

			float getBuiltInLatency() const
			{
				return (m_iSamplingRate != 0) ? (1.0f * m_vResampler[0]->getInLenBeforeOutStart(0) / m_iSamplingRate) : 0.f;
			}

			size_t resample(const ICallback& rCallback, const TFloat* pInputSample, size_t nSampleIn)
			{
				return (this->*m_fpResample)(rCallback, pInputSample, nSampleIn);
			}

			size_t downsample(const ICallback& rCallback, const TFloat* pInputSample, size_t nSampleIn)
			{
				return (this->*m_fpResample)(rCallback, pInputSample, nSampleIn);
			}

		private:

			/*
			 * This function resamples the signal assuming the input samples are ordered this way :
			 *  - sample 1 of channel 1, sample 1 of channel 2, ..., sample 1 of channel nChannel,
			 *  - sample 2 of channel 1, sample 2 of channel 2, ..., sample 2 of channel nChannel,
			 *  - ...
			 *  - sample nSampleIn of channel 1, sample nSampleIn of channel 2, ..., sample nSampleIn of channel nChannel,
			 *
			 * This is convenient for resampling at the acquisition level.
			 */
			size_t resample_channel_wise(const ICallback& rCallback, const TFloat* pInputSample, size_t nSampleIn)
			{
				int nI              = 0;
				bool isFirstChannel = true;

				std::vector<double> l_vInputBuffer(nSampleIn);
				std::vector<TFloat> l_vOutputBuffer;

				for (uint32_t j = 0; j < nChannel; j++)
				{
					for (uint32_t k = 0; k < nSampleIn; k++) { l_vInputBuffer[k] = double(pInputSample[k * nChannel + j]); }

					double* l_pResamplerOutputBuffer;
					nI = m_vResampler[j]->process(&l_vInputBuffer[0], int(nSampleIn), l_pResamplerOutputBuffer);

					if (isFirstChannel)
					{
						l_vOutputBuffer.resize(nI * nChannel);
						isFirstChannel = false;
					}

					for (int k = 0; k < nI; k++) { l_vOutputBuffer[k * nChannel + j] = TFloat(l_pResamplerOutputBuffer[k]); }
				}

				for (int k = 0; k < nI; k++) { rCallback.processResampler(&l_vOutputBuffer[k * nChannel], nChannel); }

				return nI;
			}

			/*
			 * This function resamples the signal assuming the input samples are ordered this way :
			 *  - sample 1 of channel 1, sample 2 of channel 1, ..., sample nSampleIn of channel 1,
			 *  - sample 1 of channel 2, sample 2 of channel 2, ..., sample nSampleIn of channel 2,
			 *  - ...
			 *  - sample 1 of channel nChannel, sample 2 of channel nChannel, ..., sample nSampleIn of channel nChannel,
			 *
			 * This is convenient for resampling at the signal-processing level.
			 */
			size_t resample_sample_wise(const ICallback& rCallback, const TFloat* pInputSample, size_t nSampleIn)
			{
				int nI              = 0;
				bool isFirstChannel = true;

				std::vector<double> l_vInputBuffer(nSampleIn);
				std::vector<TFloat> l_vOutputBuffer;

				for (size_t j = 0; j < nChannel; j++)
				{
					for (size_t k = 0; k < nSampleIn; k++) { l_vInputBuffer[k] = double(pInputSample[j * nSampleIn + k]); }

					double* l_pResamplerOutputBuffer;
					nI = m_vResampler[j]->process(&l_vInputBuffer[0], int(nSampleIn), l_pResamplerOutputBuffer);

					if (isFirstChannel)
					{
						l_vOutputBuffer.resize(nI * nChannel);
						isFirstChannel = false;
					}

					for (int k = 0; k < nI; k++) { l_vOutputBuffer[k * nChannel + j] = TFloat(l_pResamplerOutputBuffer[k]); }
				}

				for (int k = 0; k < nI; k++) { rCallback.processResampler(&l_vOutputBuffer[k * nChannel], nChannel); }

				return nI;
			}

		protected:

			/*
			 * Trivial channel wise callback implementation
			 */
			class CCallbackChannelWise final : public ICallback
			{
			public:
				CCallbackChannelWise(TFloat* pOutputSample)
					: m_pOutputSample(pOutputSample) { }

				void processResampler(const TFloat* pSample, size_t nChannel) const override
				{
					for (size_t i = 0; i < nChannel; i++)
					{
						*m_pOutputSample = *pSample;
						++m_pOutputSample;
						++pSample;
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
				CCallbackSampleWise(TFloat* pOutputSample, size_t ui32OutputSampleCount)
					: m_pOutputSample(pOutputSample)
					  , m_nOutputSample(ui32OutputSampleCount)
					  , m_ui32OutputSampleIndex(0) { }

				void processResampler(const TFloat* pSample, size_t nChannel) const override
				{
					for (size_t i = 0; i < nChannel; i++) { m_pOutputSample[i * m_nOutputSample + m_ui32OutputSampleIndex] = pSample[i]; }
					m_ui32OutputSampleIndex++;
					m_ui32OutputSampleIndex %= m_nOutputSample;
				}

				TFloat* const m_pOutputSample;
				size_t m_nOutputSample;
				mutable size_t m_ui32OutputSampleIndex;
			};

			size_t resample_channel_wise(TFloat* pOutputSample, const TFloat* pInputSample, size_t nSampleIn, size_t /*ui32OutputSampleCount*/)
			{
				return this->resample(CCallbackChannelWise(pOutputSample), pInputSample, nSampleIn);
			}

			size_t resample_sample_wise(TFloat* pOutputSample, const TFloat* pInputSample, size_t nSampleIn, size_t ui32OutputSampleCount)
			{
				return this->resample(CCallbackSampleWise(pOutputSample, ui32OutputSampleCount), pInputSample, nSampleIn);
			}

		public:

			size_t resample(TFloat* pOutputSample, const TFloat* pInputSample, size_t nSampleIn, size_t ui32OutputSampleCount = 1)
			{
				return (this->*m_fpResampleDirect)(pOutputSample, pInputSample, nSampleIn, ui32OutputSampleCount);
			}

			size_t downsample(TFloat* pOutputSample, const TFloat* pInputSample, size_t nSampleIn, size_t ui32OutputSampleCount = 1)
			{
				return resample(pOutputSample, pInputSample, nSampleIn, ui32OutputSampleCount);
			}

		protected:

			size_t nChannel                 = 0;
			size_t m_iSamplingRate  = 0;
			size_t m_oSamplingRate = 0;

			int m_nFractionalDelayFilterSample = 6;
			int m_iMaxInputSampleCount              = 1024;
			double m_transitionBandPercent     = 45;
			double m_stopBandAttenuation         = 49;

			std::vector<r8b::CDSPProcessor*> m_vResampler;

			size_t (TResampler<TFloat, eStoreMode>::*m_fpResample)(const ICallback& rCallback, const TFloat* pInputSample, size_t nSampleIn);
			size_t (TResampler<TFloat, eStoreMode>::*m_fpResampleDirect)(TFloat* pOutputSample, const TFloat* pInputSample, size_t nSampleIn,
																		 size_t ui32OutputSampleCount);
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

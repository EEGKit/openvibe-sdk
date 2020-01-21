#pragma once

#include <cstdint>

/**
 * \author Loic Mahe. Minor tweaks by Jussi T. Lindgren.
 * \date 2013-03-27
 * \brief Static functions to perform various conversions to and from the OpenViBE 32:32 fixed point time format.
 */

namespace OpenViBE
{
	namespace TimeArithmetics
	{
		/**
		 * \brief Given a sample count and the sampling rate, returns the duration of the samples in fixed point time
		 * \param sampling : the sampling rate of the signal, must not be 0 or the function will crash.
		 * \param sampleCount : the size of the sample
		 * \return Time in fixed point format corresponding to the input parameters
		 */
		inline uint64_t sampleCountToTime(const uint64_t sampling, const uint64_t sampleCount)
		{
			// Note that if sampling is 0, this will crash. Its preferable to silent fail, the caller should check the argument.
			// FIXME: assert or something
			return (sampleCount << 32) / sampling;
		}

		/**
		 * \brief Given a fixed point time and the sampling rate, returns the number of samples obtained
		 * \param sampling : the sampling rate of the signal
		 * \param time : elapsed time in fixed point format
		 * \return Sample count corresponding to the input parameters
		 */
		inline uint64_t timeToSampleCount(const uint64_t sampling, const uint64_t time) { return ((time + 1) * sampling - 1) >> 32; }
		
		/**
		 * \brief Get the time in seconds given a fixed point time
		 * \param time : time in fixed point format
		 * \return Regular floating point time in seconds
		 *
		 */
		inline double timeToSeconds(const uint64_t time) { return time / double(1LL << 32); }

		/**
		 * \brief Get the time in fixed point format given float time in seconds
		 * \param time : Regular floating point time in seconds 
		 * \return Time in fixed point format
		 *
		 */
		inline uint64_t secondsToTime(const double time) { return uint64_t(time * double(1LL << 32)); }
	} // namespace TimeArithmetics
} // namespace OpenViBE

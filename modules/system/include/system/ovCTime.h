#pragma once

#include "defines.h"

namespace System
{
	/**
	 * \class Time
	 * \brief Static functions to handle time within the framework
	 *
	 * \note Please see openvibe/ovITimeArithmetics.h for conversion routines to/from OpenViBE fixed point time
	 *
	 */
	class System_API Time
	{
	public:

		/**
		 * \brief Make the calling thread sleep 
		 * \param ui32MilliSeconds : sleep duration in ms
		 * \return Always true
		 */
		static bool sleep(uint32_t ui32MilliSeconds);
		
		/**
		 * \brief Make the calling thread sleep 
		 * \param ui64Seconds : sleep duration in fixed point 32:32 seconds
		 * \return Always true
		 */
		static bool zsleep(uint64_t ui64Seconds);
		
		/**
		 * \brief Retrieve time in ms 
		 * \return Elapsed time in ms since the first call to this function or zgetTime functions
		 */
		static uint32_t getTime(void); 

		/**
		 * \brief Retrieve time in fixed point 32:32 seconds 
		 * \return Elapsed time since the first call to the zgetTime functions or getTime.
		 */
		static uint64_t zgetTime(void);

		/**
		 * \brief Retrieve time in fixed point 32:32 seconds 
		 * \param If sinceFirstCall is true, returns the time since the first call to the zgetTime function or getTime. 
		 *        Otherwise, returns time since epoch of the clock.
		 * \return Elapsed time 
		 */
		static uint64_t zgetTimeRaw(bool sinceFirstCall = true);

		/**
		 * \brief Check if the internal clock used by the framework is steady
		 * \return True if the clock is steady, false otherwise
		 * \note This is a theoretical check that queries the internal
		 *       clock implementation for available services
		 */
		static bool isClockSteady();
		 
		/**
		 * \brief Check if the internal clock used by the framework has
		 *        a resolution higher than the required one
		 * \param ui32MilliSeconds : Expected clock resolution (period between ticks) in ms (must be non-zero value)
		 * \return True if the clock meets the requirements, false otherwise
		 * \note This is a theoretical check that queries the internal
		 *  	 clock implementation for available services
		 */
		static bool checkResolution(uint32_t ui32MilliSeconds);

	private:

		Time(void);
	};
};

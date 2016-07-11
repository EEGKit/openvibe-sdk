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
		static bool sleep(const System::uint32 ui32MilliSeconds);
		
		/**
		 * \brief Make the calling thread sleep 
		 * \param ui64Seconds : sleep duration in fixed point 32:32 seconds
		 * \return Always true
		 */
		static bool zsleep(const System::uint64 ui64Seconds);
		
		/**
		 * \brief Retrieve time in ms 
		 * \return Elpased time in ms since the first call to this function or zgetTime
		 */
		static System::uint32 getTime(void); 
		
		/**
		 * \brief Retrieve time in fixed point 32:32 seconds 
		 * \return Elpased time since the first call to the function or getTime
		 */
		static System::uint64 zgetTime(void);
		
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
		 static bool checkResolution(const System::uint32 ui32MilliSeconds);
		  
	private:

		Time(void);
	};
};


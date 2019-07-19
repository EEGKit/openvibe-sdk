#include "system/ovCTime.h"

#include <cmath>
#include <cassert>
#include <iostream>

// \warning On Windows, avoid "using namespace System;" here as it may cause confusion with stuff coming from windows/boost
// \note Support of C++11 steady clock:
//       - From GCC 4.8.1
//       - From Visual Studio 2015 (therefore a strategy is needed to handle Visual Studio 2013 version)

// time handling strategy selection
// \note With officialy supported compilers and required boost version
//       it should never fallback in a OV_USE_SYSTEM case
#if (defined(_MSC_VER) && _MSC_VER <= 1800 && defined(TARGET_HAS_Boost_Chrono))

#include <boost/chrono/config.hpp>

#ifdef BOOST_CHRONO_HAS_CLOCK_STEADY

#include <boost/chrono.hpp>
#include <boost/thread.hpp>
namespace timelib = boost;

#else
#error "Please use OpenViBE recommended version of Boost"
#endif // BOOST_CHRONO_HAS_CLOCK_STEADY

#else // defined(_MSC_VER) && _MSC_VER <= 1800 && defined(TARGET_HAS_Boost_Chrono)

#include <chrono>
#include <thread>
namespace timelib = std;

#endif // defined(_MSC_VER) && _MSC_VER <= 1800 && defined(TARGET_HAS_Boost_Chrono)

using internal_clock = timelib::chrono::steady_clock;
// using internal_clock = chrono::high_resolution_clock;

bool System::Time::sleep(const uint32_t ui32MilliSeconds)
{
	timelib::this_thread::sleep_for(timelib::chrono::milliseconds(ui32MilliSeconds));
	return true;
}

bool System::Time::zsleep(const uint64_t ui64Seconds)
{
	const uint32_t l_ui32Seconds = static_cast<uint32_t>(ui64Seconds >> 32);
	// zero the seconds with 0xFFFFFFFF, multiply to get the rest as fixed point microsec, then grab them (now in the 32 msbs)
	const uint64_t l_ui64MicroSeconds = ((ui64Seconds & 0xFFFFFFFFLL) * 1000000LL) >> 32;

	const timelib::chrono::microseconds l_oDuration = timelib::chrono::seconds(l_ui32Seconds) + timelib::chrono::microseconds(l_ui64MicroSeconds);
	timelib::this_thread::sleep_for(l_oDuration);

	return true;
}

uint32_t System::Time::getTime()
{
	// turn the 32:32 fixed point seconds to milliseconds
	return static_cast<uint32_t>((zgetTime() * 1000) >> 32);
}

uint64_t System::Time::zgetTime()
{
	return zgetTimeRaw(true);
}

uint64_t System::Time::zgetTimeRaw(bool sinceFirstCall)
{
	static bool l_bInitialized = false;
	static internal_clock::time_point l_oTimeStart;

	if (!l_bInitialized)
	{
		l_oTimeStart   = internal_clock::now();
		l_bInitialized = true;
	}

	const internal_clock::time_point l_oTimeNow = internal_clock::now();

	const internal_clock::duration l_oElapsed = (sinceFirstCall ? l_oTimeNow - l_oTimeStart : l_oTimeNow.time_since_epoch());

	const timelib::chrono::microseconds l_oElapsedMs = timelib::chrono::duration_cast<timelib::chrono::microseconds>(l_oElapsed);

	const uint64_t l_ui64MicrosPerSecond = 1000ULL * 1000ULL;
	const uint64_t l_ui64Seconds         = static_cast<uint64_t>(l_oElapsedMs.count() / l_ui64MicrosPerSecond);
	const uint64_t l_ui64Fraction        = static_cast<uint64_t>(l_oElapsedMs.count() % l_ui64MicrosPerSecond);

	// below in fraction part, scale [0,l_ui64MicrosPerSecond-1] to 32bit integer range
	const uint64_t l_ui64ReturnValue = (l_ui64Seconds << 32) + l_ui64Fraction * (0xFFFFFFFFLL / (l_ui64MicrosPerSecond - 1));

	return l_ui64ReturnValue;
}

bool System::Time::isClockSteady()
{
	return internal_clock::is_steady;
}

bool System::Time::checkResolution(const uint32_t ui32MilliSeconds)
{
	assert(ui32MilliSeconds != 0);

	auto l_ui32Resolution = static_cast<double>(internal_clock::period::num) / internal_clock::period::den;
	return (static_cast<uint32_t>(std::ceil(l_ui32Resolution * 1000)) <= ui32MilliSeconds);
}

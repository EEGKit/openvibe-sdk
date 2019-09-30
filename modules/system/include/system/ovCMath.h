#pragma once

#include "defines.h"

#if defined TARGET_OS_Windows
#include <float.h>
#elif defined System_OS_Linux
 #include <cmath>
#else
 #include <cmath>
#endif

namespace System
{
	class System_API Math
	{
	public:

		static bool initializeRandomMachine(uint64_t randomSeed);

		static uint8_t randomUInteger8();
		static uint16_t randomUInteger16();
		static uint32_t randomUInteger32();
		static uint64_t randomUInteger64();

		// returns a value in [0,upperLimit( -- i.e. upperLimit not included in range
		static uint32_t randomUInteger32WithCeiling(uint32_t upperLimit);

		static int8_t randomSInteger8();
		static int16_t randomSInteger16();
		static int randomSInteger32();
		static int64_t randomSInteger64();

		static float randomFloat32();
		static float randomFloat32BetweenZeroAndOne();
		static double randomFloat64();

		// Numerical check
		// Checks if the value is normal, subnormal or zero, but not infinite or NAN.
		static bool isfinite(double value);
		// Checks if the value is positive or negative infinity.
		static bool isinf(double value);
		// Checks if the value is Not a Number
		static bool isnan(double value);
		// Checks if the value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
		static bool isnormal(double value);
	private:

		Math() = delete;
	};
} // namespace System

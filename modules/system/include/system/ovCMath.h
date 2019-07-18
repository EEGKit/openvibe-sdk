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

		static bool initializeRandomMachine(const uint64_t ui64RandomSeed);

		static uint8_t randomUInteger8(void);
		static uint16_t randomUInteger16(void);
		static uint32_t randomUInteger32(void);
		static uint64_t randomUInteger64(void);

		// returns a value in [0,ui32upperLimit( -- i.e. ui32upperLimit not included in range
		static uint32_t randomUInteger32WithCeiling(uint32_t ui32upperLimit);

		static int8_t randomSInteger8(void);
		static int16_t randomSInteger16(void);
		static int32_t randomSInteger32(void);
		static int64_t randomSInteger64(void);

		static float randomFloat32(void);
		static float randomFloat32BetweenZeroAndOne(void);
		static double randomFloat64(void);

		// Numerical check
		// Checks if the value is normal, subnormal or zero, but not infinite or NAN.
		static bool isfinite(double f64Value);
		// Checks if the value is positive or negative infinity.
		static bool isinf(double f64Value);
		// Checks if the value is Not a Number
		static bool isnan(double f64Value);
		// Checks if the value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
		static bool isnormal(double f64Value);
	private:

		Math(void);
	};
};

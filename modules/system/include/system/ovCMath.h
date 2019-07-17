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

		static bool initializeRandomMachine(const System::uint64 ui64RandomSeed);

		static System::uint8 randomUInteger8(void);
		static System::uint16 randomUInteger16(void);
		static System::uint32 randomUInteger32(void);
		static System::uint64 randomUInteger64(void);

		// returns a value in [0,ui32upperLimit( -- i.e. ui32upperLimit not included in range
		static System::uint32 randomUInteger32WithCeiling(uint32 ui32upperLimit);

		static System::int8 randomSInteger8(void);
		static System::int16 randomSInteger16(void);
		static System::int32 randomSInteger32(void);
		static System::int64 randomSInteger64(void);

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

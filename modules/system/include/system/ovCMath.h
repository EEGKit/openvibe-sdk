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

		static System::boolean initializeRandomMachine(const System::uint64 ui64RandomSeed);

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

		static System::float32 randomFloat32(void);
		static System::float32 randomFloat32BetweenZeroAndOne(void);
		static System::float64 randomFloat64(void);

		// Numerical check
		// Checks if the value is normal, subnormal or zero, but not infinite or NAN.
		static System::boolean isfinite(System::float64 f64Value);
		// Checks if the value is positive or negative infinity.
		static System::boolean isinf(System::float64 f64Value);
		// Checks if the value is Not a Number
		static System::boolean isnan(System::float64 f64Value);
		// Checks if the value is normal, i.e. is neither zero, subnormal, infinite, nor NaN.
		static System::boolean isnormal(System::float64 f64Value);
	private:

		Math(void);
	};
};


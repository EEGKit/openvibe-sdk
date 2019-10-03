#pragma once

#include "defines.h"

#include <cmath>

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

	private:
		Math() = delete;
	};
} // namespace System

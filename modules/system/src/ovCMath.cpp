/**
 *
 * @fixme This class could benefit from a serious overhaul, e.g. using randomness from some established library or C11.
 *
 * - Here Linear Congruential Generator is re-implemented to avoid third-party dependencies messing the up the rand() state.
 *   This happened before when we used the global srand() / rand(). It made hard to make repeatable experiments on some
 *   platforms. The generated randomness from the introduced home-made class is not super but it should be sufficient for 
 *   OpenViBE's present use-cases.
 *
 * Other notes
 *
 * - The randomFloat32() and randomFloat64() functions might not be working as expected (verify)
 * - Due to generative process, values generated above l_ui32RandMax may not be dense (verify) 
 * - randomUInteger32WithCeiling() may not be dense either
 *
 */
#include "system/ovCMath.h"
#include <cstdlib>
#include <cstring>

using namespace System;

class RandomGenerator
{
	uint32_t m_nextValue = 0;

public:
	static const uint32_t l_ui32RandMax = 0x7FFFFFFF; // (2^32)/2-1 == 2147483647

	explicit RandomGenerator(const uint32_t seed = 1) : m_nextValue(seed) {}

	int rand()
	{
		// Pretty much C99 convention and parameters for a Linear Congruential Generator
		m_nextValue = (m_nextValue * 1103515245 + 12345) & l_ui32RandMax;
		return int(m_nextValue);
	}

	void setSeed(const uint32_t seed) { m_nextValue = seed; }

	uint32_t getSeed() const { return m_nextValue; }
};

// Should be only accessed via Math:: calls defined below
static RandomGenerator g_oRandomGenerator;

bool Math::initializeRandomMachine(const uint64_t randomSeed)
{
	g_oRandomGenerator.setSeed(uint32_t(randomSeed));

	// For safety, we install also the C++ basic random engine (it might be useg by dependencies, old code, etc)
	srand(uint32_t(randomSeed));

	return true;
}

uint8_t Math::randomUInteger8() { return uint8_t(randomUInteger64()); }

uint16_t Math::randomUInteger16() { return uint16_t(randomUInteger64()); }

uint32_t Math::randomUInteger32() { return uint32_t(randomUInteger64()); }

uint64_t Math::randomUInteger64()
{
	const uint64_t r1 = g_oRandomGenerator.rand();
	const uint64_t r2 = g_oRandomGenerator.rand();
	const uint64_t r3 = g_oRandomGenerator.rand();
	const uint64_t r4 = g_oRandomGenerator.rand();
	return (r1 << 24) ^ (r2 << 16) ^ (r3 << 8) ^ (r4);
}

uint32_t Math::randomUInteger32WithCeiling(uint32_t upperLimit)
{
	// float in range [0,1]
	const double l_f64Temp = g_oRandomGenerator.rand() / double(g_oRandomGenerator.l_ui32RandMax);

	// static_cast is effectively floor(), so below we get output range [0,upperLimit-1], without explicit subtraction of 1
	const uint32_t l_ui32ReturnValue = uint32_t(upperLimit * l_f64Temp);

	return l_ui32ReturnValue;
}

int8_t Math::randomSInteger8() { return int8_t(randomUInteger64()); }

int16_t Math::randomSInteger16() { return int16_t(randomUInteger64()); }

int Math::randomSInteger32() { return int(randomUInteger64()); }

int64_t Math::randomSInteger64() { return int64_t(randomUInteger64()); }

float Math::randomFloat32()
{
	const uint32_t r = randomUInteger32();
	float fr;
	memcpy(&fr, &r, sizeof(fr));
	return fr;
}

float Math::randomFloat32BetweenZeroAndOne()
{
	const float fr = float(g_oRandomGenerator.rand()) / float(g_oRandomGenerator.l_ui32RandMax);
	return fr;
}

double Math::randomFloat64()
{
	const uint64_t r = randomUInteger64();
	double fr;
	memcpy(&fr, &r, sizeof(fr));
	return fr;
}

bool Math::isfinite(double value)
{
#ifdef TARGET_OS_Windows
	return (_finite(value) != 0 || value == 0.);
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	return std::isfinite(value);
#else
	return std::isfinite(value);
#endif
}

bool Math::isinf(double value)
{
#ifdef TARGET_OS_Windows
	int l_i32Class = _fpclass(value);
	return (l_i32Class == _FPCLASS_NINF || l_i32Class == _FPCLASS_PINF);
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	return std::isinf(value);
#else
	return std::isinf(value);
#endif
}

bool Math::isnan(double value)
{
#ifdef TARGET_OS_Windows
	return (_isnan(value) != 0);
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	return std::isnan(value);
#else
	return std::isnan(value);
#endif
}

bool Math::isnormal(double value)
{
#ifdef TARGET_OS_Windows
	int l_i32Class = _fpclass(value);
	return (l_i32Class == _FPCLASS_NN || l_i32Class == _FPCLASS_PN);
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	return std::isnormal(value);
#else
	return std::isnormal(value);
#endif
}

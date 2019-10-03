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
 * - Due to generative process, values generated above L_RAND_MAX may not be dense (verify) 
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
	static const uint32_t L_RAND_MAX = 0x7FFFFFFF; // (2^32)/2-1 == 2147483647

	explicit RandomGenerator(const uint32_t seed = 1) : m_nextValue(seed) {}

	int rand()
	{
		// Pretty much C99 convention and parameters for a Linear Congruential Generator
		m_nextValue = (m_nextValue * 1103515245 + 12345) & L_RAND_MAX;
		return int(m_nextValue);
	}

	void setSeed(const uint32_t seed) { m_nextValue = seed; }

	uint32_t getSeed() const { return m_nextValue; }
};

// Should be only accessed via Math:: calls defined below
static RandomGenerator randomGenerator;

bool Math::initializeRandomMachine(const uint64_t randomSeed)
{
	randomGenerator.setSeed(uint32_t(randomSeed));

	// For safety, we install also the C++ basic random engine (it might be useg by dependencies, old code, etc)
	srand(uint32_t(randomSeed));

	return true;
}

uint8_t Math::randomUInteger8() { return uint8_t(randomUInteger64()); }

uint16_t Math::randomUInteger16() { return uint16_t(randomUInteger64()); }

uint32_t Math::randomUInteger32() { return uint32_t(randomUInteger64()); }

uint64_t Math::randomUInteger64()
{
	const uint64_t r1 = randomGenerator.rand();
	const uint64_t r2 = randomGenerator.rand();
	const uint64_t r3 = randomGenerator.rand();
	const uint64_t r4 = randomGenerator.rand();
	return (r1 << 24) ^ (r2 << 16) ^ (r3 << 8) ^ (r4);
}

uint32_t Math::randomUInteger32WithCeiling(uint32_t upperLimit)
{
	// float in range [0,1]
	const double temp = randomGenerator.rand() / double(RandomGenerator::L_RAND_MAX);

	// static_cast is effectively floor(), so below we get output range [0,upperLimit-1], without explicit subtraction of 1
	const uint32_t returnValue = uint32_t(upperLimit * temp);

	return returnValue;
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
	const float fr = float(randomGenerator.rand()) / float(RandomGenerator::L_RAND_MAX);
	return fr;
}

double Math::randomFloat64()
{
	const uint64_t r = randomUInteger64();
	double fr;
	memcpy(&fr, &r, sizeof(fr));
	return fr;
}

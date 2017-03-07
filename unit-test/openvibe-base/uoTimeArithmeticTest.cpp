/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

#include "ovtAssert.h"
#include "ovtTestFixtureCommon.h"

#include "openvibe/ovITimeArithmetics.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

int uoTimeArithmeticTest(int argc, char* argv[])
{
	// time values to test in seconds
	std::vector<double> timesToTestInSecond = 
	{ 0, 0.001, 0.01, 0.1, 0.2, 0.25, 0.5, 1.0, 1.1, 1.5, 2, 
	5, 10, 50, 100, 123.456789, 128.0, 500, 1000, 2500, 5000 };

	// time values to test in fixed point format
	std::vector<unsigned long long> timesToTestInFixedPoint =
	{ 1LL << 8, 1LL << 16, 1L << 19, 1LL << 22, 1LL << 27, 1L << 30, 1LL << 32, 10LL << 32, 100LL << 32, 123LL << 32, 500LL << 32, 512LL << 32,
	1000LL << 32, 1024LL << 32, 2001LL << 32, 5000LL << 32 };

	// sampling rates to test
	std::vector<unsigned int> samplingRatesToTest = { 100, 128, 512, 1000, 1024, 16000, 44100 };

	// sample to test
	std::vector<unsigned long long> samplesToTest = { 0, 1, 100, 128, 512, 1000, 1021, 1024, 5005, 12345, 59876, 100000, 717893, 1000001 };

	// epoch duration to test
	std::vector<double> epochDurationsToTest = { 0.01, 0.1, 0.2, 0.25, 0.5, 1.0, 1.1, 1.5, 2, 5, 10, 50, 100 };

	const double timeTolerance = 1LL << ITimeArithmetics::m_ui32DecimalPrecision;

	std::stringstream  errorMessage;

	// test conversion second -> fixed point -> second
	for (auto testTimeInSecond : timesToTestInSecond)
	{
		auto computedTimeInSecond = ITimeArithmetics::timeToSeconds(ITimeArithmetics::secondsToTime(testTimeInSecond));

		OVT_ASSERT(std::abs(computedTimeInSecond - testTimeInSecond) < timeTolerance,
			errorMessage << "Failure to convert [second -> fixed point -> second]: test time = "
			<< testTimeInSecond << " VS computed time = " << computedTimeInSecond << std::endl
			);
	}


	// test conversion fixed point -> second -> fixed point
	for (auto testTimeInFixedPoint : timesToTestInFixedPoint)
	{
		auto computedTimeInFixedPoint = ITimeArithmetics::secondsToTime(ITimeArithmetics::timeToSeconds(testTimeInFixedPoint));

		auto testSignificantBits = (testTimeInFixedPoint >> (32 - ITimeArithmetics::m_ui32DecimalPrecision));
		auto computedSignificantBits = (computedTimeInFixedPoint >> (32 - ITimeArithmetics::m_ui32DecimalPrecision));

		OVT_ASSERT(computedSignificantBits == testSignificantBits, 
			errorMessage << "Failure to convert [fixed point -> second -> fixed point]: test time = "
			<< testTimeInFixedPoint << " VS computed time = " << computedTimeInFixedPoint << std::endl);
	}

	// test conversion time -> sample -> time
	for (auto testSamplingRate : samplingRatesToTest)
	{
		for (auto testTimeInSecond : timesToTestInSecond)
		{
			auto testTimeInFixedPoint = ITimeArithmetics::secondsToTime(testTimeInSecond);
			auto computedTimeInFixedPoint = ITimeArithmetics::sampleCountToTime(
				testSamplingRate, 
				ITimeArithmetics::timeToSampleCount(testSamplingRate, testTimeInFixedPoint)
				);

			unsigned long long timeDifference = std::abs(static_cast<long long>(computedTimeInFixedPoint) - static_cast<long long>(testTimeInFixedPoint));

			OVT_ASSERT(ITimeArithmetics::timeToSeconds(timeDifference) < (1.0 / static_cast<double>(testSamplingRate)),
				errorMessage << "Failure to convert [sample -> time -> sample] at sample rate " << testSamplingRate << 
				": test time = " << testTimeInFixedPoint << " VS computed time = " << computedTimeInFixedPoint << std::endl
				);
		}
	}

	// test conversion sample -> time -> sample
	for (auto testSamplingRate : samplingRatesToTest)
	{
		for (auto testSample : samplesToTest)
		{
			auto computedSampleCount = ITimeArithmetics::timeToSampleCount(
				testSamplingRate,
				ITimeArithmetics::sampleCountToTime(testSamplingRate, testSample)
				);

			OVT_ASSERT(computedSampleCount == testSample,
				errorMessage << "Failure to convert [time -> sample -> time] at sample rate " << testSamplingRate <<
				": test sample count = " << testSample << " VS computed sample count = " << computedSampleCount << std::endl
				);
		}
	}

	// test time -> sample count for 1 second signal duration at given rates
	for (auto testSamplingRate : samplingRatesToTest)
	{
		auto sampleCount = ITimeArithmetics::timeToSampleCount(testSamplingRate, ITimeArithmetics::secondsToTime(1.0));
		
		OVT_ASSERT(sampleCount == testSamplingRate,
			errorMessage << "Failure to retrieve sample count for 1 second signal duration at sample rate " << testSamplingRate <<
			": computed sample count = " << sampleCount << std::endl
			);
	}

	// compare second -> time conversion to legacy method
	for (auto testEpochDuration : epochDurationsToTest)
	{
		auto legacyTime = static_cast<unsigned long long>(testEpochDuration * (1LL << 32)); // Legacy code from stimulationBasedEpoching
		auto computedTimeInFixedPoint = ITimeArithmetics::secondsToTime(testEpochDuration);

		auto legacySignificantBits = (legacyTime >> (32 - ITimeArithmetics::m_ui32DecimalPrecision));
		auto computedSignificantBits = (computedTimeInFixedPoint >> (32 - ITimeArithmetics::m_ui32DecimalPrecision));

		OVT_ASSERT(computedSignificantBits == legacySignificantBits,
			errorMessage << "Failure to convert [second -> time] as with legacy method: test time = "
			<< legacyTime << " VS computed time = " << computedTimeInFixedPoint << std::endl
			);

	}
	
	return EXIT_SUCCESS;
}

//==========================End OF File==============================

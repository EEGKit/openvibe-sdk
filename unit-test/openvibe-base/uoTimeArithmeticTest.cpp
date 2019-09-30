/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
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
#include <gtest/gtest.h>

#include "openvibe/ovTimeArithmetics.h"

using namespace OpenViBE;

namespace
{
	const double HOUR  = 60 * 60;
	const double DAY   = 24 * HOUR;
	const double WEEK  = 7 * DAY;
	const double MONTH = 30 * DAY;
	const double YEAR  = 365 * DAY;

	// time values to test in seconds
	std::vector<double> timesToTestInSecond =
	{
		0, 0.001, 0.01, 0.1, 0.2, 0.25, 0.5, 1.0, 1.1, 1.5, 2,
		1.000001, 1.999, 1.999999,
		5, 10, 50, 100, 123.456789, 128.0, 500, 1000, 2500, 5000,
		DAY, DAY + 0.03, DAY + 0.999, DAY + 1,
		WEEK, WEEK + 0.03, WEEK + 0.999, WEEK + 1,
		MONTH, MONTH + 0.03, MONTH + 0.999, MONTH + 1,
		YEAR, YEAR + 0.03, YEAR + 0.999, YEAR + 1,
	};

	// time values to test in fixed point format
	std::vector<uint64_t> timesToTestInFixedPoint =
	{
		1LL << 8, 1LL << 16, 1L << 19, 1LL << 22, 1LL << 27, 1L << 30, 1LL << 32, 10LL << 32, 100LL << 32, 123LL << 32, 500LL << 32, 512LL << 32, 1000LL << 32, 1024LL << 32, 2001LL << 32, 5000LL << 32
	};

	// sampling rates to test
	std::vector<uint32_t> samplingRatesToTest = { 100, 128, 512, 1000, 1024, 16000, 44100 };

	// sample to test
	std::vector<uint64_t> samplesToTest = { 0, 1, 100, 128, 512, 1000, 1021, 1024, 5005, 12345, 59876, 100000, 717893, 1000001 };

	// epoch duration to test
	std::vector<double> epochDurationsToTest = { 0.01, 0.1, 0.2, 0.25, 0.5, 1.0, 1.1, 1.5, 2, 5, 10, 50, 100 };

	// We require at least a millisecond precision
	const double timeTolerance = 0.001;
}


TEST(time_arithmetic_test_case, seconds_to_fixed_to_seconds)
{
	// test conversion second -> fixed point -> second
	for (auto testTimeInSecond : timesToTestInSecond)
	{
		const auto computedTimeInSecond = TimeArithmetics::timeToSeconds(TimeArithmetics::secondsToTime(testTimeInSecond));
		EXPECT_LT(std::abs(computedTimeInSecond - testTimeInSecond), timeTolerance);
	}
}


TEST(time_arithmetic_test_case, fixed_to_seconds_to_fixed)
{
	// test conversion fixed point -> second -> fixed point
	for (auto testTimeInFixedPoint : timesToTestInFixedPoint)
	{
		const auto computedTimeInFixedPoint = TimeArithmetics::secondsToTime(TimeArithmetics::timeToSeconds(testTimeInFixedPoint));
		EXPECT_EQ(computedTimeInFixedPoint, testTimeInFixedPoint);
	}
}

TEST(time_arithmetic_test_case, time_to_fixed_to_samples_to_fixed)
{
	// test conversion time -> sample -> time
	for (auto testSamplingRate : samplingRatesToTest)
	{
		for (auto testTimeInSecond : timesToTestInSecond)
		{
			auto testTimeInFixedPoint = TimeArithmetics::secondsToTime(testTimeInSecond);
			// If the sample count would overflow an uint64_t we skip the test
			if (std::log2(testSamplingRate) + std::log2(testTimeInFixedPoint) >= 64) { continue; }
			const auto computedTimeInFixedPoint = TimeArithmetics::sampleCountToTime(testSamplingRate, TimeArithmetics::timeToSampleCount(testSamplingRate, testTimeInFixedPoint));

			const uint64_t timeDifference = uint64_t(std::abs(int64_t(computedTimeInFixedPoint) - int64_t(testTimeInFixedPoint)));
			EXPECT_LT(TimeArithmetics::timeToSeconds(timeDifference), (1.0 / double(testSamplingRate)))
			        << "Time difference too large between OV(" << testTimeInSecond << ") and "
			        << "SCtoOV(" << testSamplingRate << ", OVtoSC(" << testSamplingRate << "," << testTimeInFixedPoint << "))";
		}
	}
}

TEST(time_arithmetic_test_case, samples_to_time_to_samples)
{
	// test conversion sample -> time -> sample
	for (auto testSamplingRate : samplingRatesToTest)
	{
		for (auto testSample : samplesToTest)
		{
			const auto computedSampleCount = TimeArithmetics::timeToSampleCount(testSamplingRate, TimeArithmetics::sampleCountToTime(testSamplingRate, testSample));
			EXPECT_EQ(testSample, computedSampleCount);
		}
	}
}


TEST(time_arithmetic_test_case, 1s_samples_to_samplig_rate)
{
	// test time -> sample count for 1 second signal duration at given rates
	for (auto testSamplingRate : samplingRatesToTest)
	{
		const auto nSample = TimeArithmetics::timeToSampleCount(testSamplingRate, TimeArithmetics::secondsToTime(1.0));
		EXPECT_EQ(nSample, testSamplingRate);
	}
}

TEST(time_arithmetic_test_case, legacy_epoching)
{
	// compare second -> time conversion to legacy method
	for (auto testEpochDuration : epochDurationsToTest)
	{
		const auto legacyTime               = static_cast<unsigned long long>(testEpochDuration * (1LL << 32)); // Legacy code from stimulationBasedEpoching
		const auto computedTimeInFixedPoint = TimeArithmetics::secondsToTime(testEpochDuration);

		EXPECT_EQ(computedTimeInFixedPoint, legacyTime);
	}
}

int uoTimeArithmeticTest(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "time_aritmetic_test_case.*";
	return RUN_ALL_TESTS();
}

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
#include <limits>
#include <vector>
#include <cmath>
#include <tuple>
#include <numeric>
#include <algorithm>
#include <chrono>

#include "system/ovCTime.h"
#include "openvibe/ovITimeArithmetics.h"

#include "ovtAssert.h"

using namespace System;

//
// \note This test should be improved. Some improvements could be:
//       - Compare clock results to gold standard
//       - True testing of monotonic state
//       - Stress tests on longer period
//

// \brief Calibrate sleep function to estimate the extra time not spent at sleeping
uint64_t calibrateSleep(unsigned int sampleCount, bool (*sleepFunction)(uint64_t), uint64_t (*timeFunction)())
{
	uint64_t preTime;
	uint64_t processingTime;
	uint64_t maxTime = 0;
	for (size_t i = 0; i < sampleCount; ++i)
	{
		preTime = timeFunction();
		sleepFunction(0);
		processingTime = timeFunction() - preTime;

		if (processingTime > maxTime)
		{
			maxTime = processingTime;
		}
	}

	return maxTime;
}

// \brief Record sleep function precision
std::vector<uint64_t> testSleep(const std::vector<uint64_t>& sleepTimes, bool (*sleepFunction)(uint64_t), uint64_t (*timeFunction)())
{
	std::vector<uint64_t> effectiveSleepTimes;
	uint64_t preTime;
	for (auto time : sleepTimes)
	{
		preTime = timeFunction();
		sleepFunction(time);
		effectiveSleepTimes.push_back(timeFunction() - preTime);
	}

	return effectiveSleepTimes;
}

// \brief Return a warning count that is incremented when sleep function did not meet following requirements:
//       - sleep enough time
//       - sleep less than the expected time + delta
unsigned int assessSleepTestResult(const std::vector<uint64_t>& expectedTimes,
								   const std::vector<uint64_t>& resultTimes,
								   uint64_t delta, uint64_t epsilon)
{
	unsigned int warningCount = 0;
	for (size_t i = 0; i < expectedTimes.size(); ++i)
	{
		if (resultTimes[i] + epsilon < expectedTimes[i]
			|| resultTimes[i] > (expectedTimes[i] + delta + epsilon))
		{
			std::cerr << "WARNING: Failure to sleep the right amount of time: [expected|result] = "
					<< OpenViBE::ITimeArithmetics::timeToSeconds(expectedTimes[i]) << "|"
					<< OpenViBE::ITimeArithmetics::timeToSeconds(resultTimes[i]) << std::endl;
			warningCount++;
		}
	}

	return warningCount;
}

// \brief Record clock function data (spin test taken from OpenViBE). Return a tuple with:
//       - bool = monotonic state
//       - std::vector<uint64_t> = all the cumulative steps
std::tuple<bool, std::vector<uint64_t>> testClock(uint64_t samplePeriod, unsigned sampleCountGuess, uint64_t (*timeFunction)())
{
	std::vector<uint64_t> cumulativeSteps;
	cumulativeSteps.reserve(sampleCountGuess);

	bool monotonic        = true;
	uint64_t startTime    = timeFunction();
	uint64_t nowTime      = startTime;
	uint64_t previousTime = nowTime;

	while (nowTime - startTime < samplePeriod)
	{
		nowTime = timeFunction();
		if (nowTime > previousTime)
		{
			cumulativeSteps.push_back(nowTime - previousTime);
		}
		else if (nowTime < previousTime)
		{
			monotonic = false;
			break;
		}
		previousTime = nowTime;
	}

	return std::make_tuple(monotonic, cumulativeSteps);
}

// \brief Compute jitter measurements for 32:32 time. Return a tuple with:
//       - double = mean
//       - double = max deviation from mean
//       - double = RMSE
std::tuple<double, double, double> assessTimeClock(const std::vector<uint64_t>& measurements)
{
	double jitterMax = 0.0;
	double jitterMSE = 0.0;
	double mean      = 0.0;

	// compute mean
	for (auto& data : measurements)
	{
		// convert data
		auto seconds                         = static_cast<uint32_t>(data >> 32);
		auto microseconds                    = ((data & 0xFFFFFFFFLL) * 1000000LL) >> 32;
		std::chrono::microseconds chronoData = std::chrono::seconds(seconds) + std::chrono::microseconds(microseconds);

		mean += static_cast<double>(chronoData.count()) / (1000 * measurements.size());
	}

	// compute deviations
	for (auto& data : measurements)
	{
		// convert data
		auto seconds                         = static_cast<uint32_t>(data >> 32);
		auto microseconds                    = ((data & 0xFFFFFFFFLL) * 1000000LL) >> 32;
		std::chrono::microseconds chronoData = std::chrono::seconds(seconds) + std::chrono::microseconds(microseconds);

		double deviation = std::abs(static_cast<double>(chronoData.count()) / 1000 - mean);
		jitterMSE += std::pow(deviation, 2) / measurements.size();

		if (deviation - jitterMax > std::numeric_limits<double>::epsilon())
		{
			jitterMax = deviation;
		}
	}

	return std::make_tuple(mean, jitterMax, std::sqrt(jitterMSE));
}

int uoTimeTest(int argc, char* argv[])
{
	// This is a very theoretical test. But if it returns false, we can
	// assume that a steady clock is not available on the test
	// platform. If it returns true, it just means that the internal clock
	// says it is steady...
	OVT_ASSERT(Time::isClockSteady(), "Failure to retrieve a steady clock");

	// Same as above.
	// The test is set to 1ms at it is a requirement for OpenViBE clocks
	OVT_ASSERT(Time::checkResolution(1), "Failure to check for resolution");

	// A stress test to check no overflow happens
	OVT_ASSERT(Time::checkResolution(std::numeric_limits<uint32_t >::max()), "Failure to check for resolution");

	//
	// zSleep() function test
	//

	std::vector<uint64_t> expectedSleepData = {
		0x80000000LL, 0x40000000LL, 0x20000000LL, 0x10000000LL,
		0x80000000LL, 0x40000000LL, 0x20000000LL, 0x10000000LL,
		0x08000000LL, 0x04000000LL, 0x02000000LL, 0x01000000LL,
		0x08000000LL, 0x04000000LL, 0x02000000LL, 0x01000000LL
	};

	// calibrate sleep function
	auto deltaTime = calibrateSleep(1000, Time::zsleep, Time::zgetTime);

	std::cout << "INFO: Delta time for zsleep calibration = " << OpenViBE::ITimeArithmetics::timeToSeconds(deltaTime) << std::endl;

	auto resultSleepData = testSleep(expectedSleepData, Time::zsleep, Time::zgetTime);

	OVT_ASSERT(resultSleepData.size() == expectedSleepData.size(), "Failure to run zsleep tests");

	unsigned int warningCount = assessSleepTestResult(expectedSleepData, resultSleepData, deltaTime,
													  OpenViBE::ITimeArithmetics::secondsToTime(0.005));

	// relax this threshold in case there is some recurrent problems
	// according to the runtime environment
	OVT_ASSERT(warningCount <= 2, "Failure to zsleep the right amount of time");

	//
	// zGetTime() function test
	//

	// the sample count guess was found in an empiric way
	auto resultGetTimeData = testClock(OpenViBE::ITimeArithmetics::secondsToTime(0.5), 500000, Time::zgetTime);

	OVT_ASSERT(std::get<0>(resultGetTimeData), "Failure in zgetTime() test: the clock is not monotonic");

	auto clockMetrics = assessTimeClock(std::get<1>(resultGetTimeData));

	std::cout << "INFO: Sample count for getTime() = " << std::get<1>(resultGetTimeData).size() << std::endl;
	std::cout << "INFO: Mean step in ms for getTime() = " << std::get<0>(clockMetrics) << std::endl;
	std::cout << "INFO: Max deviation in ms for getTime() = " << std::get<1>(clockMetrics) << std::endl;
	std::cout << "INFO: RMSE in ms for getTime() = " << std::get<2>(clockMetrics) << std::endl;

	// We expect at least 1ms resolution
	double resolutionDelta = std::get<0>(clockMetrics) - 1;
	OVT_ASSERT(resolutionDelta <= std::numeric_limits<double>::epsilon(), "Failure in zgetTime() test: the clock resolution does not match requirements");

	return EXIT_SUCCESS;
}

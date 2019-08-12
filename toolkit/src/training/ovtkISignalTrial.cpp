#include "ovtkISignalTrial.h"

#include <system/ovCMemory.h>

using namespace OpenViBEToolkit;
using namespace OpenViBE;

ISignalTrial& OpenViBEToolkit::copyHeader(ISignalTrial& trial, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	if (srcTrial != &trial)
	{
		uint32_t channelCount = srcTrial->getChannelCount();
		trial.setChannelCount(channelCount);
		for (uint32_t i = 0; i < channelCount; i++)
		{
			trial.setChannelName(i, srcTrial->getChannelName(i));
		}
		trial.setSamplingRate(srcTrial->getSamplingRate());
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::copy(ISignalTrial& trial, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	// copyHeader(trial, srcTrial);

	if (srcTrial != &trial)
	{
		uint32_t l_ui32ChannelCount = srcTrial->getChannelCount();
		uint32_t l_ui32SampleCount  = srcTrial->getSampleCount();
		trial.setSampleCount(l_ui32SampleCount, false);
		for (uint32_t i = 0; i < l_ui32ChannelCount; i++)
		{
			System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), l_ui32SampleCount * sizeof(double));
		}
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::selectSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	// copyHeader(trial, srcTrial);

	uint32_t channelCount = srcTrial->getChannelCount();
	uint32_t sampleCount  = sampleEnd - sampleStart;

	trial.setSampleCount(sampleCount, false);
	for (uint32_t i = 0; i < channelCount; i++)
	{
		System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i) + sampleStart, sampleCount * sizeof(double));
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::selectTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	uint32_t sampleStart = uint32_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	uint32_t sampleEnd   = uint32_t((timeEnd * srcTrial->getSamplingRate()) >> 32);
	return selectSamples(trial, sampleStart, sampleEnd, srcTrial);
}

ISignalTrial& OpenViBEToolkit::removeSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; } 	// $$$ NOT YET IMPLEMENTED

	// copyHeader(trial, srcTrial);

	uint32_t srcChannelCount = srcTrial->getChannelCount();
	uint32_t srcSampleCount  = srcTrial->getSampleCount() - (sampleEnd - sampleStart);

	trial.setSampleCount(srcSampleCount, false);
	for (uint32_t i = 0; i < srcChannelCount; i++)
	{
		System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), sampleStart * sizeof(double));
		System::Memory::copy(trial.getChannelSampleBuffer(i) + sampleStart, srcTrial->getChannelSampleBuffer(i) + sampleEnd, (srcSampleCount - sampleStart) * sizeof(double));
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::removeTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	uint32_t sampleStart = uint32_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	uint32_t sampleEnd   = uint32_t((timeEnd * srcTrial->getSamplingRate()) >> 32);
	return removeSamples(trial, sampleStart, sampleEnd, srcTrial);
}

ISignalTrial& OpenViBEToolkit::insertBufferSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleCount, const double* buffer, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	// copyHeader(trial, srcTrial);

	uint32_t srcChannelCount = srcTrial->getChannelCount();
	uint32_t srcSamplecount  = srcTrial->getSampleCount();

	trial.setSampleCount(srcSamplecount + sampleCount, true);
	for (uint32_t i = 0; i < srcChannelCount; i++)
	{
		if (&trial != srcTrial)
		{
			System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), sampleStart * sizeof(double));
		}

		System::Memory::copy(trial.getChannelSampleBuffer(i) + sampleStart + sampleCount, srcTrial->getChannelSampleBuffer(i), (srcSamplecount - sampleStart) * sizeof(double));
		System::Memory::copy(trial.getChannelSampleBuffer(i) + sampleStart, buffer + sampleCount * i, sampleCount * sizeof(double));
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::insertBufferTime(ISignalTrial& trial, const uint64_t timeStart, const uint32_t sampleCount, const double* buffer, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	uint32_t sampleStart = uint32_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	return insertBufferSamples(trial, sampleStart, sampleCount, buffer, srcTrial);
}

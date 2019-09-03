#include "ovtkISignalTrial.h"

#include <system/ovCMemory.h>

using namespace OpenViBEToolkit;
using namespace OpenViBE;

ISignalTrial& OpenViBEToolkit::copyHeader(ISignalTrial& trial, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	if (srcTrial != &trial)
	{
		const uint32_t nChannel = srcTrial->getChannelCount();
		trial.setChannelCount(nChannel);
		for (uint32_t i = 0; i < nChannel; i++) { trial.setChannelName(i, srcTrial->getChannelName(i)); }
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
		const uint32_t nChannel = srcTrial->getChannelCount();
		const uint32_t nSample  = srcTrial->getSampleCount();
		trial.setSampleCount(nSample, false);
		for (uint32_t i = 0; i < nChannel; i++)
		{
			System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), nSample * sizeof(double));
		}
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::selectSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	// copyHeader(trial, srcTrial);

	const uint32_t nChannel = srcTrial->getChannelCount();
	const uint32_t nSample  = sampleEnd - sampleStart;

	trial.setSampleCount(nSample, false);
	for (uint32_t i = 0; i < nChannel; i++)
	{
		System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i) + sampleStart, nSample * sizeof(double));
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::selectTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	const uint32_t sampleStart = uint32_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	const uint32_t sampleEnd   = uint32_t((timeEnd * srcTrial->getSamplingRate()) >> 32);
	return selectSamples(trial, sampleStart, sampleEnd, srcTrial);
}

ISignalTrial& OpenViBEToolkit::removeSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; } 	// $$$ NOT YET IMPLEMENTED

	// copyHeader(trial, srcTrial);

	const uint32_t srcNChannel = srcTrial->getChannelCount();
	const uint32_t srcNSample  = srcTrial->getSampleCount() - (sampleEnd - sampleStart);

	trial.setSampleCount(srcNSample, false);
	for (uint32_t i = 0; i < srcNChannel; i++)
	{
		System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), sampleStart * sizeof(double));
		System::Memory::copy(trial.getChannelSampleBuffer(i) + sampleStart, srcTrial->getChannelSampleBuffer(i) + sampleEnd,
							 (srcNSample - sampleStart) * sizeof(double));
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::removeTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	const uint32_t sampleStart = uint32_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	const uint32_t sampleEnd   = uint32_t((timeEnd * srcTrial->getSamplingRate()) >> 32);
	return removeSamples(trial, sampleStart, sampleEnd, srcTrial);
}

ISignalTrial& OpenViBEToolkit::insertBufferSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleCount, const double* buffer,
												   const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	// copyHeader(trial, srcTrial);

	const uint32_t srcNChannel = srcTrial->getChannelCount();
	const uint32_t srcNSample  = srcTrial->getSampleCount();

	trial.setSampleCount(srcNSample + sampleCount, true);
	for (uint32_t i = 0; i < srcNChannel; i++)
	{
		if (&trial != srcTrial) { System::Memory::copy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), sampleStart * sizeof(double)); }

		System::Memory::copy(trial.getChannelSampleBuffer(i) + sampleStart + sampleCount, srcTrial->getChannelSampleBuffer(i),
							 (srcNSample - sampleStart) * sizeof(double));
		System::Memory::copy(trial.getChannelSampleBuffer(i) + sampleStart, buffer + sampleCount * i, sampleCount * sizeof(double));
	}

	return trial;
}

ISignalTrial& OpenViBEToolkit::insertBufferTime(ISignalTrial& trial, const uint64_t timeStart, const uint32_t sampleCount, const double* buffer,
												const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	const uint32_t sampleStart = uint32_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	return insertBufferSamples(trial, sampleStart, sampleCount, buffer, srcTrial);
}

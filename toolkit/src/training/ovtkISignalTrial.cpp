#include "ovtkISignalTrial.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Toolkit;

ISignalTrial& Toolkit::copyHeader(ISignalTrial& trial, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	if (srcTrial != &trial)
	{
		const size_t nChannel = srcTrial->getChannelCount();
		trial.setChannelCount(nChannel);
		for (size_t i = 0; i < nChannel; ++i) { trial.setChannelName(i, srcTrial->getChannelName(i)); }
		trial.setSamplingRate(srcTrial->getSamplingRate());
	}

	return trial;
}

ISignalTrial& Toolkit::copy(ISignalTrial& trial, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	// copyHeader(trial, srcTrial);

	if (srcTrial != &trial)
	{
		const size_t nChannel = srcTrial->getChannelCount();
		const size_t nSample  = srcTrial->getSampleCount();
		trial.setSampleCount(nSample, false);
		for (size_t i = 0; i < nChannel; ++i) { memcpy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), nSample * sizeof(double)); }
	}

	return trial;
}

ISignalTrial& Toolkit::selectSamples(ISignalTrial& trial, const size_t sampleStart, const size_t sampleEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	// copyHeader(trial, srcTrial);

	const size_t nChannel = srcTrial->getChannelCount();
	const size_t nSample  = sampleEnd - sampleStart;

	trial.setSampleCount(nSample, false);
	for (size_t i = 0; i < nChannel; ++i)
	{
		memcpy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i) + sampleStart, nSample * sizeof(double));
	}

	return trial;
}

ISignalTrial& Toolkit::selectTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	const size_t sampleStart = size_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	const size_t sampleEnd   = size_t((timeEnd * srcTrial->getSamplingRate()) >> 32);
	return selectSamples(trial, sampleStart, sampleEnd, srcTrial);
}

ISignalTrial& Toolkit::removeSamples(ISignalTrial& trial, const size_t sampleStart, const size_t sampleEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; } 	// $$$ NOT YET IMPLEMENTED

	// copyHeader(trial, srcTrial);

	const size_t srcNChannel = srcTrial->getChannelCount();
	const size_t srcNSample  = srcTrial->getSampleCount() - (sampleEnd - sampleStart);

	trial.setSampleCount(srcNSample, false);
	for (size_t i = 0; i < srcNChannel; ++i)
	{
		memcpy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), sampleStart * sizeof(double));
		memcpy(trial.getChannelSampleBuffer(i) + sampleStart, srcTrial->getChannelSampleBuffer(i) + sampleEnd, (srcNSample - sampleStart) * sizeof(double));
	}

	return trial;
}

ISignalTrial& Toolkit::removeTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }
	if (srcTrial == &trial) { return trial; }	// $$$ NOT YET IMPLEMENTED

	const size_t sampleStart = size_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	const size_t sampleEnd   = size_t((timeEnd * srcTrial->getSamplingRate()) >> 32);
	return removeSamples(trial, sampleStart, sampleEnd, srcTrial);
}

ISignalTrial& Toolkit::insertBufferSamples(ISignalTrial& trial, const size_t sampleStart, const size_t nSample, const double* buffer,
										   const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	// copyHeader(trial, srcTrial);

	const size_t srcNChannel = srcTrial->getChannelCount();
	const size_t srcNSample  = srcTrial->getSampleCount();

	trial.setSampleCount(srcNSample + nSample, true);
	for (size_t i = 0; i < srcNChannel; ++i)
	{
		if (&trial != srcTrial) { memcpy(trial.getChannelSampleBuffer(i), srcTrial->getChannelSampleBuffer(i), sampleStart * sizeof(double)); }

		memcpy(trial.getChannelSampleBuffer(i) + sampleStart + nSample, srcTrial->getChannelSampleBuffer(i), (srcNSample - sampleStart) * sizeof(double));
		memcpy(trial.getChannelSampleBuffer(i) + sampleStart, buffer + nSample * i, nSample * sizeof(double));
	}

	return trial;
}

ISignalTrial& Toolkit::insertBufferTime(ISignalTrial& trial, const uint64_t timeStart, const size_t nSample, const double* buffer,
										const ISignalTrial* srcTrial)
{
	if (srcTrial == nullptr) { srcTrial = &trial; }

	const size_t sampleStart = size_t((timeStart * srcTrial->getSamplingRate()) >> 32);
	return insertBufferSamples(trial, sampleStart, nSample, buffer, srcTrial);
}

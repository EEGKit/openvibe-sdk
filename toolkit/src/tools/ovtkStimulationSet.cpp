#include "ovtkStimulationSet.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Toolkit;

bool StimulationSet::shift(IStimulationSet& stimSet, const uint64_t timeShift)
{
	const size_t count = stimSet.getStimulationCount();
	for (size_t i = 0; i < count; ++i) { stimSet.setStimulationDate(i, stimSet.getStimulationDate(i) + timeShift); }
	return true;
}

bool StimulationSet::copy(IStimulationSet& dst, const IStimulationSet& src, const uint64_t timeShift)
{
	dst.clear();
	return append(dst, src, timeShift);
}

bool StimulationSet::append(IStimulationSet& dst, const IStimulationSet& src, const uint64_t timeShift)
{
	const size_t count = src.getStimulationCount();
	for (size_t i = 0; i < count; ++i)
	{
		dst.appendStimulation(src.getStimulationIdentifier(i), src.getStimulationDate(i) + timeShift, src.getStimulationDuration(i));
	}
	return true;
}

bool StimulationSet::appendRange(IStimulationSet& dst, const IStimulationSet& src, const uint64_t srcStartTime, const uint64_t srcEndTime,
								 const uint64_t timeShift)
{
	const size_t count = src.getStimulationCount();
	for (size_t i = 0; i < count; ++i)
	{
		const uint64_t date = src.getStimulationDate(i);
		if (srcStartTime <= date && date < srcEndTime)
		{
			dst.appendStimulation(src.getStimulationIdentifier(i), src.getStimulationDate(i) + timeShift, src.getStimulationDuration(i));
		}
	}
	return true;
}

bool StimulationSet::removeRange(IStimulationSet& stimSet, const uint64_t startTime, const uint64_t endTime)
{
	for (size_t i = 0; i < stimSet.getStimulationCount(); ++i)
	{
		const uint64_t date = stimSet.getStimulationDate(i);
		if (startTime <= date && date < endTime) { stimSet.removeStimulation(i--); }
	}
	return true;
}

#include "ovtkStimulationSet.h"

using namespace OpenViBE;

bool OpenViBE::Toolkit::Tools::StimulationSet::shift(IStimulationSet& stimSet, const uint64_t timeShift)
{
	const size_t count = stimSet.getStimulationCount();
	for (size_t i = 0; i < count; ++i) { stimSet.setStimulationDate(i, stimSet.getStimulationDate(i) + timeShift); }
	return true;
}

bool OpenViBE::Toolkit::Tools::StimulationSet::copy(IStimulationSet& dstStimSet, const IStimulationSet& srcStimSet, const uint64_t timeShift)
{
	dstStimSet.clear();
	return append(dstStimSet, srcStimSet, timeShift);
}

bool OpenViBE::Toolkit::Tools::StimulationSet::append(IStimulationSet& dstStimSet, const IStimulationSet& srcStimSet, const uint64_t timeShift)
{
	const size_t count = srcStimSet.getStimulationCount();
	for (size_t i = 0; i < count; ++i)
	{
		dstStimSet.appendStimulation(srcStimSet.getStimulationIdentifier(i), srcStimSet.getStimulationDate(i) + timeShift,
									 srcStimSet.getStimulationDuration(i));
	}
	return true;
}

bool OpenViBE::Toolkit::Tools::StimulationSet::appendRange(IStimulationSet& dstStimSet, const IStimulationSet& srcStimSet,
														 const uint64_t srcStartTime, const uint64_t srcEndTime, const uint64_t timeShift)
{
	const size_t count = srcStimSet.getStimulationCount();
	for (size_t i = 0; i < count; ++i)
	{
		const uint64_t date = srcStimSet.getStimulationDate(i);
		if (srcStartTime <= date && date < srcEndTime)
		{
			dstStimSet.appendStimulation(srcStimSet.getStimulationIdentifier(i), srcStimSet.getStimulationDate(i) + timeShift,
										 srcStimSet.getStimulationDuration(i));
		}
	}
	return true;
}

bool OpenViBE::Toolkit::Tools::StimulationSet::removeRange(IStimulationSet& stimSet, const uint64_t startTime, const uint64_t endTime)
{
	for (size_t i = 0; i < stimSet.getStimulationCount(); ++i)
	{
		const uint64_t date = stimSet.getStimulationDate(i);
		if (startTime <= date && date < endTime) { stimSet.removeStimulation(i--); }
	}
	return true;
}

#include "ovtkStimulationSet.h"

using namespace OpenViBE;

bool OpenViBEToolkit::Tools::StimulationSet::shift(IStimulationSet& rStimulationSet, const uint64_t ui64TimeShift)
{
	const size_t count = rStimulationSet.getStimulationCount();
	for (uint64_t i = 0; i < count; i++) { rStimulationSet.setStimulationDate(i, rStimulationSet.getStimulationDate(i) + ui64TimeShift); }
	return true;
}

bool OpenViBEToolkit::Tools::StimulationSet::copy(IStimulationSet& rDestinationStimulationSet, const IStimulationSet& rSourceStimulationSet,
												  const uint64_t ui64TimeShift)
{
	rDestinationStimulationSet.clear();
	return append(rDestinationStimulationSet, rSourceStimulationSet, ui64TimeShift);
}

bool OpenViBEToolkit::Tools::StimulationSet::append(IStimulationSet& rDestinationStimulationSet, const IStimulationSet& rSourceStimulationSet,
													const uint64_t ui64TimeShift)
{
	const size_t count = rSourceStimulationSet.getStimulationCount();
	for (uint64_t i = 0; i < count; i++)
	{
		rDestinationStimulationSet.appendStimulation(rSourceStimulationSet.getStimulationIdentifier(i),
													 rSourceStimulationSet.getStimulationDate(i) + ui64TimeShift,
													 rSourceStimulationSet.getStimulationDuration(i));
	}
	return true;
}

bool OpenViBEToolkit::Tools::StimulationSet::appendRange(IStimulationSet& rDestinationStimulationSet, const IStimulationSet& rSourceStimulationSet,
														 const uint64_t ui64SourceStartTime, const uint64_t ui64SourceEndTime, const uint64_t ui64TimeShift)
{
	const size_t count = rSourceStimulationSet.getStimulationCount();
	for (uint64_t i = 0; i < count; i++)
	{
		const uint64_t date = rSourceStimulationSet.getStimulationDate(i);
		if (ui64SourceStartTime <= date && date < ui64SourceEndTime)
		{
			rDestinationStimulationSet.appendStimulation(rSourceStimulationSet.getStimulationIdentifier(i),
														 rSourceStimulationSet.getStimulationDate(i) + ui64TimeShift,
														 rSourceStimulationSet.getStimulationDuration(i));
		}
	}
	return true;
}

bool OpenViBEToolkit::Tools::StimulationSet::removeRange(IStimulationSet& rStimulationSet, const uint64_t ui64StartTime, const uint64_t ui64EndTime)
{
	for (size_t i = 0; i < rStimulationSet.getStimulationCount(); i++)
	{
		const uint64_t date = rStimulationSet.getStimulationDate(i);
		if (ui64StartTime <= date && date < ui64EndTime) { rStimulationSet.removeStimulation(i--); }
	}
	return true;
}

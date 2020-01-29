#pragma once

#include "../ovtk_base.h"

namespace OpenViBE
{
	namespace Toolkit
	{
	namespace Tools
	{
		namespace StimulationSet
		{
			OVTK_API bool shift(IStimulationSet& stimSet, uint64_t timeShift);
			OVTK_API bool copy(IStimulationSet& dstStimSet, const IStimulationSet& srcStimSet, uint64_t timeShift = 0);
			OVTK_API bool append(IStimulationSet& dstStimSet, const IStimulationSet& srcStimSet, uint64_t timeShift = 0);
			OVTK_API bool appendRange(IStimulationSet& dstStimSet, const IStimulationSet& srcStimSet, uint64_t srcStartTime,
									  uint64_t srcEndTime, uint64_t timeShift = 0);
			OVTK_API bool removeRange(IStimulationSet& stimSet, uint64_t startTime, uint64_t endTime);
		} // namespace StimulationSet
	} // namespace Tools
	}  // namespace Toolkit
}  // namespace OpenViBE

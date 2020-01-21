#pragma once

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace StimulationSet
		{
			OVTK_API bool shift(OpenViBE::IStimulationSet& stimSet, uint64_t timeShift);
			OVTK_API bool copy(OpenViBE::IStimulationSet& dstStimSet, const OpenViBE::IStimulationSet& srcStimSet, uint64_t timeShift = 0);
			OVTK_API bool append(OpenViBE::IStimulationSet& dstStimSet, const OpenViBE::IStimulationSet& srcStimSet, uint64_t timeShift = 0);
			OVTK_API bool appendRange(OpenViBE::IStimulationSet& dstStimSet, const OpenViBE::IStimulationSet& srcStimSet, uint64_t srcStartTime,
									  uint64_t srcEndTime, uint64_t timeShift = 0);
			OVTK_API bool removeRange(OpenViBE::IStimulationSet& stimSet, uint64_t startTime, uint64_t endTime);
		} // namespace StimulationSet
	} // namespace Tools
} // namespace OpenViBEToolkit

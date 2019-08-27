#pragma once

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace StimulationSet
		{
			OVTK_API bool shift(OpenViBE::IStimulationSet& rStimulationSet, uint64_t ui64TimeShift);
			OVTK_API bool copy(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, uint64_t ui64TimeShift = 0);
			OVTK_API bool append(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, uint64_t ui64TimeShift = 0);
			OVTK_API bool appendRange(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, uint64_t ui64SourceStartTime, uint64_t ui64SourceEndTime, uint64_t ui64TimeShift = 0);
			OVTK_API bool removeRange(OpenViBE::IStimulationSet& rStimulationSet, uint64_t ui64StartTime, uint64_t ui64EndTime);
		} // namespace StimulationSet
	} // namespace Tools
} // namespace OpenViBEToolkit

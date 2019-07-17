#ifndef __OpenViBEToolkit_Tools_StimulationSet_H__
#define __OpenViBEToolkit_Tools_StimulationSet_H__

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace StimulationSet
		{
			OVTK_API bool shift(OpenViBE::IStimulationSet& rStimulationSet, const OpenViBE::uint64 ui64TimeShift);
			OVTK_API bool copy(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, const OpenViBE::uint64 ui64TimeShift = 0);
			OVTK_API bool append(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, const OpenViBE::uint64 ui64TimeShift = 0);
			OVTK_API bool appendRange(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, const OpenViBE::uint64 ui64SourceStartTime, const OpenViBE::uint64 ui64SourceEndTime, const OpenViBE::uint64 ui64TimeShift = 0);
			OVTK_API bool removeRange(OpenViBE::IStimulationSet& rStimulationSet, const OpenViBE::uint64 ui64StartTime, const OpenViBE::uint64 ui64EndTime);
		};
	};
};

#endif // __OpenViBEToolkit_Tools_StimulationSet_H__

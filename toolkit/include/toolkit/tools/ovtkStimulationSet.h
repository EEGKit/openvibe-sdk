#ifndef __OpenViBEToolkit_Tools_StimulationSet_H__
#define __OpenViBEToolkit_Tools_StimulationSet_H__

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace StimulationSet
		{
			OVTK_API bool shift(OpenViBE::IStimulationSet& rStimulationSet, const uint64_t ui64TimeShift);
			OVTK_API bool copy(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, const uint64_t ui64TimeShift = 0);
			OVTK_API bool append(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, const uint64_t ui64TimeShift = 0);
			OVTK_API bool appendRange(OpenViBE::IStimulationSet& rDestinationStimulationSet, const OpenViBE::IStimulationSet& rSourceStimulationSet, const uint64_t ui64SourceStartTime, const uint64_t ui64SourceEndTime, const uint64_t ui64TimeShift = 0);
			OVTK_API bool removeRange(OpenViBE::IStimulationSet& rStimulationSet, const uint64_t ui64StartTime, const uint64_t ui64EndTime);
		};
	};
};

#endif // __OpenViBEToolkit_Tools_StimulationSet_H__

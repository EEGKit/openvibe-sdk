#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	class IStimulationSet;

	namespace Kernel
	{

		/**
		  * \class IServerExtensionDynamicContext
		  * \author Jozef Legeny (Inria)
		  * \date 2013-06-12
		  * \brief Dynamic context of a Server Extension object
		  * \ingroup Group_Server
		  * \ingroup Group_Kernel
		  * \ingroup Group_Extend
		  *
		  * This class contains accessors to data structures made available by
		  * the driver running in the Acquisition Server during the acquisition.
		  *
		  * \sa OpenViBE::Plugins::IServerExtension
		  */

		class OV_API IServerExtensionDynamicContext : public IKernelObject
		{
		public:
			virtual uint32_t getChannelCount() = 0;

			virtual uint32_t getSampleCountPerSentBlock() = 0;

			virtual double* getBuffer() = 0;

			virtual uint64_t getStimulationSetStartTime() = 0;

			virtual uint64_t getStimulationSetEndTime() = 0;

			virtual IStimulationSet& getStimulationSet() = 0;
		};
	}
}



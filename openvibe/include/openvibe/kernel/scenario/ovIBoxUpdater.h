#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IBoxUpdater
		 * \author Cedric RIOU
		 * \date 2018-03-16
		 * \brief 
		 *
		 * 
		 *
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 */
		class OV_API IBoxUpdater : public OpenViBE::Kernel::IKernelObject
		{
		public:
			
			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_BoxUpdater)
		};

	}
}


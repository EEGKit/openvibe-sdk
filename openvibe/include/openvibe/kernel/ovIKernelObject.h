#pragma once

#include "../ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IKernelObject
		 * \brief Base class for all kernel objects
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Kernel
		 */
		class OV_API IKernelObject : public IObject
		{
		public:

			_IsDerivedFromClass_(IObject, OV_ClassId_Kernel_KernelObject)
		};
	} // namespace Kernel
} // namespace OpenViBE

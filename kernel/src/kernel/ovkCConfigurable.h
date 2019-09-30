#pragma once

#include "ovkTKernelObject.h"
#include "ovkTConfigurable.h"
#include "ovkCParameter.h"

namespace OpenViBE
{
	namespace Kernel
	{
		typedef TBaseConfigurable<TKernelObject<IConfigurable>> configurable;

		class CConfigurable final : public configurable
		{
		public:

			explicit CConfigurable(const IKernelContext& ctx)
				: TBaseConfigurable<TKernelObject<IConfigurable>>(ctx) { }

			_IsDerivedFromClass_Final_(configurable, OVK_ClassId_Kernel_Configurable)
		};
	} // namespace Kernel
} // namespace OpenViBE

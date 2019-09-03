#pragma once

#include "ovkTKernelObject.h"
#include "ovkTConfigurable.h"
#include "ovkCParameter.h"

namespace OpenViBE
{
	namespace Kernel
	{
		typedef TBaseConfigurable<TKernelObject<IConfigurable>> _Base__CConfigurable_;

		class CConfigurable final : public _Base__CConfigurable_
		{
		public:

			explicit CConfigurable(const IKernelContext& ctx)
				: TBaseConfigurable<TKernelObject<IConfigurable>>(ctx) { }

			_IsDerivedFromClass_Final_(_Base__CConfigurable_, OVK_ClassId_Kernel_Configurable)
		};
	} // namespace Kernel
} // namespace OpenViBE

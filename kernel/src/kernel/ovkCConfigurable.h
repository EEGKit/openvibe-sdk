#ifndef __OpenViBEKernel_Kernel_CConfigurable_H__
#define __OpenViBEKernel_Kernel_CConfigurable_H__

#include "ovkTKernelObject.h"
#include "ovkTConfigurable.h"
#include "ovkCParameter.h"

namespace OpenViBE
{
	namespace Kernel
	{
		typedef TBaseConfigurable<TKernelObject<IConfigurable>> _Base__CConfigurable_;

		class CConfigurable : public _Base__CConfigurable_
		{
		public:

			explicit CConfigurable(const IKernelContext& rKernelContext)
				: TBaseConfigurable<TKernelObject<IConfigurable>>(rKernelContext) { }

			_IsDerivedFromClass_Final_(_Base__CConfigurable_, OVK_ClassId_Kernel_Configurable);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CConfigurable_H__

#ifndef __OpenViBEKernel_Kernel_Player_CMessageSignal_H__
#define __OpenViBEKernel_Kernel_Player_CMessageSignal_H__

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CMessageSignal : public TMessage<TKernelObject<IMessageSignal>>
		{
		public:

			explicit CMessageSignal(const IKernelContext& rKernelContext)
				: TMessage<TKernelObject<IMessageSignal>>(rKernelContext) { }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageSignal> >, OVK_ClassId_Kernel_Player_MessageSignal);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CMessageSignal_H__

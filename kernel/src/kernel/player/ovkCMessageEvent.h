#pragma once

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CMessageEvent : public TMessage<TKernelObject<IMessageEvent>>
		{
		public:

			explicit CMessageEvent(const IKernelContext& rKernelContext)
				: TMessage<TKernelObject<IMessageEvent>>(rKernelContext) { }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageEvent> >, OVK_ClassId_Kernel_Player_MessageEvent)
		};
	} // namespace Kernel
} // namespace OpenViBE

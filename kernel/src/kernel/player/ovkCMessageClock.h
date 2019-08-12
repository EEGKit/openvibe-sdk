#pragma once

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CMessageClock : public TMessage<TKernelObject<IMessageClock>>
		{
		public:

			explicit CMessageClock(const IKernelContext& rKernelContext)
				: TMessage<TKernelObject<IMessageClock>>(rKernelContext) { }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TMessage<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMessageClock> >, OVK_ClassId_Kernel_Player_MessageClock)
		};
	}  // namespace Kernel
}  // namespace OpenViBE

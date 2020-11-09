#pragma once

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE {
namespace Kernel {
class CMessageClock final : public TMessage<TKernelObject<IMessageClock>>
{
public:

	explicit CMessageClock(const IKernelContext& ctx)
		: TMessage<TKernelObject<IMessageClock>>(ctx) { }

	_IsDerivedFromClass_Final_(TMessage<TKernelObject<IMessageClock>>, OVK_ClassId_Kernel_Player_MessageClock)
};
}  // namespace Kernel
}  // namespace OpenViBE

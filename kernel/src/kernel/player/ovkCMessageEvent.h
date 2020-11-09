#pragma once

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE {
namespace Kernel {
class CMessageEvent final : public TMessage<TKernelObject<IMessageEvent>>
{
public:

	explicit CMessageEvent(const IKernelContext& ctx)
		: TMessage<TKernelObject<IMessageEvent>>(ctx) { }

	_IsDerivedFromClass_Final_(TMessage<TKernelObject<IMessageEvent>>, OVK_ClassId_Kernel_Player_MessageEvent)
};
}  // namespace Kernel
}  // namespace OpenViBE

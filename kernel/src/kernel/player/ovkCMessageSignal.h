#pragma once

#include "../ovkTKernelObject.h"
#include "ovkTMessage.h"

#include <openvibe/ov_all.h>

namespace OpenViBE {
namespace Kernel {
class CMessageSignal final : public TMessage<TKernelObject<IMessageSignal>>
{
public:

	explicit CMessageSignal(const IKernelContext& ctx)
		: TMessage<TKernelObject<IMessageSignal>>(ctx) { }

	_IsDerivedFromClass_Final_(TMessage<TKernelObject<IMessageSignal>>, OVK_ClassId_Kernel_Player_MessageSignal)
};
}  // namespace Kernel
}  // namespace OpenViBE

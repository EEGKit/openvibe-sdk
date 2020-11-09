#pragma once

#include <openvibe/ov_all.h>

namespace OpenViBE {
namespace Kernel {
template <class T>
class TMessage : public T
{
public:

	explicit TMessage(const IKernelContext& ctx);

	CIdentifier getIdentifier() const override;
	uint64_t getTime() const override;

	bool setIdentifier(const CIdentifier& id) override;
	bool setTime(const uint64_t time) override;

	_IsDerivedFromClass_Final_(T, OVK_ClassId_Kernel_Player_MessageT)

protected:

	CIdentifier m_id = OV_UndefinedIdentifier;
	uint64_t m_time  = 0;
};
}  // namespace Kernel
}  // namespace OpenViBE

#include "ovkTMessage.inl"

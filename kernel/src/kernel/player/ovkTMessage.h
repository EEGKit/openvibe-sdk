#pragma once

#include "../../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		template <class T>
		class TMessage : public T
		{
		public:

			explicit TMessage(const IKernelContext& ctx);

			virtual CIdentifier getIdentifier() const;
			virtual uint64_t getTime() const;

			virtual bool setIdentifier(const CIdentifier& identifier);
			virtual bool setTime(uint64_t ui64Time);

			_IsDerivedFromClass_Final_(T, OVK_ClassId_Kernel_Player_MessageT)

		protected:

			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			uint64_t m_ui64Time       = 0;
		};
	} // namespace Kernel
} // namespace OpenViBE

#include "ovkTMessage.inl"

#ifndef __OpenViBEKernel_Kernel_Player_TMessage_H__
#define __OpenViBEKernel_Kernel_Player_TMessage_H__

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

			explicit TMessage(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::CIdentifier getIdentifier(void) const;
			virtual uint64_t getTime(void) const;

			virtual bool setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier);
			virtual bool setTime(
				const uint64_t ui64Time);

			_IsDerivedFromClass_Final_(T, OVK_ClassId_Kernel_Player_MessageT);

		protected:

			OpenViBE::CIdentifier m_oIdentifier;
			uint64_t m_ui64Time;
		};
	};
};

#include "ovkTMessage.inl"

#endif // __OpenViBEKernel_Kernel_Player_TMessage_H__

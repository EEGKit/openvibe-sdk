#pragma once

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Tools
	{
		class CScopeTester final : public IObject
		{
		public:

			CScopeTester(const Kernel::IKernelContext& ctx, const CString& sPrefix);
			~CScopeTester() override;

			_IsDerivedFromClass_Final_(IObject, OVK_ClassId_Tools_ScopeTester)

		protected:

			CString m_sPrefix;
			const Kernel::IKernelContext& m_kernelCtx;
		};
	} // namespace Tools
} // namespace OpenViBE

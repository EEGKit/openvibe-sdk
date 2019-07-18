#ifndef __OpenViBEKernel_Tools_CScopeTester_H__
#define __OpenViBEKernel_Tools_CScopeTester_H__

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Tools
	{
		class CScopeTester : public IObject
		{
		public:

			CScopeTester(const Kernel::IKernelContext& rKernelContext, const CString& sPrefix);
			virtual ~CScopeTester(void);

			_IsDerivedFromClass_Final_(IObject, OVK_ClassId_Tools_ScopeTester);

		protected:

			CString m_sPrefix;
			const Kernel::IKernelContext& m_kernelContext;
		};
	};
};

#endif // __OpenViBEKernel_Tools_CScopeTester_H__

#include "kernel/ovkCKernelContext.h"

#include <openvibe/ov_all.h>

using namespace OpenViBE;
using namespace Kernel;

namespace OpenViBE
{
	namespace Kernel
	{
		class CKernelDesc : public IKernelDesc
		{
		public:

			virtual IKernelContext* createKernel(const CString& rApplicationName, const CString& rConfigurationFilename)
			{
				return new CKernelContext(nullptr, rApplicationName, rConfigurationFilename);
			}

			virtual IKernelContext* createKernel(const IKernelContext& rMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFilename)
			{
				return new CKernelContext(&rMasterKernelContext, rApplicationName, rConfigurationFilename);
			}

			virtual void releaseKernel(IKernelContext* pKernelContext) { delete pKernelContext; }

			virtual CString getName() const { return CString("OpenViBE Kernel Implementation"); }
			virtual CString getAuthorName() const { return CString("Yann Renard"); }
			virtual CString getAuthorCompanyName() const { return CString("INRIA/IRISA"); }
			virtual CString getShortDescription() const { return CString("OpenViBE Kernel Implementation"); }
			virtual CString getDetailedDescription() const { return CString("OpenViBE Kernel Implementation"); }
			virtual CString getVersion() const { return CString("0.5"); }

			_IsDerivedFromClass_Final_(IKernelDesc, OVK_ClassId_KernelDesc)
		};
	}  // namespace Kernel
}  // namespace OpenViBE

static CKernelDesc gst_oKernelDesc;

#include <system/ovCTime.h>

extern "C" {

OVK_API bool onInitialize() { return true; }

OVK_API bool onGetKernelDesc(IKernelDesc*& rpKernelDesc)
{
	rpKernelDesc = &gst_oKernelDesc;
	return true;
}

OVK_API bool onUninitialize() { return true; }

};

#include "kernel/ovkCKernelContext.h"

#include <openvibe/ov_all.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

namespace OpenViBE {
namespace Kernel {
class CKernelDesc final : public IKernelDesc
{
public:
	IKernelContext* createKernel(const CString& rApplicationName, const CString& rConfigurationFilename) override
	{
		return new CKernelContext(nullptr, rApplicationName, rConfigurationFilename);
	}

	IKernelContext* createKernel(const IKernelContext& masterKernelCtx, const CString& applicationName, const CString& configFilename)
	override { return new CKernelContext(&masterKernelCtx, applicationName, configFilename); }

	void releaseKernel(IKernelContext* pKernelContext) override { delete pKernelContext; }
	CString getName() const override { return CString("OpenViBE Kernel Implementation"); }
	CString getAuthorName() const override { return CString("Yann Renard"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString("OpenViBE Kernel Implementation"); }
	CString getDetailedDescription() const override { return CString("OpenViBE Kernel Implementation"); }
	CString getVersion() const override { return CString("0.5"); }

	_IsDerivedFromClass_Final_(IKernelDesc, OVK_ClassId_KernelDesc)
};

}  // namespace Kernel
}  // namespace OpenViBE

static CKernelDesc gKernelDesc;

#include <system/ovCTime.h>

extern "C" {

OVK_API bool onInitialize() { return true; }

OVK_API bool onGetKernelDesc(IKernelDesc*& rpKernelDesc)
{
	rpKernelDesc = &gKernelDesc;
	return true;
}

OVK_API bool onUninitialize() { return true; }

}

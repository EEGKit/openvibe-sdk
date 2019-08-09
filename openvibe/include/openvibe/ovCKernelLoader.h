#pragma once

#include "ovIKernelLoader.h"

namespace OpenViBE
{
	class OV_API CKernelLoader : public IKernelLoader
	{
	public:

		CKernelLoader();
		~CKernelLoader() override;
		bool load(const CString& sFileName, CString* pError = nullptr) override;
		bool unload(CString* pError = nullptr) override;
		bool initialize() override;
		bool getKernelDesc(Kernel::IKernelDesc*& rpKernelDesc) override;
		bool uninitialize() override;

		_IsDerivedFromClass_Final_(OpenViBE::IKernelLoader, OV_ClassId_KernelLoaderBridge)

	private:

		IKernelLoader* m_pKernelLoaderImpl; //!< Internal implementation
	};
}  // namespace OpenViBE

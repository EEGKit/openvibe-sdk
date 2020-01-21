#pragma once

#include "ovIKernelLoader.h"

namespace OpenViBE
{
	class OV_API CKernelLoader final : public IKernelLoader
	{
	public:

		CKernelLoader();
		~CKernelLoader() override;
		bool load(const CString& filename, CString* error = nullptr) override;
		bool unload(CString* error = nullptr) override;
		bool initialize() override;
		bool getKernelDesc(Kernel::IKernelDesc*& desc) override;
		bool uninitialize() override;

		_IsDerivedFromClass_Final_(OpenViBE::IKernelLoader, OV_ClassId_KernelLoaderBridge)

	private:

		IKernelLoader* m_impl = nullptr; //!< Internal implementation
	};
} // namespace OpenViBE

#pragma once

#include "ovIKernelLoader.h"

namespace OpenViBE
{
	class OV_API CKernelLoader : public IKernelLoader
	{
	public:

		CKernelLoader(void);
		virtual ~CKernelLoader(void);

		virtual bool load(const CString& sFileName, CString* pError = NULL);
		virtual bool unload(CString* pError = NULL);

		virtual bool initialize(void);
		virtual bool getKernelDesc(Kernel::IKernelDesc*& rpKernelDesc);
		virtual bool uninitialize(void);

		_IsDerivedFromClass_Final_(OpenViBE::IKernelLoader, OV_ClassId_KernelLoaderBridge)

	private:

		IKernelLoader* m_pKernelLoaderImpl; //!< Internal implementation
	};
};



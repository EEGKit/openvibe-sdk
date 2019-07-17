#ifndef __OpenViBE_CKernelLoader_H__
#define __OpenViBE_CKernelLoader_H__

#include "ovIKernelLoader.h"

namespace OpenViBE
{
	class OV_API CKernelLoader : public OpenViBE::IKernelLoader
	{
	public:

		CKernelLoader(void);
		virtual ~CKernelLoader(void);

		virtual bool load(const OpenViBE::CString& sFileName, OpenViBE::CString* pError = NULL);
		virtual bool unload(OpenViBE::CString* pError = NULL);

		virtual bool initialize(void);
		virtual bool getKernelDesc(OpenViBE::Kernel::IKernelDesc*& rpKernelDesc);
		virtual bool uninitialize(void);

		_IsDerivedFromClass_Final_(OpenViBE::IKernelLoader, OV_ClassId_KernelLoaderBridge)

	private:

		OpenViBE::IKernelLoader* m_pKernelLoaderImpl; //!< Internal implementation
	};
};

#endif // __OpenViBE_CKernelLoader_H__

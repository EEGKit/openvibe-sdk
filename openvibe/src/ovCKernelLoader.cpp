#include "ovCKernelLoader.h"

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <dlfcn.h>
#elif defined TARGET_OS_Windows
#include <windows.h>
#else
#endif

#include <iostream>
#include <string>

using namespace OpenViBE;
using namespace Kernel;

//___________________________________________________________________//
//                                                                   //

namespace OpenViBE
{
	class CKernelLoaderBase : public IKernelLoader
	{
	public:

		CKernelLoaderBase();

		virtual bool initialize();
		virtual bool getKernelDesc(IKernelDesc*& rpKernelDesc);
		virtual bool uninitialize();
		virtual void release();

		_IsDerivedFromClass_Final_(IKernelLoader, OV_UndefinedIdentifier)

		virtual bool isOpen() = 0;

	protected:

		CString m_sFileName;
		bool (*onInitializeCB)();
		bool (*onGetKernelDescCB)(IKernelDesc*&);
		bool (*onUninitializeCB)();
	};
}  // namespace OpenViBE

//___________________________________________________________________//
//                                                                   //

//___________________________________________________________________//
//                                                                   //

CKernelLoaderBase::CKernelLoaderBase() : onInitializeCB(NULL), onGetKernelDescCB(NULL), onUninitializeCB(NULL) {}

bool CKernelLoaderBase::initialize()
{
	if (!isOpen()) { return false; }
	if (!onInitializeCB) { return true; }
	return onInitializeCB();
}

bool CKernelLoaderBase::getKernelDesc(IKernelDesc*& rpKernelDesc)
{
	if (!isOpen()) { return false; }
	if (!onGetKernelDescCB) { return false; }
	return onGetKernelDescCB(rpKernelDesc);
}

bool CKernelLoaderBase::uninitialize()
{
	if (!isOpen()) { return false; }
	if (!onUninitializeCB) { return true; }
	return onUninitializeCB();
}

void CKernelLoaderBase::release() { delete this; }

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

namespace OpenViBE
{
	class CKernelLoaderLinux : public CKernelLoaderBase
	{
	public:

		CKernelLoaderLinux();

		virtual bool load(const CString& sFileName, CString* pError);
		virtual bool unload(CString* pError);

	protected:

		virtual bool isOpen();

		void* m_pFileHandle;
	};
};

#elif defined TARGET_OS_Windows

namespace OpenViBE
{
	class CKernelLoaderWindows : public CKernelLoaderBase
	{
	public:
		CKernelLoaderWindows();

		virtual bool load(const CString& sFileName, CString* pError);
		virtual bool unload(CString* pError);

	protected:

		virtual bool isOpen();

		HMODULE m_pFileHandle;
	};
}  // namespace OpenViBE

#else

#endif

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

CKernelLoaderLinux::CKernelLoaderLinux() : m_pFileHandle(NULL) { }

bool CKernelLoaderLinux::load(const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="kernel already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(sFileName, RTLD_NOW|RTLD_LOCAL);
	m_pFileHandle=dlopen(sFileName, RTLD_LAZY|RTLD_GLOBAL);
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	onInitializeCB=(bool (*)())dlsym(m_pFileHandle, "onInitialize");
	onUninitializeCB=(bool (*)())dlsym(m_pFileHandle, "onUninitialize");
	onGetKernelDescCB=(bool (*)(IKernelDesc*&))dlsym(m_pFileHandle, "onGetKernelDesc");
	if(!onGetKernelDescCB)
	{
		if(pError) *pError=dlerror();

		dlclose(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetKernelDescCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}
	return true;
}

bool CKernelLoaderLinux::unload(CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no kernel currently loaded";
		return false;
	}
	dlclose(m_pFileHandle);
	m_pFileHandle=NULL;
	onInitializeCB=NULL;
	onGetKernelDescCB=NULL;
	onUninitializeCB=NULL;
	return true;
}

bool CKernelLoaderLinux::isOpen()
{
	return m_pFileHandle != nullptr;
}

#elif defined TARGET_OS_Windows

CKernelLoaderWindows::CKernelLoaderWindows() : m_pFileHandle(NULL) {}

bool CKernelLoaderWindows::load(const CString& sFileName, CString* pError)
{
	if (m_pFileHandle)
	{
		if (pError) { *pError = "kernel already loaded"; }
		return false;
	}

	m_pFileHandle = LoadLibrary(sFileName);
	if (!m_pFileHandle)
	{
		if (pError)
		{
			LPVOID l_pMessageBuffer = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, GetLastError(), 0, // Default language
						  (LPTSTR)&l_pMessageBuffer, 0, nullptr);
			*pError = (char*)l_pMessageBuffer;
			LocalFree(l_pMessageBuffer);
		}
		return false;
	}

	onInitializeCB    = (bool (*)())GetProcAddress(m_pFileHandle, "onInitialize");
	onUninitializeCB  = (bool (*)())GetProcAddress(m_pFileHandle, "onUninitialize");
	onGetKernelDescCB = (bool (*)(IKernelDesc*&))GetProcAddress(m_pFileHandle, "onGetKernelDesc");
	if (!onGetKernelDescCB)
	{
		if (pError)
		{
			LPVOID l_pMessageBuffer = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL, GetLastError(), 0, // Default language
						  (LPTSTR)&l_pMessageBuffer, 0, nullptr);
			*pError = (char*)l_pMessageBuffer;
			LocalFree(l_pMessageBuffer);
		}

		FreeLibrary(m_pFileHandle);
		m_pFileHandle     = NULL;
		onInitializeCB    = NULL;
		onGetKernelDescCB = NULL;
		onUninitializeCB  = NULL;
		return false;
	}
	return true;
}

bool CKernelLoaderWindows::unload(CString* pError)
{
	if (!m_pFileHandle)
	{
		if (pError) { *pError = "no kernel currently loaded"; }
		return false;
	}
	FreeLibrary(m_pFileHandle);
	m_pFileHandle     = NULL;
	onInitializeCB    = NULL;
	onGetKernelDescCB = NULL;
	onUninitializeCB  = NULL;

	return true;
}

bool CKernelLoaderWindows::isOpen()
{
	return m_pFileHandle != nullptr;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CKernelLoader::CKernelLoader() : m_pKernelLoaderImpl(NULL)
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_pKernelLoaderImpl=new CKernelLoaderLinux();
#elif defined TARGET_OS_Windows
	m_pKernelLoaderImpl = new CKernelLoaderWindows();
#else
#endif
}

CKernelLoader::~CKernelLoader() { delete m_pKernelLoaderImpl; }

bool CKernelLoader::load(const CString& sFileName, CString* pError)
{
	return m_pKernelLoaderImpl ? m_pKernelLoaderImpl->load(sFileName, pError) : false;
}

bool CKernelLoader::unload(CString* pError)
{
	return m_pKernelLoaderImpl ? m_pKernelLoaderImpl->unload(pError) : false;
}

bool CKernelLoader::initialize()
{
	return m_pKernelLoaderImpl ? m_pKernelLoaderImpl->initialize() : false;
}

bool CKernelLoader::getKernelDesc(IKernelDesc*& rpKernelDesc)
{
	return m_pKernelLoaderImpl ? m_pKernelLoaderImpl->getKernelDesc(rpKernelDesc) : false;
}

bool CKernelLoader::uninitialize()
{
	return m_pKernelLoaderImpl ? m_pKernelLoaderImpl->uninitialize() : false;
}

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

		CKernelLoaderBase() { }
		bool initialize() override;
		bool getKernelDesc(IKernelDesc*& rpKernelDesc) override;
		bool uninitialize() override;
		virtual void release();

		_IsDerivedFromClass_Final_(IKernelLoader, OV_UndefinedIdentifier)

		virtual bool isOpen() = 0;

	protected:

		CString m_filename;
		bool (*m_onInitializeCB)()                 = nullptr;
		bool (*m_onUninitializeCB)()               = nullptr;
		bool (*m_onGetKernelDescCB)(IKernelDesc*&) = nullptr;
	};
} // namespace OpenViBE

//___________________________________________________________________//
//                                                                   //

//___________________________________________________________________//
//                                                                   //

bool CKernelLoaderBase::initialize()
{
	if (!isOpen()) { return false; }
	if (!m_onInitializeCB) { return true; }
	return m_onInitializeCB();
}

bool CKernelLoaderBase::getKernelDesc(IKernelDesc*& rpKernelDesc)
{
	if (!isOpen()) { return false; }
	if (!m_onGetKernelDescCB) { return false; }
	return m_onGetKernelDescCB(rpKernelDesc);
}

bool CKernelLoaderBase::uninitialize()
{
	if (!isOpen()) { return false; }
	if (!m_onUninitializeCB) { return true; }
	return m_onUninitializeCB();
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

		virtual bool load(const CString& filename, CString* pError);
		virtual bool unload(CString* pError);

	protected:

		virtual bool isOpen();

		void* m_pFileHandle;
	};
};

#elif defined TARGET_OS_Windows

namespace OpenViBE
{
	class CKernelLoaderWindows final : public CKernelLoaderBase
	{
	public:
		CKernelLoaderWindows();
		bool load(const CString& filename, CString* error) override;
		bool unload(CString* pError) override;

	protected:
		bool isOpen() override;

		HMODULE m_pFileHandle;
	};
} // namespace OpenViBE

#else

#endif

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

CKernelLoaderLinux::CKernelLoaderLinux() : m_pFileHandle(NULL) { }

bool CKernelLoaderLinux::load(const CString& filename,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="kernel already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(filename, RTLD_NOW|RTLD_LOCAL);
	m_pFileHandle=dlopen(filename, RTLD_LAZY|RTLD_GLOBAL);
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	m_onInitializeCB=(bool (*)())dlsym(m_pFileHandle, "onInitialize");
	m_onUninitializeCB=(bool (*)())dlsym(m_pFileHandle, "onUninitialize");
	m_onGetKernelDescCB=(bool (*)(IKernelDesc*&))dlsym(m_pFileHandle, "onGetKernelDesc");
	if(!m_onGetKernelDescCB)
	{
		if(pError) *pError=dlerror();

		dlclose(m_pFileHandle);
		m_pFileHandle=NULL;
		m_onInitializeCB=NULL;
		m_onGetKernelDescCB=NULL;
		m_onUninitializeCB=NULL;
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
	m_onInitializeCB=NULL;
	m_onGetKernelDescCB=NULL;
	m_onUninitializeCB=NULL;
	return true;
}

bool CKernelLoaderLinux::isOpen()
{
	return m_pFileHandle != nullptr;
}

#elif defined TARGET_OS_Windows

CKernelLoaderWindows::CKernelLoaderWindows() : m_pFileHandle(nullptr) {}

bool CKernelLoaderWindows::load(const CString& filename, CString* error)
{
	if (m_pFileHandle)
	{
		if (error) { *error = "kernel already loaded"; }
		return false;
	}

	m_pFileHandle = LoadLibrary(filename);
	if (!m_pFileHandle)
	{
		if (error)
		{
			LPVOID buffer = nullptr;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(),
						  0, // Default language
						  LPTSTR(&buffer), 0, nullptr);
			*error = (char*)buffer;
			LocalFree(buffer);
		}
		return false;
	}

	m_onInitializeCB    = (bool (*)())GetProcAddress(m_pFileHandle, "onInitialize");
	m_onUninitializeCB  = (bool (*)())GetProcAddress(m_pFileHandle, "onUninitialize");
	m_onGetKernelDescCB = (bool (*)(IKernelDesc*&))GetProcAddress(m_pFileHandle, "onGetKernelDesc");
	if (!m_onGetKernelDescCB)
	{
		if (error)
		{
			LPVOID buffer = nullptr;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(),
						  0, // Default language
						  LPTSTR(&buffer), 0, nullptr);
			*error = (char*)buffer;
			LocalFree(buffer);
		}

		FreeLibrary(m_pFileHandle);
		m_pFileHandle       = nullptr;
		m_onInitializeCB    = nullptr;
		m_onGetKernelDescCB = nullptr;
		m_onUninitializeCB  = nullptr;
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
	m_pFileHandle       = nullptr;
	m_onInitializeCB    = nullptr;
	m_onGetKernelDescCB = nullptr;
	m_onUninitializeCB  = nullptr;

	return true;
}

bool CKernelLoaderWindows::isOpen() { return m_pFileHandle != nullptr; }

#else

#endif

//___________________________________________________________________//
//                                                                   //

CKernelLoader::CKernelLoader()
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_impl=new CKernelLoaderLinux();
#elif defined TARGET_OS_Windows
	m_impl = new CKernelLoaderWindows();
#else
#endif
}

CKernelLoader::~CKernelLoader() { delete m_impl; }

bool CKernelLoader::load(const CString& filename, CString* pError) { return m_impl ? m_impl->load(filename, pError) : false; }

bool CKernelLoader::unload(CString* pError) { return m_impl ? m_impl->unload(pError) : false; }

bool CKernelLoader::initialize() { return m_impl ? m_impl->initialize() : false; }

bool CKernelLoader::getKernelDesc(IKernelDesc*& rpKernelDesc) { return m_impl ? m_impl->getKernelDesc(rpKernelDesc) : false; }

bool CKernelLoader::uninitialize() { return m_impl ? m_impl->uninitialize() : false; }

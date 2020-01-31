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
using namespace /*OpenViBE::*/Kernel;

//___________________________________________________________________//
//                                                                   //

namespace OpenViBE
{
	class CKernelLoaderBase : public IKernelLoader
	{
	public:

		CKernelLoaderBase() { }
		bool initialize() override;
		bool getKernelDesc(IKernelDesc*& desc) override;
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

bool CKernelLoaderBase::initialize()
{
	if (!isOpen()) { return false; }
	if (!m_onInitializeCB) { return true; }
	return m_onInitializeCB();
}

bool CKernelLoaderBase::getKernelDesc(IKernelDesc*& desc)
{
	if (!isOpen() || !m_onGetKernelDescCB) { return false; }
	return m_onGetKernelDescCB(desc);
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

namespace OpenViBE
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	class CKernelLoaderLinux : public CKernelLoaderBase
	{
	public:

		CKernelLoaderLinux() : m_fileHandle(NULL) { }

		bool load(const CString& filename, CString* error) override;
		bool unload(CString* error) override;

	protected:

		bool isOpen() override{ return m_fileHandle != nullptr; }

		void* m_fileHandle;
	};

#elif defined TARGET_OS_Windows
	class CKernelLoaderWindows final : public CKernelLoaderBase
	{
	public:
		CKernelLoaderWindows() : m_fileHandle(nullptr) { }
		bool load(const CString& filename, CString* error) override;
		bool unload(CString* error) override;

	protected:
		bool isOpen() override { return m_fileHandle != nullptr; }

		HMODULE m_fileHandle;
	};

#else

#endif
} // namespace OpenViBE

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

bool CKernelLoaderLinux::load(const CString& filename, CString* error)
{
	if(m_fileHandle)
	{
		if(error) *error="kernel already loaded";
		return false;
	}

	// m_fileHandle=dlopen(filename, RTLD_NOW|RTLD_LOCAL);
	m_fileHandle=dlopen(filename, RTLD_LAZY|RTLD_GLOBAL);
	if(!m_fileHandle)
	{
		if(error) *error=dlerror();
		return false;
	}

	m_onInitializeCB=(bool (*)())dlsym(m_fileHandle, "onInitialize");
	m_onUninitializeCB=(bool (*)())dlsym(m_fileHandle, "onUninitialize");
	m_onGetKernelDescCB=(bool (*)(IKernelDesc*&))dlsym(m_fileHandle, "onGetKernelDesc");
	if(!m_onGetKernelDescCB)
	{
		if(error) *error=dlerror();

		dlclose(m_fileHandle);
		m_fileHandle=NULL;
		m_onInitializeCB=NULL;
		m_onGetKernelDescCB=NULL;
		m_onUninitializeCB=NULL;
		return false;
	}
	return true;
}

bool CKernelLoaderLinux::unload(CString* error)
{
	if(!m_fileHandle)
	{
		if(error) *error="no kernel currently loaded";
		return false;
	}
	dlclose(m_fileHandle);
	m_fileHandle=NULL;
	m_onInitializeCB=NULL;
	m_onGetKernelDescCB=NULL;
	m_onUninitializeCB=NULL;
	return true;
}

#elif defined TARGET_OS_Windows

bool CKernelLoaderWindows::load(const CString& filename, CString* error)
{
	if (m_fileHandle)
	{
		if (error) { *error = "kernel already loaded"; }
		return false;
	}

	m_fileHandle = LoadLibrary(filename);
	if (!m_fileHandle)
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

	m_onInitializeCB    = (bool (*)())GetProcAddress(m_fileHandle, "onInitialize");
	m_onUninitializeCB  = (bool (*)())GetProcAddress(m_fileHandle, "onUninitialize");
	m_onGetKernelDescCB = (bool (*)(IKernelDesc*&))GetProcAddress(m_fileHandle, "onGetKernelDesc");
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

		FreeLibrary(m_fileHandle);
		m_fileHandle        = nullptr;
		m_onInitializeCB    = nullptr;
		m_onGetKernelDescCB = nullptr;
		m_onUninitializeCB  = nullptr;
		return false;
	}
	return true;
}

bool CKernelLoaderWindows::unload(CString* error)
{
	if (!m_fileHandle)
	{
		if (error) { *error = "no kernel currently loaded"; }
		return false;
	}
	FreeLibrary(m_fileHandle);
	m_fileHandle        = nullptr;
	m_onInitializeCB    = nullptr;
	m_onGetKernelDescCB = nullptr;
	m_onUninitializeCB  = nullptr;

	return true;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CKernelLoader::CKernelLoader()
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_impl = new CKernelLoaderLinux();
#elif defined TARGET_OS_Windows
	m_impl = new CKernelLoaderWindows();
#else
#endif
}

CKernelLoader::~CKernelLoader() { delete m_impl; }

bool CKernelLoader::load(const CString& filename, CString* error) { return m_impl ? m_impl->load(filename, error) : false; }

bool CKernelLoader::unload(CString* error) { return m_impl ? m_impl->unload(error) : false; }

bool CKernelLoader::initialize() { return m_impl ? m_impl->initialize() : false; }

bool CKernelLoader::getKernelDesc(IKernelDesc*& desc) { return m_impl ? m_impl->getKernelDesc(desc) : false; }

bool CKernelLoader::uninitialize() { return m_impl ? m_impl->uninitialize() : false; }

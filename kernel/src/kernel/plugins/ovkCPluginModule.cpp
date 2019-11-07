#include "ovkCPluginModule.h"

#include <map>
#include <vector>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	#include <dlfcn.h>
#elif defined TARGET_OS_Windows
#include <windows.h>
#else
#endif

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleBase : public TKernelObject<IPluginModule>
		{
		public:
			explicit CPluginModuleBase(const IKernelContext& ctx);
			~CPluginModuleBase() override;
			bool initialize() override;
			bool getPluginObjectDescription(size_t index, IPluginObjectDesc*& pluginObjectDesc) override;
			bool uninitialize() override;
			bool getFileName(CString& rFileName) const override;

			_IsDerivedFromClass_Final_(TKernelObject<IPluginModule>, OV_UndefinedIdentifier)

		protected:

			virtual bool isOpen() const = 0;

			vector<IPluginObjectDesc*> m_pluginObjectDescs;
			CString m_filename;
			bool m_gotDesc;

			bool (*m_onInitializeCB)(const IPluginModuleContext&);
			bool (*m_onGetPluginObjectDescriptionCB)(const IPluginModuleContext&, size_t, IPluginObjectDesc*&);
			bool (*m_onUninitializeCB)(const IPluginModuleContext&);
		};
	} // namespace Kernel
} // namespace OpenViBE

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CPluginModuleContext final : public TKernelObject<IPluginModuleContext>
			{
			public:

				explicit CPluginModuleContext(const IKernelContext& ctx)
					: TKernelObject<IPluginModuleContext>(ctx), m_logManager(ctx.getLogManager()), m_typeManager(ctx.getTypeManager()),
					  m_rScenarioManager(ctx.getScenarioManager()) { }

				ILogManager& getLogManager() const override { return m_logManager; }
				ITypeManager& getTypeManager() const override { return m_typeManager; }
				IScenarioManager& getScenarioManager() const override { return m_rScenarioManager; }

				_IsDerivedFromClass_Final_(TKernelObject<IPluginModuleContext>, OVK_ClassId_Kernel_Plugins_PluginModuleContext)

			protected:

				ILogManager& m_logManager;
				ITypeManager& m_typeManager;
				IScenarioManager& m_rScenarioManager;
			};
		} // namespace
	} // namespace Kernel
} // namespace OpenViBE

//___________________________________________________________________//
//                                                                   //

CPluginModuleBase::CPluginModuleBase(const IKernelContext& ctx)
	: TKernelObject<IPluginModule>(ctx), m_gotDesc(false), m_onInitializeCB(nullptr), m_onGetPluginObjectDescriptionCB(nullptr), m_onUninitializeCB(nullptr) {}

CPluginModuleBase::~CPluginModuleBase() { }

bool CPluginModuleBase::initialize()
{
	if (!isOpen()) { return false; }
	if (!m_onInitializeCB) { return true; }
	return m_onInitializeCB(CPluginModuleContext(getKernelContext()));
}

bool CPluginModuleBase::getPluginObjectDescription(size_t index, IPluginObjectDesc*& pluginObjectDesc)
{
	if (!m_gotDesc)
	{
		if (!isOpen()) { return false; }
		if (!m_onGetPluginObjectDescriptionCB) { return false; }

		size_t idx             = 0;
		IPluginObjectDesc* pod = nullptr;
		while (m_onGetPluginObjectDescriptionCB(CPluginModuleContext(getKernelContext()), idx, pod))
		{
			if (pod) { m_pluginObjectDescs.push_back(pod); }
			idx++;
		}

		m_gotDesc = true;
	}

	if (index >= m_pluginObjectDescs.size())
	{
		pluginObjectDesc = nullptr;
		return false;
	}

	pluginObjectDesc = m_pluginObjectDescs[index];
	return true;
}

bool CPluginModuleBase::uninitialize()
{
	if (!isOpen()) { return false; }
	if (!m_onUninitializeCB) { return true; }
	return m_onUninitializeCB(CPluginModuleContext(getKernelContext()));
}

bool CPluginModuleBase::getFileName(CString& rFileName) const
{
	if (!isOpen()) { return false; }
	rFileName = m_filename;
	return true;
}

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleLinux : public CPluginModuleBase
		{
		public:

			CPluginModuleLinux(const IKernelContext& ctx);

			virtual bool load(const CString& filename, CString* pError);
			virtual bool unload(CString* pError);
			virtual bool isOpen() const;

		protected:

			void* m_pFileHandle;
		};
	}
} // namespace OpenViBE

#elif defined TARGET_OS_Windows

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleWindows final : public CPluginModuleBase
		{
		public:

			explicit CPluginModuleWindows(const IKernelContext& ctx);
			bool load(const CString& filename, CString* pError) override;
			bool unload(CString* pError) override;

		protected:
			bool isOpen() const override;

			HMODULE m_pFileHandle;

		private:

			CString getLastErrorMessageString();
		};
	} // namespace Kernel
} // namespace OpenViBE

#else

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleDummy : public CPluginModuleBase
		{
		public:

			explicit CPluginModuleDummy(const IKernelContext& ctx);

			virtual bool load(const CString& filename, CString* pError);
			virtual bool unload(CString* pError);

		protected:

			virtual bool isOpen() const;
		};
	}
} // namespace OpenViBE

#endif

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

CPluginModuleLinux::CPluginModuleLinux(const IKernelContext& ctx) :CPluginModuleBase(ctx),m_pFileHandle(NULL) { }

bool CPluginModuleLinux::load(const CString& filename, CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="plugin module already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(filename, RTLD_NOW|RTLD_LOCAL);
#if defined OV_LOCAL_SYMBOLS
	m_pFileHandle=dlopen(filename, RTLD_LAZY|RTLD_LOCAL);
#else
	m_pFileHandle=dlopen(filename, RTLD_LAZY|RTLD_GLOBAL);
#endif
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	m_onInitializeCB=(bool (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onInitialize");
	m_onUninitializeCB=(bool (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onUninitialize");
	m_onGetPluginObjectDescriptionCB=(bool (*)(const IPluginModuleContext&, size_t, Plugins::IPluginObjectDesc*&))dlsym(m_pFileHandle, "onGetPluginObjectDescription");

	if(!m_onGetPluginObjectDescriptionCB)
	{
		if(pError) *pError=dlerror();

		dlclose(m_pFileHandle);
		m_pFileHandle=NULL;
		m_onInitializeCB=NULL;
		m_onUninitializeCB=NULL;
		m_onGetPluginObjectDescriptionCB=NULL;
		return false;
	}

	m_filename=filename;
	return true;
}

bool CPluginModuleLinux::unload(CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no plugin module currently loaded";
		return false;
	}

	dlclose(m_pFileHandle);
	m_pFileHandle=NULL;
	m_onInitializeCB=NULL;
	m_onUninitializeCB=NULL;
	m_onGetPluginObjectDescriptionCB=NULL;
	return true;
}

bool CPluginModuleLinux::isOpen() const
{
	return m_pFileHandle != nullptr;
}

#elif defined TARGET_OS_Windows

CPluginModuleWindows::CPluginModuleWindows(const IKernelContext& ctx) : CPluginModuleBase(ctx), m_pFileHandle(nullptr) {}

bool CPluginModuleWindows::load(const CString& filename, CString* pError)
{
	if (m_pFileHandle)
	{
		if (pError) { *pError = "plugin module already loaded"; }
		return false;
	}

	m_pFileHandle = LoadLibrary(filename);
	if (!m_pFileHandle)
	{
		if (pError) { *pError = this->getLastErrorMessageString(); }
		return false;
	}

	m_onInitializeCB                 = reinterpret_cast<bool (*)(const IPluginModuleContext&)>(GetProcAddress(m_pFileHandle, "onInitialize"));
	m_onUninitializeCB               = reinterpret_cast<bool (*)(const IPluginModuleContext&)>(GetProcAddress(m_pFileHandle, "onUninitialize"));
	m_onGetPluginObjectDescriptionCB = reinterpret_cast<bool (*)(const IPluginModuleContext&, size_t, IPluginObjectDesc*&)>(GetProcAddress(
		m_pFileHandle, "onGetPluginObjectDescription"));
	if (!m_onGetPluginObjectDescriptionCB)
	{
		if (pError) { *pError = this->getLastErrorMessageString(); }

		FreeLibrary(m_pFileHandle);
		m_pFileHandle                    = nullptr;
		m_onInitializeCB                 = nullptr;
		m_onGetPluginObjectDescriptionCB = nullptr;
		m_onUninitializeCB               = nullptr;
		return false;
	}

	m_filename = filename;
	return true;
}

bool CPluginModuleWindows::unload(CString* pError)
{
	if (!m_pFileHandle)
	{
		if (pError) { *pError = "no plugin module currently loaded"; }
		return false;
	}

	FreeLibrary(m_pFileHandle);
	m_pFileHandle                    = nullptr;
	m_onInitializeCB                 = nullptr;
	m_onGetPluginObjectDescriptionCB = nullptr;
	m_onUninitializeCB               = nullptr;
	return true;
}

bool CPluginModuleWindows::isOpen() const { return m_pFileHandle != nullptr; }

CString CPluginModuleWindows::getLastErrorMessageString()
{
	CString res;

	char* buffer = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), 0,
				  reinterpret_cast<LPTSTR>(&buffer), 0, nullptr);
	if (buffer)
	{
		const size_t length = strlen(buffer);
		for (size_t i = 0; i < length; ++i) { if (buffer[i] == '\n' || buffer[i] == '\r') { buffer[i] = ' '; } }
		res = buffer;
	}
	LocalFree(LPVOID(buffer));

	return res;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CPluginModule::CPluginModule(const IKernelContext& ctx)
	: TKernelObject<IPluginModule>(ctx)
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_impl=new CPluginModuleLinux(getKernelContext());
#elif defined TARGET_OS_Windows
	m_impl = new CPluginModuleWindows(getKernelContext());
#else
#endif
}

CPluginModule::~CPluginModule() { delete m_impl; }

bool CPluginModule::load(const CString& filename, CString* pError) { return !m_impl ? false : m_impl->load(filename, pError); }

bool CPluginModule::unload(CString* pError) { return !m_impl ? false : m_impl->unload(pError); }

bool CPluginModule::initialize() { return !m_impl ? false : m_impl->initialize(); }

bool CPluginModule::getPluginObjectDescription(size_t index, IPluginObjectDesc*& pluginObjectDesc)
{
	return !m_impl ? false : m_impl->getPluginObjectDescription(index, pluginObjectDesc);
}

bool CPluginModule::uninitialize() { return !m_impl ? false : m_impl->uninitialize(); }

bool CPluginModule::getFileName(CString& rFileName) const { return !m_impl ? false : m_impl->getFileName(rFileName); }

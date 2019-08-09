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
			explicit CPluginModuleBase(const IKernelContext& rKernelContext);
			~CPluginModuleBase() override;
			bool initialize() override;
			bool getPluginObjectDescription(uint32_t ui32Index, IPluginObjectDesc*& rpPluginObjectDescription) override;
			bool uninitialize() override;
			bool getFileName(CString& rFileName) const override;

			_IsDerivedFromClass_Final_(IPluginModule, OV_UndefinedIdentifier)

		protected:

			virtual bool isOpen() const = 0;

			vector<IPluginObjectDesc*> m_vPluginObjectDescriptor;
			CString m_sFileName;
			bool m_bGotDescriptions;

			bool (*onInitializeCB)(const IPluginModuleContext&);
			bool (*onGetPluginObjectDescriptionCB)(const IPluginModuleContext&, uint32_t, IPluginObjectDesc*&);
			bool (*onUninitializeCB)(const IPluginModuleContext&);
		};
	}  // namespace Kernel
}  // namespace OpenViBE

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CPluginModuleContext : public TKernelObject<IPluginModuleContext>
			{
			public:

				explicit CPluginModuleContext(const IKernelContext& rKernelContext)
					: TKernelObject<IPluginModuleContext>(rKernelContext)
					  , m_rLogManager(rKernelContext.getLogManager())
					  , m_rTypeManager(rKernelContext.getTypeManager())
					  , m_rScenarioManager(rKernelContext.getScenarioManager()) { }

				ILogManager& getLogManager() const override { return m_rLogManager; }
				ITypeManager& getTypeManager() const override { return m_rTypeManager; }
				IScenarioManager& getScenarioManager() const override { return m_rScenarioManager; }

				_IsDerivedFromClass_Final_(TKernelObject<IPluginModuleContext>, OVK_ClassId_Kernel_Plugins_PluginModuleContext)

			protected:

				ILogManager& m_rLogManager;
				ITypeManager& m_rTypeManager;
				IScenarioManager& m_rScenarioManager;
			};
		}  // namespace
	}  // namespace Kernel
}  // namespace OpenViBE

//___________________________________________________________________//
//                                                                   //

CPluginModuleBase::CPluginModuleBase(const IKernelContext& rKernelContext)
	: TKernelObject<IPluginModule>(rKernelContext)
	  , m_bGotDescriptions(false)
	  , onInitializeCB(nullptr)
	  , onGetPluginObjectDescriptionCB(nullptr)
	  , onUninitializeCB(nullptr) {}

CPluginModuleBase::~CPluginModuleBase() { }

bool CPluginModuleBase::initialize()
{
	if (!isOpen()) { return false; }
	if (!onInitializeCB) { return true; }
	return onInitializeCB(CPluginModuleContext(getKernelContext()));
}

bool CPluginModuleBase::getPluginObjectDescription(uint32_t ui32Index, IPluginObjectDesc*& rpPluginObjectDescription)
{
	if (!m_bGotDescriptions)
	{
		if (!isOpen()) { return false; }
		if (!onGetPluginObjectDescriptionCB) { return false; }

		uint32_t l_ui32Index                           = 0;
		IPluginObjectDesc* l_pPluginObjectDescriptor = nullptr;
		while (onGetPluginObjectDescriptionCB(CPluginModuleContext(getKernelContext()), l_ui32Index, l_pPluginObjectDescriptor))
		{
			if (l_pPluginObjectDescriptor)
			{
				m_vPluginObjectDescriptor.push_back(l_pPluginObjectDescriptor);
			}
			l_ui32Index++;
		}

		m_bGotDescriptions = true;
	}

	if (ui32Index >= m_vPluginObjectDescriptor.size())
	{
		rpPluginObjectDescription = nullptr;
		return false;
	}

	rpPluginObjectDescription = m_vPluginObjectDescriptor[ui32Index];
	return true;
}

bool CPluginModuleBase::uninitialize()
{
	if (!isOpen()) { return false; }
	if (!onUninitializeCB) { return true; }
	return onUninitializeCB(CPluginModuleContext(getKernelContext()));
}

bool CPluginModuleBase::getFileName(CString& rFileName) const
{
	if (!isOpen()) { return false; }
	rFileName = m_sFileName;
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

			CPluginModuleLinux(const IKernelContext& rKernelContext);

			virtual bool load(const CString& sFileName, CString* pError);
			virtual bool unload(CString* pError);
			virtual bool isOpen() const;

		protected:

			void* m_pFileHandle;
		};
	};
};

#elif defined TARGET_OS_Windows

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleWindows : public CPluginModuleBase
		{
		public:

			explicit CPluginModuleWindows(const IKernelContext& rKernelContext);
			bool load(const CString& sFileName, CString* pError) override;
			bool unload(CString* pError) override;

		protected:
			bool isOpen() const override;

			HMODULE m_pFileHandle;

		private:

			CString getLastErrorMessageString();
		};
	}  // namespace Kernel
}  // namespace OpenViBE

#else

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleDummy : public CPluginModuleBase
		{
		public:

			explicit CPluginModuleDummy(const IKernelContext& rKernelContext);

			virtual bool load(const CString& sFileName, CString* pError);
			virtual bool unload(CString* pError);

		protected:

			virtual bool isOpen() const;
		};
	};
};

#endif

//___________________________________________________________________//
//                                                                   //

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

CPluginModuleLinux::CPluginModuleLinux(const IKernelContext& rKernelContext)
	:CPluginModuleBase(rKernelContext)
	,m_pFileHandle(NULL)
{
}

bool CPluginModuleLinux::load(const CString& sFileName, CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="plugin module already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(sFileName, RTLD_NOW|RTLD_LOCAL);
#if defined OV_LOCAL_SYMBOLS
	m_pFileHandle=dlopen(sFileName, RTLD_LAZY|RTLD_LOCAL);
#else
	m_pFileHandle=dlopen(sFileName, RTLD_LAZY|RTLD_GLOBAL);
#endif
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	onInitializeCB=(bool (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onInitialize");
	onUninitializeCB=(bool (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onUninitialize");
	onGetPluginObjectDescriptionCB=(bool (*)(const IPluginModuleContext&, uint32_t, Plugins::IPluginObjectDesc*&))dlsym(m_pFileHandle, "onGetPluginObjectDescription");

	if(!onGetPluginObjectDescriptionCB)
	{
		if(pError) *pError=dlerror();

		dlclose(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetPluginObjectDescriptionCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}

	m_sFileName=sFileName;
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
	onInitializeCB=NULL;
	onGetPluginObjectDescriptionCB=NULL;
	onUninitializeCB=NULL;
	return true;
}

bool CPluginModuleLinux::isOpen() const
{
	return m_pFileHandle != nullptr;
}

#elif defined TARGET_OS_Windows

CPluginModuleWindows::CPluginModuleWindows(const IKernelContext& rKernelContext)
	: CPluginModuleBase(rKernelContext)
	  , m_pFileHandle(nullptr) {}

bool CPluginModuleWindows::load(const CString& sFileName, CString* pError)
{
	if (m_pFileHandle)
	{
		if (pError) { *pError = "plugin module already loaded"; }
		return false;
	}

	m_pFileHandle = LoadLibrary(sFileName);
	if (!m_pFileHandle)
	{
		if (pError)
		{
			*pError = this->getLastErrorMessageString();
		}
		return false;
	}

	onInitializeCB                 = (bool (*)(const IPluginModuleContext&))GetProcAddress(m_pFileHandle, "onInitialize");
	onUninitializeCB               = (bool (*)(const IPluginModuleContext&))GetProcAddress(m_pFileHandle, "onUninitialize");
	onGetPluginObjectDescriptionCB = (bool (*)(const IPluginModuleContext&, uint32_t, IPluginObjectDesc*&))GetProcAddress(m_pFileHandle, "onGetPluginObjectDescription");
	if (!onGetPluginObjectDescriptionCB)
	{
		if (pError)
		{
			*pError = this->getLastErrorMessageString();
		}

		FreeLibrary(m_pFileHandle);
		m_pFileHandle                  = nullptr;
		onInitializeCB                 = nullptr;
		onGetPluginObjectDescriptionCB = nullptr;
		onUninitializeCB               = nullptr;
		return false;
	}

	m_sFileName = sFileName;
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
	m_pFileHandle                  = nullptr;
	onInitializeCB                 = nullptr;
	onGetPluginObjectDescriptionCB = nullptr;
	onUninitializeCB               = nullptr;
	return true;
}

bool CPluginModuleWindows::isOpen() const
{
	return m_pFileHandle != nullptr;
}

CString CPluginModuleWindows::getLastErrorMessageString()
{
	CString l_sResult;

	char* l_pMessageBuffer = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), 0, (LPTSTR)&l_pMessageBuffer, 0, nullptr);
	if (l_pMessageBuffer)
	{
		size_t l_iMessageLength = strlen(l_pMessageBuffer);
		for (size_t i = 0; i < l_iMessageLength; i++)
		{
			if (l_pMessageBuffer[i] == '\n' || l_pMessageBuffer[i] == '\r')
			{
				l_pMessageBuffer[i] = ' ';
			}
		}
		l_sResult = l_pMessageBuffer;
	}
	LocalFree((LPVOID)l_pMessageBuffer);

	return l_sResult;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CPluginModule::CPluginModule(const IKernelContext& rKernelContext)
	: TKernelObject<IPluginModule>(rKernelContext), m_pImplementation(nullptr)
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_pImplementation=new CPluginModuleLinux(getKernelContext());
#elif defined TARGET_OS_Windows
	m_pImplementation = new CPluginModuleWindows(getKernelContext());
#else
#endif
}

CPluginModule::~CPluginModule()
{
	delete m_pImplementation;
}

bool CPluginModule::load(const CString& sFileName, CString* pError)
{
	return !m_pImplementation ? false : m_pImplementation->load(sFileName, pError);
}

bool CPluginModule::unload(CString* pError)
{
	return !m_pImplementation ? false : m_pImplementation->unload(pError);
}

bool CPluginModule::initialize()
{
	return !m_pImplementation ? false : m_pImplementation->initialize();
}

bool CPluginModule::getPluginObjectDescription(uint32_t ui32Index,
	IPluginObjectDesc*& rpPluginObjectDescription)
{
	return !m_pImplementation ? false : m_pImplementation->getPluginObjectDescription(ui32Index, rpPluginObjectDescription);
}

bool CPluginModule::uninitialize()
{
	return !m_pImplementation ? false : m_pImplementation->uninitialize();
}

bool CPluginModule::getFileName(CString& rFileName) const
{
	return !m_pImplementation ? false : m_pImplementation->getFileName(rFileName);
}

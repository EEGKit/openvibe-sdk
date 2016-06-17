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
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleBase : public TKernelObject<IPluginModule>
		{
		public:
			CPluginModuleBase(const IKernelContext& rKernelContext);
			virtual ~CPluginModuleBase();

			virtual boolean initialize(void);
			virtual boolean getPluginObjectDescription(
				uint32 ui32Index,
				IPluginObjectDesc*& rpPluginObjectDescription);
			virtual boolean uninitialize(void);
			virtual boolean getFileName(CString& rFileName) const;

			_IsDerivedFromClass_Final_(IPluginModule, OV_UndefinedIdentifier);

		protected:

			virtual boolean isOpen(void) const=0;

			vector<IPluginObjectDesc*> m_vPluginObjectDescriptor;
			CString m_sFileName;
			boolean m_bGotDescriptions;

			boolean (*onInitializeCB)(const IPluginModuleContext&);
			boolean (*onGetPluginObjectDescriptionCB)(const IPluginModuleContext&, uint32, Plugins::IPluginObjectDesc*&);
			boolean (*onUninitializeCB)(const IPluginModuleContext&);
		};
	};
};

namespace OpenViBE
{
	namespace Kernel
	{
		namespace
		{
			class CPluginModuleContext : public TKernelObject<IPluginModuleContext>
			{
			public:

				CPluginModuleContext(const IKernelContext& rKernelContext)
					:TKernelObject<IPluginModuleContext>(rKernelContext)
					,m_rLogManager(rKernelContext.getLogManager())
					,m_rTypeManager(rKernelContext.getTypeManager())
					,m_rScenarioManager(rKernelContext.getScenarioManager())
				{
				}

				virtual ILogManager& getLogManager(void) const { return m_rLogManager; };
				virtual ITypeManager& getTypeManager(void) const { return m_rTypeManager; }
				virtual IScenarioManager& getScenarioManager(void) const { return m_rScenarioManager; }

				_IsDerivedFromClass_Final_(TKernelObject<IPluginModuleContext>, OVK_ClassId_Kernel_Plugins_PluginModuleContext);

			protected:

				ILogManager& m_rLogManager;
				ITypeManager& m_rTypeManager;
				IScenarioManager& m_rScenarioManager;
			};
		};
	};
};

#define boolean OpenViBE::boolean

//___________________________________________________________________//
//                                                                   //

CPluginModuleBase::CPluginModuleBase(const IKernelContext& rKernelContext)
	:TKernelObject<IPluginModule>(rKernelContext)
	,m_bGotDescriptions(false)
	,onInitializeCB(NULL)
	,onGetPluginObjectDescriptionCB(NULL)
	,onUninitializeCB(NULL)
{
}

CPluginModuleBase::~CPluginModuleBase() { };

boolean CPluginModuleBase::initialize(void)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onInitializeCB)
	{
		return true;
	}
	return onInitializeCB(CPluginModuleContext(getKernelContext()));
}

boolean CPluginModuleBase::getPluginObjectDescription(
	uint32 ui32Index,
	IPluginObjectDesc*& rpPluginObjectDescription)
{
	if(!m_bGotDescriptions)
	{
		if(!isOpen())
		{
			return false;
		}
		if(!onGetPluginObjectDescriptionCB)
		{
			return false;
		}

		uint32 l_ui32Index=0;
		IPluginObjectDesc* l_pPluginObjectDescriptor=NULL;
		while(onGetPluginObjectDescriptionCB(CPluginModuleContext(getKernelContext()), l_ui32Index, l_pPluginObjectDescriptor))
		{
			if(l_pPluginObjectDescriptor)
			{
				m_vPluginObjectDescriptor.push_back(l_pPluginObjectDescriptor);
			}
			l_ui32Index++;
		}

		m_bGotDescriptions=true;
	}

	if(ui32Index>=m_vPluginObjectDescriptor.size())
	{
		rpPluginObjectDescription=NULL;
		return false;
	}

	rpPluginObjectDescription=m_vPluginObjectDescriptor[ui32Index];
	return true;
}

boolean CPluginModuleBase::uninitialize(void)
{
	if(!isOpen())
	{
		return false;
	}
	if(!onUninitializeCB)
	{
		return true;
	}
	return onUninitializeCB(CPluginModuleContext(getKernelContext()));
}

boolean CPluginModuleBase::getFileName(
	CString& rFileName) const
{
	if(!isOpen())
	{
		return false;
	}
	rFileName=m_sFileName;
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

			virtual boolean load(
				const CString& sFileName,
				CString* pError);
			virtual boolean unload(
				CString* pError);
			virtual boolean isOpen(void) const;

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

			CPluginModuleWindows(const IKernelContext& rKernelContext);

			virtual boolean load(
				const CString& sFileName,
				CString* pError);
			virtual boolean unload(
				CString* pError);

		protected:

			virtual boolean isOpen(void) const;

			HMODULE m_pFileHandle;

		private:

			CString getLastErrorMessageString(void);
		};
	};
};

#else

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModuleDummy : public CPluginModuleBase
		{
		public:

			CPluginModuleDummy(const IKernelContext& rKernelContext);

			virtual boolean load(
				const CString& sFileName,
				CString* pError);
			virtual boolean unload(
				CString* pError);

		protected:

			virtual boolean isOpen(void) const;
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

boolean CPluginModuleLinux::load(
	const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="plugin module already loaded";
		return false;
	}

	// m_pFileHandle=dlopen(sFileName, RTLD_NOW|RTLD_LOCAL);
	m_pFileHandle=dlopen(sFileName, RTLD_LAZY|RTLD_GLOBAL);
	if(!m_pFileHandle)
	{
		if(pError) *pError=dlerror();
		return false;
	}

	onInitializeCB=(boolean (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onInitialize");
	onUninitializeCB=(boolean (*)(const IPluginModuleContext&))dlsym(m_pFileHandle, "onUninitialize");
	onGetPluginObjectDescriptionCB=(boolean (*)(const IPluginModuleContext&, uint32, Plugins::IPluginObjectDesc*&))dlsym(m_pFileHandle, "onGetPluginObjectDescription");

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

boolean CPluginModuleLinux::unload(
	CString* pError)
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

boolean CPluginModuleLinux::isOpen(void) const
{
	return m_pFileHandle!=NULL;
}

#elif defined TARGET_OS_Windows

CPluginModuleWindows::CPluginModuleWindows(const IKernelContext& rKernelContext)
	:CPluginModuleBase(rKernelContext)
	,m_pFileHandle(NULL)
{
}

boolean CPluginModuleWindows::load(
	const CString& sFileName,
	CString* pError)
{
	if(m_pFileHandle)
	{
		if(pError) *pError="plugin module already loaded";
		return false;
	}

	m_pFileHandle=LoadLibrary(sFileName);
	if(!m_pFileHandle)
	{
		if(pError)
		{
			*pError=this->getLastErrorMessageString();
		}
		return false;
	}

	onInitializeCB=(boolean (*)(const IPluginModuleContext&))GetProcAddress(m_pFileHandle, "onInitialize");
	onUninitializeCB=(boolean (*)(const IPluginModuleContext&))GetProcAddress(m_pFileHandle, "onUninitialize");
	onGetPluginObjectDescriptionCB=(boolean (*)(const IPluginModuleContext&, uint32, Plugins::IPluginObjectDesc*&))GetProcAddress(m_pFileHandle, "onGetPluginObjectDescription");
	if(!onGetPluginObjectDescriptionCB)
	{
		if(pError)
		{
			*pError=this->getLastErrorMessageString();
		}

		FreeLibrary(m_pFileHandle);
		m_pFileHandle=NULL;
		onInitializeCB=NULL;
		onGetPluginObjectDescriptionCB=NULL;
		onUninitializeCB=NULL;
		return false;
	}

	m_sFileName=sFileName;
	return true;
}

boolean CPluginModuleWindows::unload(
	CString* pError)
{
	if(!m_pFileHandle)
	{
		if(pError) *pError="no plugin module currently loaded";
		return false;
	}

	FreeLibrary(m_pFileHandle);
	m_pFileHandle=NULL;
	onInitializeCB=NULL;
	onGetPluginObjectDescriptionCB=NULL;
	onUninitializeCB=NULL;
	return true;
}

boolean CPluginModuleWindows::isOpen(void) const
{
	return m_pFileHandle!=NULL;
}

CString CPluginModuleWindows::getLastErrorMessageString(void)
{
	CString l_sResult;

	char* l_pMessageBuffer=NULL;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM|
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0, // Default language
		(LPTSTR)&l_pMessageBuffer,
		0,
		NULL);
	if(l_pMessageBuffer)
	{
		int l_iMessageLength=::strlen(l_pMessageBuffer);
		for(int i=0; i<l_iMessageLength; i++)
		{
			if(l_pMessageBuffer[i]=='\n' || l_pMessageBuffer[i]=='\r')
			{
				l_pMessageBuffer[i]=' ';
			}
		}
		l_sResult=l_pMessageBuffer;
	}
	LocalFree((LPVOID)l_pMessageBuffer);

	return l_sResult;
}

#else

#endif

//___________________________________________________________________//
//                                                                   //

CPluginModule::CPluginModule(const IKernelContext& rKernelContext)
	:TKernelObject<IPluginModule>(rKernelContext)
	,m_pImplementation(NULL)
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_pImplementation=new CPluginModuleLinux(getKernelContext());
#elif defined TARGET_OS_Windows
	m_pImplementation=new CPluginModuleWindows(getKernelContext());
#else
#endif
}

CPluginModule::~CPluginModule(void)
{
	delete m_pImplementation;
}

boolean CPluginModule::load(
	const CString& sFileName,
	CString* pError)
{
	return !m_pImplementation?false:m_pImplementation->load(sFileName, pError);
}

boolean CPluginModule::unload(
	CString* pError)
{
	return !m_pImplementation?false:m_pImplementation->unload(pError);
}

boolean CPluginModule::initialize(void)
{
	return !m_pImplementation?false:m_pImplementation->initialize();
}

boolean CPluginModule::getPluginObjectDescription(
	uint32 ui32Index,
	IPluginObjectDesc*& rpPluginObjectDescription)
{
	return !m_pImplementation?false:m_pImplementation->getPluginObjectDescription(ui32Index, rpPluginObjectDescription);
}

boolean CPluginModule::uninitialize(void)
{
	return !m_pImplementation?false:m_pImplementation->uninitialize();
}

boolean CPluginModule::getFileName(
	CString& rFileName) const
{
	return !m_pImplementation?false:m_pImplementation->getFileName(rFileName);
}

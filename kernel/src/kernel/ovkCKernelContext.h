#ifndef __OpenViBEKernel_CKernelContext_H__
#define __OpenViBEKernel_CKernelContext_H__

#include <memory>

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole;
		class CLogListenerFile;

		class CKernelContext : public OpenViBE::Kernel::IKernelContext
		{
		public:

			CKernelContext(const OpenViBE::Kernel::IKernelContext* pMasterKernelContext, const OpenViBE::CString& rApplicationName, const OpenViBE::CString& rConfigurationFile);
			virtual ~CKernelContext(void);

			virtual bool initialize(const char* const * tokenList, size_t tokenCount);
			virtual bool uninitialize(void);

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const;
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const;
			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const;
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const;
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const;
			virtual OpenViBE::Kernel::IMetaboxManager& getMetaboxManager(void) const;
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const;
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const;
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const;
			virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			virtual OpenViBE::Kernel::ELogLevel earlyGetLogLevel(const OpenViBE::CString& rLogLevelName);

		private:

			const OpenViBE::Kernel::IKernelContext& m_rMasterKernelContext;

			std::unique_ptr<OpenViBE::Kernel::IAlgorithmManager> m_pAlgorithmManager;
			std::unique_ptr<OpenViBE::Kernel::IConfigurationManager> m_pConfigurationManager;
			std::unique_ptr<OpenViBE::Kernel::IKernelObjectFactory> m_pKernelObjectFactory;
			std::unique_ptr<OpenViBE::Kernel::IPlayerManager> m_pPlayerManager;
			std::unique_ptr<OpenViBE::Kernel::IPluginManager> m_pPluginManager;
			std::unique_ptr<OpenViBE::Kernel::IMetaboxManager> m_pMetaboxManager;
			std::unique_ptr<OpenViBE::Kernel::IScenarioManager> m_pScenarioManager;
			std::unique_ptr<OpenViBE::Kernel::ITypeManager> m_pTypeManager;
			std::unique_ptr<OpenViBE::Kernel::ILogManager> m_pLogManager;
			std::unique_ptr<OpenViBE::Kernel::IErrorManager> m_pErrorManager;

			OpenViBE::CString m_sApplicationName;
			OpenViBE::CString m_sConfigurationFile;

			std::unique_ptr<OpenViBE::Kernel::CLogListenerConsole> m_pLogListenerConsole;
			std::unique_ptr<OpenViBE::Kernel::CLogListenerFile> m_pLogListenerFile;

		private:

			CKernelContext(void);
		};

		class CKernelContextBridge : public OpenViBE::Kernel::IKernelContext
		{
		public:

			explicit CKernelContextBridge(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				: m_kernelContext(rKernelContext)
				  , m_pAlgorithmManager(NULL)
				  , m_pConfigurationManager(NULL)
				  , m_pKernelObjectFactory(NULL)
				  , m_pPlayerManager(NULL)
				  , m_pPluginManager(NULL)
				  , m_pMetaboxManager(NULL)
				  , m_pScenarioManager(NULL)
				  , m_pTypeManager(NULL)
				  , m_pLogManager(NULL)
				  , m_pErrorManager(NULL) { }

			virtual bool initialize(void) { return true; }
			virtual bool uninitialize(void) { return true; }

			void setAlgorithmManager(OpenViBE::Kernel::IAlgorithmManager* pAlgorithmManager) { m_pAlgorithmManager = pAlgorithmManager; }
			void setConfigurationManager(OpenViBE::Kernel::IConfigurationManager* pConfigurationManager) { m_pConfigurationManager = pConfigurationManager; }
			void setKernelObjectFactory(OpenViBE::Kernel::IKernelObjectFactory* pKernelObjectFactory) { m_pKernelObjectFactory = pKernelObjectFactory; }
			void setPlayerManager(OpenViBE::Kernel::IPlayerManager* pPlayerManager) { m_pPlayerManager = pPlayerManager; }
			void setPluginManager(OpenViBE::Kernel::IPluginManager* pPluginManager) { m_pPluginManager = pPluginManager; }
			void setMetaboxManager(OpenViBE::Kernel::IMetaboxManager* pMetaboxManager) { m_pMetaboxManager = pMetaboxManager; }
			void setScenarioManager(OpenViBE::Kernel::IScenarioManager* pScenarioManager) { m_pScenarioManager = pScenarioManager; }
			void setTypeManager(OpenViBE::Kernel::ITypeManager* pTypeManager) { m_pTypeManager = pTypeManager; }
			void setLogManager(OpenViBE::Kernel::ILogManager* pLogManager) { m_pLogManager = pLogManager; }
			void setErrorManager(OpenViBE::Kernel::IErrorManager* pErrorManager) { m_pErrorManager = pErrorManager; }

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const { return m_pAlgorithmManager ? *m_pAlgorithmManager : m_kernelContext.getAlgorithmManager(); }
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const { return m_pConfigurationManager ? *m_pConfigurationManager : m_kernelContext.getConfigurationManager(); }
			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const { return m_pKernelObjectFactory ? *m_pKernelObjectFactory : m_kernelContext.getKernelObjectFactory(); }
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const { return m_pPlayerManager ? *m_pPlayerManager : m_kernelContext.getPlayerManager(); }
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const { return m_pPluginManager ? *m_pPluginManager : m_kernelContext.getPluginManager(); }
			virtual OpenViBE::Kernel::IMetaboxManager& getMetaboxManager(void) const { return m_pMetaboxManager ? *m_pMetaboxManager : m_kernelContext.getMetaboxManager(); }
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const { return m_pScenarioManager ? *m_pScenarioManager : m_kernelContext.getScenarioManager(); }
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const { return m_pTypeManager ? *m_pTypeManager : m_kernelContext.getTypeManager(); }
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const { return m_pLogManager ? *m_pLogManager : m_kernelContext.getLogManager(); }
			virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void) const { return m_pErrorManager ? *m_pErrorManager : m_kernelContext.getErrorManager(); }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			const OpenViBE::Kernel::IKernelContext& m_kernelContext;

		protected:

			mutable OpenViBE::Kernel::IAlgorithmManager* m_pAlgorithmManager;
			mutable OpenViBE::Kernel::IConfigurationManager* m_pConfigurationManager;
			mutable OpenViBE::Kernel::IKernelObjectFactory* m_pKernelObjectFactory;
			mutable OpenViBE::Kernel::IPlayerManager* m_pPlayerManager;
			mutable OpenViBE::Kernel::IPluginManager* m_pPluginManager;
			mutable OpenViBE::Kernel::IMetaboxManager* m_pMetaboxManager;
			mutable OpenViBE::Kernel::IScenarioManager* m_pScenarioManager;
			mutable OpenViBE::Kernel::ITypeManager* m_pTypeManager;
			mutable OpenViBE::Kernel::ILogManager* m_pLogManager;
			mutable OpenViBE::Kernel::IErrorManager* m_pErrorManager;
		};
	};
};

#endif // __OpenViBEKernel_CKernelContext_H__

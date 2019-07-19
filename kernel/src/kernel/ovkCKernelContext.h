#pragma once

#include <memory>

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole;
		class CLogListenerFile;

		class CKernelContext : public IKernelContext
		{
		public:

			CKernelContext(const IKernelContext* pMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFile);
			virtual ~CKernelContext();

			virtual bool initialize(const char* const * tokenList, size_t tokenCount);
			virtual bool uninitialize();

			virtual IAlgorithmManager& getAlgorithmManager() const;
			virtual IConfigurationManager& getConfigurationManager() const;
			virtual IKernelObjectFactory& getKernelObjectFactory() const;
			virtual IPlayerManager& getPlayerManager() const;
			virtual IPluginManager& getPluginManager() const;
			virtual IMetaboxManager& getMetaboxManager() const;
			virtual IScenarioManager& getScenarioManager() const;
			virtual ITypeManager& getTypeManager() const;
			virtual ILogManager& getLogManager() const;
			virtual IErrorManager& getErrorManager() const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			virtual ELogLevel earlyGetLogLevel(const CString& rLogLevelName);

		private:

			const IKernelContext& m_rMasterKernelContext;

			std::unique_ptr<IAlgorithmManager> m_pAlgorithmManager;
			std::unique_ptr<IConfigurationManager> m_pConfigurationManager;
			std::unique_ptr<IKernelObjectFactory> m_pKernelObjectFactory;
			std::unique_ptr<IPlayerManager> m_pPlayerManager;
			std::unique_ptr<IPluginManager> m_pPluginManager;
			std::unique_ptr<IMetaboxManager> m_pMetaboxManager;
			std::unique_ptr<IScenarioManager> m_pScenarioManager;
			std::unique_ptr<ITypeManager> m_pTypeManager;
			std::unique_ptr<ILogManager> m_pLogManager;
			std::unique_ptr<IErrorManager> m_pErrorManager;

			CString m_sApplicationName;
			CString m_sConfigurationFile;

			std::unique_ptr<CLogListenerConsole> m_pLogListenerConsole;
			std::unique_ptr<CLogListenerFile> m_pLogListenerFile;

		private:

			CKernelContext();
		};

		class CKernelContextBridge : public IKernelContext
		{
		public:

			explicit CKernelContextBridge(const IKernelContext& rKernelContext)
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

			virtual bool initialize() { return true; }
			virtual bool uninitialize() { return true; }

			void setAlgorithmManager(IAlgorithmManager* pAlgorithmManager) { m_pAlgorithmManager = pAlgorithmManager; }
			void setConfigurationManager(IConfigurationManager* pConfigurationManager) { m_pConfigurationManager = pConfigurationManager; }
			void setKernelObjectFactory(IKernelObjectFactory* pKernelObjectFactory) { m_pKernelObjectFactory = pKernelObjectFactory; }
			void setPlayerManager(IPlayerManager* pPlayerManager) { m_pPlayerManager = pPlayerManager; }
			void setPluginManager(IPluginManager* pPluginManager) { m_pPluginManager = pPluginManager; }
			void setMetaboxManager(IMetaboxManager* pMetaboxManager) { m_pMetaboxManager = pMetaboxManager; }
			void setScenarioManager(IScenarioManager* pScenarioManager) { m_pScenarioManager = pScenarioManager; }
			void setTypeManager(ITypeManager* pTypeManager) { m_pTypeManager = pTypeManager; }
			void setLogManager(ILogManager* pLogManager) { m_pLogManager = pLogManager; }
			void setErrorManager(IErrorManager* pErrorManager) { m_pErrorManager = pErrorManager; }

			virtual IAlgorithmManager& getAlgorithmManager() const { return m_pAlgorithmManager ? *m_pAlgorithmManager : m_kernelContext.getAlgorithmManager(); }
			virtual IConfigurationManager& getConfigurationManager() const { return m_pConfigurationManager ? *m_pConfigurationManager : m_kernelContext.getConfigurationManager(); }
			virtual IKernelObjectFactory& getKernelObjectFactory() const { return m_pKernelObjectFactory ? *m_pKernelObjectFactory : m_kernelContext.getKernelObjectFactory(); }
			virtual IPlayerManager& getPlayerManager() const { return m_pPlayerManager ? *m_pPlayerManager : m_kernelContext.getPlayerManager(); }
			virtual IPluginManager& getPluginManager() const { return m_pPluginManager ? *m_pPluginManager : m_kernelContext.getPluginManager(); }
			virtual IMetaboxManager& getMetaboxManager() const { return m_pMetaboxManager ? *m_pMetaboxManager : m_kernelContext.getMetaboxManager(); }
			virtual IScenarioManager& getScenarioManager() const { return m_pScenarioManager ? *m_pScenarioManager : m_kernelContext.getScenarioManager(); }
			virtual ITypeManager& getTypeManager() const { return m_pTypeManager ? *m_pTypeManager : m_kernelContext.getTypeManager(); }
			virtual ILogManager& getLogManager() const { return m_pLogManager ? *m_pLogManager : m_kernelContext.getLogManager(); }
			virtual IErrorManager& getErrorManager() const { return m_pErrorManager ? *m_pErrorManager : m_kernelContext.getErrorManager(); }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			const IKernelContext& m_kernelContext;

		protected:

			mutable IAlgorithmManager* m_pAlgorithmManager;
			mutable IConfigurationManager* m_pConfigurationManager;
			mutable IKernelObjectFactory* m_pKernelObjectFactory;
			mutable IPlayerManager* m_pPlayerManager;
			mutable IPluginManager* m_pPluginManager;
			mutable IMetaboxManager* m_pMetaboxManager;
			mutable IScenarioManager* m_pScenarioManager;
			mutable ITypeManager* m_pTypeManager;
			mutable ILogManager* m_pLogManager;
			mutable IErrorManager* m_pErrorManager;
		};
	};
};



#pragma once

#include <memory>

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole;
		class CLogListenerFile;

		class CKernelContext final : public IKernelContext
		{
		public:

			CKernelContext(const IKernelContext* pMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFile);
			~CKernelContext() override;
			bool initialize(const char* const * tokenList, size_t tokenCount) override;
			bool uninitialize() override;
			IAlgorithmManager& getAlgorithmManager() const override;
			IConfigurationManager& getConfigurationManager() const override;
			IKernelObjectFactory& getKernelObjectFactory() const override;
			IPlayerManager& getPlayerManager() const override;
			IPluginManager& getPluginManager() const override;
			IMetaboxManager& getMetaboxManager() const override;
			IScenarioManager& getScenarioManager() const override;
			ITypeManager& getTypeManager() const override;
			ILogManager& getLogManager() const override;
			IErrorManager& getErrorManager() const override;

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

			CKernelContext() = delete;
		};

		class CKernelContextBridge final : public IKernelContext
		{
		public:

			explicit CKernelContextBridge(const IKernelContext& ctx) : m_kernelContext(ctx) { }

			virtual bool initialize() { return true; }
			bool uninitialize() override { return true; }

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

			IAlgorithmManager& getAlgorithmManager() const override
			{
				return m_pAlgorithmManager ? *m_pAlgorithmManager : m_kernelContext.getAlgorithmManager();
			}

			IConfigurationManager& getConfigurationManager() const override
			{
				return m_pConfigurationManager ? *m_pConfigurationManager : m_kernelContext.getConfigurationManager();
			}

			IKernelObjectFactory& getKernelObjectFactory() const override
			{
				return m_pKernelObjectFactory ? *m_pKernelObjectFactory : m_kernelContext.getKernelObjectFactory();
			}

			IPlayerManager& getPlayerManager() const override { return m_pPlayerManager ? *m_pPlayerManager : m_kernelContext.getPlayerManager(); }
			IPluginManager& getPluginManager() const override { return m_pPluginManager ? *m_pPluginManager : m_kernelContext.getPluginManager(); }
			IMetaboxManager& getMetaboxManager() const override { return m_pMetaboxManager ? *m_pMetaboxManager : m_kernelContext.getMetaboxManager(); }
			IScenarioManager& getScenarioManager() const override { return m_pScenarioManager ? *m_pScenarioManager : m_kernelContext.getScenarioManager(); }
			ITypeManager& getTypeManager() const override { return m_pTypeManager ? *m_pTypeManager : m_kernelContext.getTypeManager(); }
			ILogManager& getLogManager() const override { return m_pLogManager ? *m_pLogManager : m_kernelContext.getLogManager(); }
			IErrorManager& getErrorManager() const override { return m_pErrorManager ? *m_pErrorManager : m_kernelContext.getErrorManager(); }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelContext, OVK_ClassId_Kernel_KernelContext)

		protected:

			const IKernelContext& m_kernelContext;

			mutable IAlgorithmManager* m_pAlgorithmManager         = nullptr;
			mutable IConfigurationManager* m_pConfigurationManager = nullptr;
			mutable IKernelObjectFactory* m_pKernelObjectFactory   = nullptr;
			mutable IPlayerManager* m_pPlayerManager               = nullptr;
			mutable IPluginManager* m_pPluginManager               = nullptr;
			mutable IMetaboxManager* m_pMetaboxManager             = nullptr;
			mutable IScenarioManager* m_pScenarioManager           = nullptr;
			mutable ITypeManager* m_pTypeManager                   = nullptr;
			mutable ILogManager* m_pLogManager                     = nullptr;
			mutable IErrorManager* m_pErrorManager                 = nullptr;
		};
	} // namespace Kernel
} // namespace OpenViBE

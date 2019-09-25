#pragma once

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginManager final : public TKernelObject<IPluginManager>
		{
		public:

			explicit CPluginManager(const IKernelContext& ctx) : TKernelObject<IPluginManager>(ctx) {}
			~CPluginManager() override;
			bool addPluginsFromFiles(const CString& rFileNameWildCard) override;
			bool registerPluginDesc(const Plugins::IPluginObjectDesc& rPluginObjectDesc) override;
			CIdentifier getNextPluginObjectDescIdentifier(const CIdentifier& previousID) const override;
			CIdentifier getNextPluginObjectDescIdentifier(const CIdentifier& previousID, const CIdentifier& rBaseClassIdentifier) const override;
			bool canCreatePluginObject(const CIdentifier& rClassIdentifier) override;
			const Plugins::IPluginObjectDesc* getPluginObjectDesc(const CIdentifier& rClassIdentifier) const override;
			const Plugins::IPluginObjectDesc* getPluginObjectDescCreating(const CIdentifier& rClassIdentifier) const override;
			CIdentifier getPluginObjectHashValue(const CIdentifier& rClassIdentifier) const override;
			CIdentifier getPluginObjectHashValue(const Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc) const override;
			bool isPluginObjectFlaggedAsDeprecated(const CIdentifier& rClassIdentifier) const override;
			Plugins::IPluginObject* createPluginObject(const CIdentifier& rClassIdentifier) override;
			bool releasePluginObject(Plugins::IPluginObject* pPluginObject) override;
			Plugins::IAlgorithm* createAlgorithm(const CIdentifier& rClassIdentifier, const Plugins::IAlgorithmDesc** ppAlgorithmDesc) override;
			Plugins::IAlgorithm* createAlgorithm(const Plugins::IAlgorithmDesc& rAlgorithmDesc) override;
			Plugins::IBoxAlgorithm* createBoxAlgorithm(const CIdentifier& rClassIdentifier, const Plugins::IBoxAlgorithmDesc** ppBoxAlgorithmDesc) override;

			_IsDerivedFromClass_Final_(TKernelObject<IPluginManager>, OVK_ClassId_Kernel_Plugins_PluginManager)

		protected:

			template <class IPluginObjectT, class IPluginObjectDescT>
			IPluginObjectT* createPluginObjectT(const CIdentifier& rClassIdentifier, const IPluginObjectDescT** ppPluginObjectDescT);

			std::vector<IPluginModule*> m_vPluginModule;
			std::map<Plugins::IPluginObjectDesc*, IPluginModule*> m_vPluginObjectDesc;
			std::map<Plugins::IPluginObjectDesc*, std::vector<Plugins::IPluginObject*>> m_vPluginObject;

			mutable std::mutex m_oMutex;
		};
	} // namespace Kernel
} // namespace OpenViBE

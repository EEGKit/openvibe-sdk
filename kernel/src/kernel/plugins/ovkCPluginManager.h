#pragma once

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginManager : public TKernelObject<IPluginManager>
		{
		public:

			explicit CPluginManager(const IKernelContext& rKernelContext);
			virtual ~CPluginManager();

			virtual bool addPluginsFromFiles(
				const CString& rFileNameWildCard);
			virtual bool registerPluginDesc(
				const Plugins::IPluginObjectDesc& rPluginObjectDesc);

			virtual CIdentifier getNextPluginObjectDescIdentifier(
				const CIdentifier& rPreviousIdentifier) const;
			virtual CIdentifier getNextPluginObjectDescIdentifier(
				const CIdentifier& rPreviousIdentifier,
				const CIdentifier& rBaseClassIdentifier) const;

			virtual bool canCreatePluginObject(
				const CIdentifier& rClassIdentifier);
			virtual const Plugins::IPluginObjectDesc* getPluginObjectDesc(
				const CIdentifier& rClassIdentifier) const;
			virtual const Plugins::IPluginObjectDesc* getPluginObjectDescCreating(
				const CIdentifier& rClassIdentifier) const;

			virtual CIdentifier getPluginObjectHashValue(
				const CIdentifier& rClassIdentifier) const;
			virtual CIdentifier getPluginObjectHashValue(
				const Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc) const;
			virtual bool isPluginObjectFlaggedAsDeprecated(
				const CIdentifier& rClassIdentifier) const;

			virtual Plugins::IPluginObject* createPluginObject(
				const CIdentifier& rClassIdentifier);
			virtual bool releasePluginObject(
				Plugins::IPluginObject* pPluginObject);

			virtual Plugins::IAlgorithm* createAlgorithm(
				const CIdentifier& rClassIdentifier,
				const Plugins::IAlgorithmDesc** ppAlgorithmDesc);
			virtual Plugins::IAlgorithm* createAlgorithm(
				const Plugins::IAlgorithmDesc& rAlgorithmDesc);
			virtual Plugins::IBoxAlgorithm* createBoxAlgorithm(
				const CIdentifier& rClassIdentifier,
				const Plugins::IBoxAlgorithmDesc** ppBoxAlgorithmDesc);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPluginManager, OVK_ClassId_Kernel_Plugins_PluginManager)

		protected:

			template <class IPluginObjectT, class IPluginObjectDescT>
			IPluginObjectT* createPluginObjectT(
				const CIdentifier& rClassIdentifier,
				const IPluginObjectDescT** ppPluginObjectDescT);

		protected:

			std::vector<IPluginModule*> m_vPluginModule;
			std::map<Plugins::IPluginObjectDesc*, IPluginModule*> m_vPluginObjectDesc;
			std::map<Plugins::IPluginObjectDesc*, std::vector<Plugins::IPluginObject*>> m_vPluginObject;

			mutable std::mutex m_oMutex;
		};
	};
};



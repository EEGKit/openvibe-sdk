#pragma once

#include "ovkCBoxAlgorithmLogManager.h"
#include "../ovkTKernelObject.h"
#include <memory>

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CPlayerContext final : public TKernelObject<IPlayerContext>
		{
		public:

			CPlayerContext(const IKernelContext& ctx, CSimulatedBox* pSimulatedBox);
			~CPlayerContext() override { }
			bool sendSignal(const CMessageSignal& messageSignal) override;
			bool sendMessage(const CMessageEvent& messageEvent, const CIdentifier& dstID) override;
			bool sendMessage(const CMessageEvent& messageEvent, const CIdentifier* dstID, const uint32_t nDstID) override;
			uint64_t getCurrentTime() const override;
			uint64_t getCurrentLateness() const override;
			double getCurrentCPUUsage() const override;
			double getCurrentFastForwardMaximumFactor() const override;
			bool stop() override;
			bool pause() override;
			bool play() override;
			bool forward() override;
			EPlayerStatus getStatus() const override;

			//@}
			IAlgorithmManager& getAlgorithmManager() const override { return m_rAlgorithmManager; }
			IConfigurationManager& getConfigurationManager() const override { return m_rConfigurationManager; }
			ILogManager& getLogManager() const override { return m_BoxLogManager; }
			IErrorManager& getErrorManager() const override { return m_rErrorManager; }
			IScenarioManager& getScenarioManager() const override { return m_rScenarioManager; }
			ITypeManager& getTypeManager() const override { return m_rTypeManager; }
			bool canCreatePluginObject(const CIdentifier& pluginID) const override { return m_rPluginManager.canCreatePluginObject(pluginID); }
			Plugins::IPluginObject* createPluginObject(const CIdentifier& pluginID) const override { return m_rPluginManager.createPluginObject(pluginID); }
			bool releasePluginObject(Plugins::IPluginObject* pluginObject) const override { return m_rPluginManager.releasePluginObject(pluginObject); }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerContext>, OVK_ClassId_Kernel_Player_PlayerContext)

		private:

			CSimulatedBox& m_rSimulatedBox;
			IPluginManager& m_rPluginManager;
			IAlgorithmManager& m_rAlgorithmManager;
			IConfigurationManager& m_rConfigurationManager;
			ILogManager& m_rLogManager;
			IErrorManager& m_rErrorManager;
			IScenarioManager& m_rScenarioManager;
			ITypeManager& m_rTypeManager;
			mutable CBoxAlgorithmLogManager m_BoxLogManager;
		};
	} // namespace Kernel
} // namespace OpenViBE

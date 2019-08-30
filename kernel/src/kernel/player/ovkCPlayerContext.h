#pragma once

#include "ovkCBoxAlgorithmLogManager.h"
#include "../ovkTKernelObject.h"
#include <memory>

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CPlayerContext : public TKernelObject<IPlayerContext>
		{
		public:

			CPlayerContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox);
			~CPlayerContext() override;
			bool sendSignal(const CMessageSignal& messageSignal) override;
			bool sendMessage(const CMessageEvent& messageEvent, const CIdentifier& rTargetIdentifier) override;
			bool sendMessage(const CMessageEvent& messageEvent, const CIdentifier* pTargetIdentifier, const uint32_t ui32TargetIdentifierCount) override;
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
			IAlgorithmManager& getAlgorithmManager() const override;
			IConfigurationManager& getConfigurationManager() const override;
			ILogManager& getLogManager() const override;
			IErrorManager& getErrorManager() const override;
			IScenarioManager& getScenarioManager() const override;
			ITypeManager& getTypeManager() const override;
			bool canCreatePluginObject(const CIdentifier& pluginIdentifier) const override;
			Plugins::IPluginObject* createPluginObject(const CIdentifier& pluginIdentifier) const override;
			bool releasePluginObject(Plugins::IPluginObject* pluginObject) const override;

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

#pragma once

#include "../ovkTKernelObject.h"
#include "ovkCScheduler.h"

#include "../ovkCKernelContext.h"

#include <system/ovCChrono.h>

#include <map>
#include <string>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenarioSettingKeywordParserCallback;

		class CPlayer final : public TKernelObject<IPlayer>
		{
		public:

			explicit CPlayer(const IKernelContext& ctx);
			~CPlayer() override;
			bool setScenario(const CIdentifier& scenarioID, const CNameValuePairList* pLocalConfigurationTokens) override;
			IConfigurationManager& getRuntimeConfigurationManager() const override;
			IScenarioManager& getRuntimeScenarioManager() const override;
			CIdentifier getRuntimeScenarioIdentifier() const override;


			bool isHoldingResources() const;
			EPlayerReturnCode initialize() override;
			bool uninitialize() override;
			bool stop() override;
			bool pause() override;
			bool step() override;
			bool play() override;
			bool forward() override;
			EPlayerStatus getStatus() const override;
			bool setFastForwardMaximumFactor(double f64FastForwardFactor) override;
			double getFastForwardMaximumFactor() const override;
			double getCPUUsage() const override;
			bool loop(uint64_t ui64ElapsedTime, uint64_t ui64MaximumTimeToReach) override;
			uint64_t getCurrentSimulatedTime() const override;
			uint64_t getCurrentSimulatedLateness() const;


			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >, OVK_ClassId_Kernel_Player_Player)

		protected:

			CKernelContextBridge m_oKernelContextBridge;
			IConfigurationManager* m_pRuntimeConfigurationManager                          = nullptr;
			IScenarioManager* m_pRuntimeScenarioManager                                    = nullptr;
			CScenarioSettingKeywordParserCallback* m_pScenarioSettingKeywordParserCallback = nullptr;

			CScheduler m_oScheduler;

			uint64_t m_ui64CurrentTimeToReach    = 0;
			uint64_t m_ui64Lateness              = 0;
			uint64_t m_ui64InnerLateness         = 0;
			EPlayerStatus m_eStatus              = PlayerStatus_Stop;
			bool m_bIsInitializing               = false;
			double m_f64FastForwardMaximumFactor = 0;

			std::string m_sScenarioConfigurationFile;
			std::string m_sWorkspaceConfigurationFile;

			// Stores the identifier of the scenario that is being played
			CIdentifier m_oScenarioIdentifier = OV_UndefinedIdentifier;

		private:
			CIdentifier m_oRuntimeScenarioIdentifier = OV_UndefinedIdentifier;

			System::CChrono m_oBenchmarkChrono;
		};
	} // namespace Kernel
} // namespace OpenViBE

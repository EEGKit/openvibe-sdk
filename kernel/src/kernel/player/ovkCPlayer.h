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

		class CPlayer : public TKernelObject<IPlayer>
		{
		public:

			explicit CPlayer(const IKernelContext& rKernelContext);
			virtual ~CPlayer();

			virtual bool setScenario(
				const CIdentifier& rScenarioIdentifier,
				const CNameValuePairList* pLocalConfigurationTokens);

			virtual IConfigurationManager& getRuntimeConfigurationManager() const;
			virtual IScenarioManager& getRuntimeScenarioManager() const;
			virtual CIdentifier getRuntimeScenarioIdentifier() const;


			virtual bool isHoldingResources() const;

			virtual EPlayerReturnCode initialize();
			virtual bool uninitialize();

			virtual bool stop();
			virtual bool pause();
			virtual bool step();
			virtual bool play();
			virtual bool forward();

			virtual EPlayerStatus getStatus() const;

			virtual bool setFastForwardMaximumFactor(double f64FastForwardFactor);
			virtual double getFastForwardMaximumFactor() const;

			virtual double getCPUUsage() const;

			virtual bool loop(
				uint64_t ui64ElapsedTime,
				uint64_t ui64MaximumTimeToReach);

			virtual uint64_t getCurrentSimulatedTime() const;
			virtual uint64_t getCurrentSimulatedLateness() const;


			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >, OVK_ClassId_Kernel_Player_Player);

		protected:

			CKernelContextBridge m_oKernelContextBridge;
			IConfigurationManager* m_pRuntimeConfigurationManager;
			IScenarioManager* m_pRuntimeScenarioManager;
			CScenarioSettingKeywordParserCallback* m_pScenarioSettingKeywordParserCallback;

			CScheduler m_oScheduler;

			uint64_t m_ui64CurrentTimeToReach;
			uint64_t m_ui64Lateness;
			uint64_t m_ui64InnerLateness;
			EPlayerStatus m_eStatus;
			bool m_bIsInitializing;
			double m_f64FastForwardMaximumFactor;

			std::string m_sScenarioConfigurationFile;
			std::string m_sWorkspaceConfigurationFile;

			// Stores the identifier of the scenario that is being played
			CIdentifier m_oScenarioIdentifier;

		private:
			CIdentifier m_oRuntimeScenarioIdentifier;

			System::CChrono m_oBenchmarkChrono;
		};
	};
};



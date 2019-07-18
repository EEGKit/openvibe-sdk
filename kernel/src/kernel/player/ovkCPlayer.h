#ifndef __OpenViBEKernel_Kernel_Player_CPlayer_H__
#define __OpenViBEKernel_Kernel_Player_CPlayer_H__

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

		class CPlayer : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayer>
		{
		public:

			explicit CPlayer(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CPlayer(void);

			virtual bool setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier,
				const OpenViBE::CNameValuePairList* pLocalConfigurationTokens);

			virtual OpenViBE::Kernel::IConfigurationManager& getRuntimeConfigurationManager(void) const;
			virtual OpenViBE::Kernel::IScenarioManager& getRuntimeScenarioManager(void) const;
			virtual OpenViBE::CIdentifier getRuntimeScenarioIdentifier(void) const;


			virtual bool isHoldingResources() const;

			virtual OpenViBE::Kernel::EPlayerReturnCode initialize(void);
			virtual bool uninitialize(void);

			virtual bool stop(void);
			virtual bool pause(void);
			virtual bool step(void);
			virtual bool play(void);
			virtual bool forward(void);

			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const;

			virtual bool setFastForwardMaximumFactor(const double f64FastForwardFactor);
			virtual double getFastForwardMaximumFactor(void) const;

			virtual double getCPUUsage() const;

			virtual bool loop(
				const uint64_t ui64ElapsedTime,
				const uint64_t ui64MaximumTimeToReach);

			virtual uint64_t getCurrentSimulatedTime(void) const;
			virtual uint64_t getCurrentSimulatedLateness(void) const;


			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >, OVK_ClassId_Kernel_Player_Player);

		protected:

			OpenViBE::Kernel::CKernelContextBridge m_oKernelContextBridge;
			OpenViBE::Kernel::IConfigurationManager* m_pRuntimeConfigurationManager;
			OpenViBE::Kernel::IScenarioManager* m_pRuntimeScenarioManager;
			CScenarioSettingKeywordParserCallback* m_pScenarioSettingKeywordParserCallback;

			OpenViBE::Kernel::CScheduler m_oScheduler;

			uint64_t m_ui64CurrentTimeToReach;
			uint64_t m_ui64Lateness;
			uint64_t m_ui64InnerLateness;
			OpenViBE::Kernel::EPlayerStatus m_eStatus;
			bool m_bIsInitializing;
			double m_f64FastForwardMaximumFactor;

			std::string m_sScenarioConfigurationFile;
			std::string m_sWorkspaceConfigurationFile;

			// Stores the identifier of the scenario that is being played
			OpenViBE::CIdentifier m_oScenarioIdentifier;

		private:
			CIdentifier m_oRuntimeScenarioIdentifier;

			System::CChrono m_oBenchmarkChrono;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayer_H__

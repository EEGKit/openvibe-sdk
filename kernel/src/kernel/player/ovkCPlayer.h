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

		class CPlayer : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >
		{
		public:

			CPlayer(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CPlayer(void);

			virtual OpenViBE::boolean setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier,
				const OpenViBE::CNameValuePairList* pLocalConfigurationTokens);

			virtual OpenViBE::Kernel::IConfigurationManager& getRuntimeConfigurationManager(void);
			virtual OpenViBE::Kernel::IScenarioManager& getRuntimeScenarioManager(void);

			virtual OpenViBE::boolean isHoldingResources() const;

			virtual OpenViBE::Kernel::EPlayerReturnCode initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean stop(void);
			virtual OpenViBE::boolean pause(void);
			virtual OpenViBE::boolean step(void);
			virtual OpenViBE::boolean play(void);
			virtual OpenViBE::boolean forward(void);

			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const;

			virtual OpenViBE::boolean setFastForwardMaximumFactor(const OpenViBE::float64 f64FastForwardFactor);
			virtual OpenViBE::float64 getFastForwardMaximumFactor(void) const;

			virtual OpenViBE::float64 getCPUUsage() const;

			virtual OpenViBE::boolean loop(
				const OpenViBE::uint64 ui64ElapsedTime,
				const OpenViBE::uint64 ui64MaximumTimeToReach);

			virtual OpenViBE::uint64 getCurrentSimulatedTime(void) const;
			virtual OpenViBE::uint64 getCurrentSimulatedLateness(void) const;


			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >, OVK_ClassId_Kernel_Player_Player);

		protected:

			OpenViBE::Kernel::CKernelContextBridge m_oKernelContextBridge;
			OpenViBE::Kernel::IConfigurationManager* m_pRuntimeConfigurationManager;
			OpenViBE::Kernel::IScenarioManager* m_pRuntimeScenarioManager;
			CScenarioSettingKeywordParserCallback* m_pScenarioSettingKeywordParserCallback;

			OpenViBE::Kernel::CScheduler m_oScheduler;

			OpenViBE::uint64 m_ui64CurrentTimeToReach;
			OpenViBE::uint64 m_ui64Lateness;
			OpenViBE::uint64 m_ui64InnerLateness;
			OpenViBE::Kernel::EPlayerStatus m_eStatus;
			OpenViBE::boolean m_bIsInitializing;
			OpenViBE::float64 m_f64FastForwardMaximumFactor;

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

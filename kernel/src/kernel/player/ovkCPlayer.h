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

		class CPlayer : public TKernelObject<IPlayer>
		{
		public:

			explicit CPlayer(const IKernelContext& rKernelContext);
			virtual ~CPlayer(void);

			virtual bool setScenario(
				const CIdentifier& rScenarioIdentifier,
				const CNameValuePairList* pLocalConfigurationTokens);

			virtual IConfigurationManager& getRuntimeConfigurationManager(void) const;
			virtual IScenarioManager& getRuntimeScenarioManager(void) const;
			virtual CIdentifier getRuntimeScenarioIdentifier(void) const;


			virtual bool isHoldingResources() const;

			virtual EPlayerReturnCode initialize(void);
			virtual bool uninitialize(void);

			virtual bool stop(void);
			virtual bool pause(void);
			virtual bool step(void);
			virtual bool play(void);
			virtual bool forward(void);

			virtual EPlayerStatus getStatus(void) const;

			virtual bool setFastForwardMaximumFactor(double f64FastForwardFactor);
			virtual double getFastForwardMaximumFactor(void) const;

			virtual double getCPUUsage() const;

			virtual bool loop(
				uint64_t ui64ElapsedTime,
				uint64_t ui64MaximumTimeToReach);

			virtual uint64_t getCurrentSimulatedTime(void) const;
			virtual uint64_t getCurrentSimulatedLateness(void) const;


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

#endif // __OpenViBEKernel_Kernel_Player_CPlayer_H__

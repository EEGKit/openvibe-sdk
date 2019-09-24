#pragma once

#include "../ovkTKernelObject.h"

#include <system/ovCChrono.h>

#include <map>
#include <list>

namespace OpenViBE
{
	namespace Kernel
	{
		enum SchedulerInitializationCode
		{
			SchedulerInitialization_Success,
			SchedulerInitialization_BoxInitializationFailed,
			SchedulerInitialization_Failed
		};

		class CSimulatedBox;
		class CChunk;
		class CPlayer;

		class CScheduler final : public TKernelObject<IKernelObject>
		{
		public:

			CScheduler(const IKernelContext& ctx, CPlayer& player);
			~CScheduler() override;

			bool setScenario(const CIdentifier& scenarioID);
			bool setFrequency(uint64_t frequency);

			bool isHoldingResources() const;

			SchedulerInitializationCode initialize();
			bool uninitialize();
			bool loop();

			bool sendInput(const CChunk& chunk, const CIdentifier& boxId, uint32_t index);
			uint64_t getCurrentTime() const { return m_ui64CurrentTime; }
			uint64_t getCurrentLateness() const;
			uint64_t getFrequency() const { return m_ui64Frequency; }
			uint64_t getStepDuration() const { return m_ui64StepDuration; }
			double getCPUUsage() const { return (const_cast<System::CChrono&>(m_oBenchmarkChrono)).getStepInPercentage(); }
			double getFastForwardMaximumFactor() const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >, OVK_ClassId_Kernel_Player_Scheduler)

			CPlayer& getPlayer() const { return m_rPlayer; }

		protected:

			CPlayer& m_rPlayer;
			CIdentifier m_oScenarioIdentifier = OV_UndefinedIdentifier;
			IScenario* m_pScenario            = nullptr;
			uint64_t m_ui64Steps              = 0;
			uint64_t m_ui64Frequency          = 0;
			uint64_t m_ui64StepDuration       = 0;
			uint64_t m_ui64CurrentTime        = 0;

			std::map<std::pair<int, CIdentifier>, CSimulatedBox*> m_vSimulatedBox;
			std::map<CIdentifier, System::CChrono> m_vSimulatedBoxChrono;
			std::map<CIdentifier, std::map<uint32_t, std::list<CChunk>>> m_vSimulatedBoxInput;

		private:

			void handleException(const CSimulatedBox* box, const char* errorHint, const std::exception& exception);
			bool processBox(CSimulatedBox* simulatedBox, const CIdentifier& boxID);
			bool flattenScenario();
			System::CChrono m_oBenchmarkChrono;
		};
	} // namespace Kernel
} // namespace OpenViBE

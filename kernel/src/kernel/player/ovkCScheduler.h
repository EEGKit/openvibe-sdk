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

		class CScheduler : public TKernelObject<IKernelObject>
		{
		public:

			CScheduler(const IKernelContext& rKernelContext, CPlayer& rPlayer);
			~CScheduler() override;

			virtual bool setScenario(const CIdentifier& rScenarioIdentifier);
			virtual bool setFrequency(uint64_t ui64Frequency);

			virtual bool isHoldingResources() const;

			virtual SchedulerInitializationCode initialize();
			virtual bool uninitialize();
			virtual bool loop();

			virtual bool sendInput(const CChunk& rChunk, const CIdentifier& rBoxIdentifier, uint32_t ui32InputIndex);
			virtual uint64_t getCurrentTime() const;
			virtual uint64_t getCurrentLateness() const;
			virtual uint64_t getFrequency() const;
			virtual uint64_t getStepDuration() const;
			virtual double getCPUUsage() const;
			virtual double getFastForwardMaximumFactor() const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >, OVK_ClassId_Kernel_Player_Scheduler)

			CPlayer& getPlayer() { return m_rPlayer; }

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
			bool processBox(CSimulatedBox* simulatedBox, const CIdentifier& boxIdentifier);
			bool flattenScenario();
			System::CChrono m_oBenchmarkChrono;
		};
	} // namespace Kernel
} // namespace OpenViBE

#ifndef __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__
#define __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__

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

		class CScheduler : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IKernelObject>
		{
		public:

			CScheduler(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CPlayer& rPlayer);
			virtual ~CScheduler(void);

			virtual bool setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);
			virtual bool setFrequency(
				const uint64_t ui64Frequency);

			virtual bool isHoldingResources() const;

			virtual SchedulerInitializationCode initialize(void);
			virtual bool uninitialize(void);
			virtual bool loop(void);

			virtual bool sendInput(const OpenViBE::Kernel::CChunk& rChunk, const OpenViBE::CIdentifier& rBoxIdentifier, const uint32_t ui32InputIndex);
			virtual uint64_t getCurrentTime(void) const;
			virtual uint64_t getCurrentLateness(void) const;
			virtual uint64_t getFrequency(void) const;
			virtual uint64_t getStepDuration(void) const;
			virtual double getCPUUsage(void) const;
			virtual double getFastForwardMaximumFactor(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >, OVK_ClassId_Kernel_Player_Scheduler);

			CPlayer& getPlayer(void)
			{
				return m_rPlayer;
			}

		protected:

			OpenViBE::Kernel::CPlayer& m_rPlayer;
			OpenViBE::CIdentifier m_oScenarioIdentifier;
			OpenViBE::Kernel::IScenario* m_pScenario;
			uint64_t m_ui64Steps;
			uint64_t m_ui64Frequency;
			uint64_t m_ui64StepDuration;
			uint64_t m_ui64CurrentTime;

			std::map<std::pair<int32_t, OpenViBE::CIdentifier>, OpenViBE::Kernel::CSimulatedBox*> m_vSimulatedBox;
			std::map<OpenViBE::CIdentifier, System::CChrono> m_vSimulatedBoxChrono;
			std::map<OpenViBE::CIdentifier, std::map<uint32_t, std::list<OpenViBE::Kernel::CChunk>>> m_vSimulatedBoxInput;

		private:

			void handleException(const CSimulatedBox* box, const char* errorHint, const std::exception& exception);
			bool processBox(CSimulatedBox* simulatedBox, const OpenViBE::CIdentifier& boxIdentifier);
			bool flattenScenario(void);
			System::CChrono m_oBenchmarkChrono;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__

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
				const OpenViBE::uint64 ui64Frequency);

			virtual bool isHoldingResources() const;

			virtual SchedulerInitializationCode initialize(void);
			virtual bool uninitialize(void);
			virtual bool loop(void);

			virtual bool sendInput(const OpenViBE::Kernel::CChunk& rChunk, const OpenViBE::CIdentifier& rBoxIdentifier, const OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::uint64 getCurrentTime(void) const;
			virtual OpenViBE::uint64 getCurrentLateness(void) const;
			virtual OpenViBE::uint64 getFrequency(void) const;
			virtual OpenViBE::uint64 getStepDuration(void) const;
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
			OpenViBE::uint64 m_ui64Steps;
			OpenViBE::uint64 m_ui64Frequency;
			OpenViBE::uint64 m_ui64StepDuration;
			OpenViBE::uint64 m_ui64CurrentTime;

			std::map<std::pair<OpenViBE::int32, OpenViBE::CIdentifier>, OpenViBE::Kernel::CSimulatedBox*> m_vSimulatedBox;
			std::map<OpenViBE::CIdentifier, System::CChrono> m_vSimulatedBoxChrono;
			std::map<OpenViBE::CIdentifier, std::map<OpenViBE::uint32, std::list<OpenViBE::Kernel::CChunk>>> m_vSimulatedBoxInput;

		private:

			void handleException(const CSimulatedBox* box, const char* errorHint, const std::exception& exception);
			bool processBox(CSimulatedBox* simulatedBox, const OpenViBE::CIdentifier& boxIdentifier);
			bool flattenScenario(void);
			System::CChrono m_oBenchmarkChrono;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__

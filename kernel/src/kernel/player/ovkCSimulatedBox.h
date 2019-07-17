#ifndef __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__
#define __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__

#include "../ovkTKernelObject.h"
#include "ovkCBuffer.h"

#include <system/ovCChrono.h>
#include <vector>
#include <string>
#include <deque>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScheduler;

		class CChunk
		{
		public:

			CChunk(void)
				: m_ui64StartTime(0)
				  , m_ui64EndTime(0)
				  , m_bIsDeprecated(false) { }

			explicit CChunk(const OpenViBE::Kernel::CChunk& rChunk)
				: m_oBuffer(rChunk.m_oBuffer)
				  , m_ui64StartTime(rChunk.m_ui64StartTime)
				  , m_ui64EndTime(rChunk.m_ui64EndTime)
				  , m_bIsDeprecated(false) { }

			const OpenViBE::Kernel::CBuffer& getBuffer(void) const
			{
				return m_oBuffer;
			}

			OpenViBE::uint64 getStartTime(void) const
			{
				return m_ui64StartTime;
			}

			OpenViBE::uint64 getEndTime(void) const
			{
				return m_ui64EndTime;
			}

			bool isDeprecated(void) const
			{
				return m_bIsDeprecated;
			}

			OpenViBE::Kernel::CBuffer& getBuffer(void)
			{
				return m_oBuffer;
			}

			bool setStartTime(OpenViBE::uint64 ui64StartTime)
			{
				m_ui64StartTime = ui64StartTime;
				return true;
			}

			bool setEndTime(OpenViBE::uint64 ui64EndTime)
			{
				m_ui64EndTime = ui64EndTime;
				return true;
			}

			bool markAsDeprecated(bool bIsDeprecated)
			{
				m_bIsDeprecated = bIsDeprecated;
				return true;
			}

		protected:

			OpenViBE::Kernel::CBuffer m_oBuffer;
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;
			bool m_bIsDeprecated;
		};

		class CSimulatedBox : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxIO>
		{
		public:

			CSimulatedBox(
				const OpenViBE::Kernel::IKernelContext& rKernelContext,
				OpenViBE::Kernel::CScheduler& rScheduler);

			virtual ~CSimulatedBox();

			virtual bool setScenarioIdentifier(
				const OpenViBE::CIdentifier& rScenarioIdentifier);

			virtual bool getBoxIdentifier(
				OpenViBE::CIdentifier& rBoxIdentifier) const;

			virtual bool setBoxIdentifier(
				const OpenViBE::CIdentifier& rBoxIdentifier);

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			virtual bool processClock(void);
			virtual bool processInput(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::Kernel::CChunk& rChunk);
			virtual bool process(void);
			virtual bool isReadyToProcess(void) const;

			virtual OpenViBE::CString getName(void) const;
			virtual const OpenViBE::Kernel::IScenario& getScenario(void) const;

			/** \name IBoxIO inputs handling */
			//@{
			virtual OpenViBE::uint32 getInputChunkCount(
				const OpenViBE::uint32 ui32InputIndex) const;
			virtual bool getInputChunk(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex,
				OpenViBE::uint64& rStartTime,
				OpenViBE::uint64& rEndTime,
				OpenViBE::uint64& rChunkSize,
				const OpenViBE::uint8*& rpChunkBuffer) const;
			virtual const OpenViBE::IMemoryBuffer* getInputChunk(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex) const;
			virtual OpenViBE::uint64 getInputChunkStartTime(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex) const;
			virtual OpenViBE::uint64 getInputChunkEndTime(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex) const;
			virtual bool markInputAsDeprecated(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::uint32 ui32ChunkIndex);
			//@}

			/** \name IBoxIO outputs handling */
			//@{
			virtual OpenViBE::uint64 getOutputChunkSize(
				const OpenViBE::uint32 ui32OutputIndex) const;
			virtual bool setOutputChunkSize(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::uint64 ui64Size,
				const bool bDiscard = true);
			virtual OpenViBE::uint8* getOutputChunkBuffer(
				const OpenViBE::uint32 ui32OutputIndex);
			virtual bool appendOutputChunkData(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::uint8* pBuffer,
				const OpenViBE::uint64 ui64BufferSize);
			virtual OpenViBE::IMemoryBuffer* getOutputChunk(
				const OpenViBE::uint32 ui32OutputIndex);
			virtual bool markOutputAsReadyToSend(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::uint64 ui64StartTime,
				const OpenViBE::uint64 ui64EndTime);
			//@}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxIO >, OVK_ClassId_Kernel_Player_SimulatedBox);

			OpenViBE::Kernel::CScheduler& getScheduler(void)
			{
				return m_rScheduler;
			}

		protected:

			bool m_bReadyToProcess;
			bool m_bChunkConsistencyChecking;
			OpenViBE::Kernel::ELogLevel m_eChunkConsistencyCheckingLogLevel;

			OpenViBE::Plugins::IBoxAlgorithm* m_pBoxAlgorithm;
			const OpenViBE::Kernel::IScenario* m_pScenario;
			const OpenViBE::Kernel::IBox* m_pBox;
			OpenViBE::Kernel::CScheduler& m_rScheduler;

			OpenViBE::uint64 m_ui64LastClockActivationDate;
			OpenViBE::uint64 m_ui64ClockFrequency;
			OpenViBE::uint64 m_ui64ClockActivationStep;

		public:

			std::vector<std::deque<OpenViBE::Kernel::CChunk>> m_vInput;
			std::vector<std::deque<OpenViBE::Kernel::CChunk>> m_vOutput;
			std::vector<OpenViBE::Kernel::CChunk> m_vCurrentOutput;
			std::vector<OpenViBE::uint64> m_vLastOutputStartTime;
			std::vector<OpenViBE::uint64> m_vLastOutputEndTime;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__

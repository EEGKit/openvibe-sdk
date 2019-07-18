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

			uint64_t getStartTime(void) const
			{
				return m_ui64StartTime;
			}

			uint64_t getEndTime(void) const
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

			bool setStartTime(uint64_t ui64StartTime)
			{
				m_ui64StartTime = ui64StartTime;
				return true;
			}

			bool setEndTime(uint64_t ui64EndTime)
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
			uint64_t m_ui64StartTime;
			uint64_t m_ui64EndTime;
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
			virtual bool processInput(const uint32_t ui32InputIndex, const OpenViBE::Kernel::CChunk& rChunk);
			virtual bool process(void);
			virtual bool isReadyToProcess(void) const;

			virtual OpenViBE::CString getName(void) const;
			virtual const OpenViBE::Kernel::IScenario& getScenario(void) const;

			/** \name IBoxIO inputs handling */
			//@{
			virtual uint32_t getInputChunkCount(
				const uint32_t ui32InputIndex) const;
			virtual bool getInputChunk(
				const uint32_t ui32InputIndex,
				const uint32_t ui32ChunkIndex,
				uint64_t& rStartTime,
				uint64_t& rEndTime,
				uint64_t& rChunkSize,
				const uint8_t*& rpChunkBuffer) const;
			virtual const OpenViBE::IMemoryBuffer* getInputChunk(
				const uint32_t ui32InputIndex,
				const uint32_t ui32ChunkIndex) const;
			virtual uint64_t getInputChunkStartTime(
				const uint32_t ui32InputIndex,
				const uint32_t ui32ChunkIndex) const;
			virtual uint64_t getInputChunkEndTime(
				const uint32_t ui32InputIndex,
				const uint32_t ui32ChunkIndex) const;
			virtual bool markInputAsDeprecated(
				const uint32_t ui32InputIndex,
				const uint32_t ui32ChunkIndex);
			//@}

			/** \name IBoxIO outputs handling */
			//@{
			virtual uint64_t getOutputChunkSize(
				const uint32_t ui32OutputIndex) const;
			virtual bool setOutputChunkSize(
				const uint32_t ui32OutputIndex,
				const uint64_t ui64Size,
				const bool bDiscard = true);
			virtual uint8_t* getOutputChunkBuffer(
				const uint32_t ui32OutputIndex);
			virtual bool appendOutputChunkData(
				const uint32_t ui32OutputIndex,
				const uint8_t* pBuffer,
				const uint64_t ui64BufferSize);
			virtual OpenViBE::IMemoryBuffer* getOutputChunk(
				const uint32_t ui32OutputIndex);
			virtual bool markOutputAsReadyToSend(
				const uint32_t ui32OutputIndex,
				const uint64_t ui64StartTime,
				const uint64_t ui64EndTime);
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

			uint64_t m_ui64LastClockActivationDate;
			uint64_t m_ui64ClockFrequency;
			uint64_t m_ui64ClockActivationStep;

		public:

			std::vector<std::deque<OpenViBE::Kernel::CChunk>> m_vInput;
			std::vector<std::deque<OpenViBE::Kernel::CChunk>> m_vOutput;
			std::vector<OpenViBE::Kernel::CChunk> m_vCurrentOutput;
			std::vector<uint64_t> m_vLastOutputStartTime;
			std::vector<uint64_t> m_vLastOutputEndTime;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CSimulatedBox_H__

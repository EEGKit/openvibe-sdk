#pragma once

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

			CChunk()
				: m_ui64StartTime(0)
				  , m_ui64EndTime(0)
				  , m_bIsDeprecated(false) { }

			explicit CChunk(const CChunk& rChunk)
				: m_oBuffer(rChunk.m_oBuffer)
				  , m_ui64StartTime(rChunk.m_ui64StartTime)
				  , m_ui64EndTime(rChunk.m_ui64EndTime)
				  , m_bIsDeprecated(false) { }

			const CBuffer& getBuffer() const
			{
				return m_oBuffer;
			}

			uint64_t getStartTime() const
			{
				return m_ui64StartTime;
			}

			uint64_t getEndTime() const
			{
				return m_ui64EndTime;
			}

			bool isDeprecated() const
			{
				return m_bIsDeprecated;
			}

			CBuffer& getBuffer()
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

			CBuffer m_oBuffer;
			uint64_t m_ui64StartTime;
			uint64_t m_ui64EndTime;
			bool m_bIsDeprecated;
		};

		class CSimulatedBox : public TKernelObject<IBoxIO>
		{
		public:

			CSimulatedBox(const IKernelContext& rKernelContext, CScheduler& rScheduler);

			virtual ~CSimulatedBox();

			virtual bool setScenarioIdentifier(const CIdentifier& rScenarioIdentifier);

			virtual bool getBoxIdentifier(CIdentifier& rBoxIdentifier) const;

			virtual bool setBoxIdentifier(const CIdentifier& rBoxIdentifier);

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool processClock();
			virtual bool processInput(const uint32_t ui32InputIndex, const CChunk& rChunk);
			virtual bool process();
			virtual bool isReadyToProcess() const;

			virtual CString getName() const;
			virtual const IScenario& getScenario() const;

			/** \name IBoxIO inputs handling */
			//@{
			virtual uint32_t getInputChunkCount(const uint32_t ui32InputIndex) const;
			virtual bool getInputChunk(const uint32_t ui32InputIndex, const uint32_t ui32ChunkIndex, uint64_t& rStartTime, uint64_t& rEndTime, uint64_t& rChunkSize, const uint8_t*& rpChunkBuffer) const;
			virtual const IMemoryBuffer* getInputChunk(const uint32_t ui32InputIndex, const uint32_t ui32ChunkIndex) const;
			virtual uint64_t getInputChunkStartTime(const uint32_t ui32InputIndex, const uint32_t ui32ChunkIndex) const;
			virtual uint64_t getInputChunkEndTime(const uint32_t ui32InputIndex, const uint32_t ui32ChunkIndex) const;
			virtual bool markInputAsDeprecated(const uint32_t ui32InputIndex, const uint32_t ui32ChunkIndex);
			//@}

			/** \name IBoxIO outputs handling */
			//@{
			virtual uint64_t getOutputChunkSize(const uint32_t ui32OutputIndex) const;
			virtual bool setOutputChunkSize(const uint32_t ui32OutputIndex, const uint64_t ui64Size, bool bDiscard = true);
			virtual uint8_t* getOutputChunkBuffer(uint32_t ui32OutputIndex);
			virtual bool appendOutputChunkData(const uint32_t ui32OutputIndex, const uint8_t* pBuffer,const uint64_t ui64BufferSize);
			virtual IMemoryBuffer* getOutputChunk(const uint32_t ui32OutputIndex);
			virtual bool markOutputAsReadyToSend(const uint32_t ui32OutputIndex, const uint64_t ui64StartTime, const uint64_t ui64EndTime);
			//@}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxIO >, OVK_ClassId_Kernel_Player_SimulatedBox)

			CScheduler& getScheduler() { return m_rScheduler; }

		protected:

			bool m_bReadyToProcess;
			bool m_bChunkConsistencyChecking;
			ELogLevel m_eChunkConsistencyCheckingLogLevel;

			Plugins::IBoxAlgorithm* m_pBoxAlgorithm;
			const IScenario* m_pScenario;
			const IBox* m_pBox;
			CScheduler& m_rScheduler;

			uint64_t m_ui64LastClockActivationDate;
			uint64_t m_ui64ClockFrequency;
			uint64_t m_ui64ClockActivationStep;

		public:

			std::vector<std::deque<CChunk>> m_vInput;
			std::vector<std::deque<CChunk>> m_vOutput;
			std::vector<CChunk> m_vCurrentOutput;
			std::vector<uint64_t> m_vLastOutputStartTime;
			std::vector<uint64_t> m_vLastOutputEndTime;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



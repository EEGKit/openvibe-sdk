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

			CChunk() { }

			explicit CChunk(const CChunk& rChunk) : m_oBuffer(rChunk.m_oBuffer), m_startTime(rChunk.m_startTime), m_endTime(rChunk.m_endTime) { }

			const CBuffer& getBuffer() const { return m_oBuffer; }

			uint64_t getStartTime() const { return m_startTime; }

			uint64_t getEndTime() const { return m_endTime; }

			bool isDeprecated() const { return m_bIsDeprecated; }

			CBuffer& getBuffer() { return m_oBuffer; }

			bool setStartTime(uint64_t startTime)
			{
				m_startTime = startTime;
				return true;
			}

			bool setEndTime(uint64_t endTime)
			{
				m_endTime = endTime;
				return true;
			}

			bool markAsDeprecated(bool isDeprecated)
			{
				m_bIsDeprecated = isDeprecated;
				return true;
			}

		protected:

			CBuffer m_oBuffer;
			uint64_t m_startTime = 0;
			uint64_t m_endTime   = 0;
			bool m_bIsDeprecated     = false;
		};

		class CSimulatedBox final : public TKernelObject<IBoxIO>
		{
		public:

			CSimulatedBox(const IKernelContext& ctx, CScheduler& rScheduler);
			~CSimulatedBox() override;

			bool setScenarioIdentifier(const CIdentifier& scenarioID);
			bool getBoxIdentifier(CIdentifier& boxId) const;
			bool setBoxIdentifier(const CIdentifier& boxId);

			bool initialize();
			bool uninitialize();

			bool processClock();
			bool processInput(const uint32_t index, const CChunk& rChunk);
			bool process();
			bool isReadyToProcess() const;

			CString getName() const;
			const IScenario& getScenario() const;

			/** \name IBoxIO inputs handling */
			//@{
			uint32_t getInputChunkCount(const uint32_t index) const override;
			bool getInputChunk(const uint32_t inputIdx, const uint32_t chunkIdx, uint64_t& startTime, uint64_t& endTime, uint64_t& size, const uint8_t*& buffer) const override;
			const IMemoryBuffer* getInputChunk(const uint32_t inputIdx, const uint32_t chunkIdx) const override;
			uint64_t getInputChunkStartTime(const uint32_t inputIdx, const uint32_t chunkIdx) const override;
			uint64_t getInputChunkEndTime(const uint32_t inputIdx, const uint32_t chunkIdx) const override;
			bool markInputAsDeprecated(const uint32_t inputIdx, const uint32_t chunkIdx) override;
			//@}

			/** \name IBoxIO outputs handling */
			//@{
			uint64_t getOutputChunkSize(const uint32_t outputIdx) const override;
			bool setOutputChunkSize(const uint32_t outputIdx, const uint64_t size, const bool discard = true) override;
			uint8_t* getOutputChunkBuffer(const uint32_t outputIdx) override;
			bool appendOutputChunkData(const uint32_t outputIdx, const uint8_t* buffer, const uint64_t size) override;
			IMemoryBuffer* getOutputChunk(const uint32_t outputIdx) override;
			bool markOutputAsReadyToSend(const uint32_t outputIdx, const uint64_t startTime, const uint64_t endTime) override;
			//@}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxIO >, OVK_ClassId_Kernel_Player_SimulatedBox)

			CScheduler& getScheduler() const { return m_rScheduler; }

		protected:

			bool m_bReadyToProcess                        = false;
			bool m_bChunkConsistencyChecking              = false;
			ELogLevel m_eChunkConsistencyCheckingLogLevel = LogLevel_Warning;

			Plugins::IBoxAlgorithm* m_pBoxAlgorithm = nullptr;
			const IScenario* m_scenario            = nullptr;
			const IBox* m_pBox                      = nullptr;
			CScheduler& m_rScheduler;

			uint64_t m_ui64LastClockActivationDate = 0;
			uint64_t m_ui64ClockFrequency          = 0;
			uint64_t m_ui64ClockActivationStep     = 0;

		public:

			std::vector<std::deque<CChunk>> m_vInput;
			std::vector<std::deque<CChunk>> m_vOutput;
			std::vector<CChunk> m_vCurrentOutput;
			std::vector<uint64_t> m_vLastOutputStartTime;
			std::vector<uint64_t> m_vLastOutputEndTime;
		};
	} // namespace Kernel
} // namespace OpenViBE

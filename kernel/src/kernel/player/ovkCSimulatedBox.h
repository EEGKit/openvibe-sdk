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

			bool isDeprecated() const { return m_isDeprecated; }

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
				m_isDeprecated = isDeprecated;
				return true;
			}

		protected:

			CBuffer m_oBuffer;
			uint64_t m_startTime = 0;
			uint64_t m_endTime   = 0;
			bool m_isDeprecated  = false;
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
			bool processInput(const size_t index, const CChunk& rChunk);
			bool process();
			bool isReadyToProcess() const;

			CString getName() const;
			const IScenario& getScenario() const;

			/** \name IBoxIO inputs handling */
			//@{
			size_t getInputChunkCount(const size_t index) const override;
			bool getInputChunk(const size_t inputIdx, const size_t chunkIdx, uint64_t& startTime, uint64_t& endTime, size_t& size,
							   const uint8_t*& buffer) const override;
			const IMemoryBuffer* getInputChunk(const size_t inputIdx, const size_t chunkIdx) const override;
			uint64_t getInputChunkStartTime(const size_t inputIdx, const size_t chunkIdx) const override;
			uint64_t getInputChunkEndTime(const size_t inputIdx, const size_t chunkIdx) const override;
			bool markInputAsDeprecated(const size_t inputIdx, const size_t chunkIdx) override;
			//@}

			/** \name IBoxIO outputs handling */
			//@{
			size_t getOutputChunkSize(const size_t outputIdx) const override;
			bool setOutputChunkSize(const size_t outputIdx, const size_t size, const bool discard = true) override;
			uint8_t* getOutputChunkBuffer(const size_t outputIdx) override;
			bool appendOutputChunkData(const size_t outputIdx, const uint8_t* buffer, const size_t size) override;
			IMemoryBuffer* getOutputChunk(const size_t outputIdx) override;
			bool markOutputAsReadyToSend(const size_t outputIdx, const uint64_t startTime, const uint64_t endTime) override;
			//@}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxIO >, OVK_ClassId_Kernel_Player_SimulatedBox)

			CScheduler& getScheduler() const { return m_rScheduler; }

		protected:

			bool m_bReadyToProcess                        = false;
			bool m_bChunkConsistencyChecking              = false;
			ELogLevel m_eChunkConsistencyCheckingLogLevel = LogLevel_Warning;

			Plugins::IBoxAlgorithm* m_boxAlgorithm = nullptr;
			const IScenario* m_scenario            = nullptr;
			const IBox* m_box                      = nullptr;
			CScheduler& m_rScheduler;

			uint64_t m_lastClockActivationDate = 0;
			uint64_t m_clockFrequency          = 0;
			uint64_t m_clockActivationStep     = 0;

		public:

			std::vector<std::deque<CChunk>> m_inputs;
			std::vector<std::deque<CChunk>> m_outputs;
			std::vector<CChunk> m_vCurrentOutput;
			std::vector<uint64_t> m_vLastOutputStartTime;
			std::vector<uint64_t> m_vLastOutputEndTime;
		};
	} // namespace Kernel
} // namespace OpenViBE

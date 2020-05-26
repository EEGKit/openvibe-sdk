#pragma once

#include "../ovkTKernelObject.h"
#include "ovkCBuffer.h"

#include <system/ovCChrono.h>
#include <vector>
#include <string>
#include <deque>

namespace OpenViBE {
namespace Kernel {
class CScheduler;

class CChunk
{
public:

	CChunk() { }

	CChunk(const CChunk& chunk) : m_buffer(chunk.m_buffer), m_startTime(chunk.m_startTime), m_endTime(chunk.m_endTime) { }

	const CBuffer& getBuffer() const { return m_buffer; }
	CTime getStartTime() const { return m_startTime; }
	CTime getEndTime() const { return m_endTime; }
	bool isDeprecated() const { return m_isDeprecated; }
	CBuffer& getBuffer() { return m_buffer; }

	bool setStartTime(const CTime startTime)
	{
		m_startTime = startTime;
		return true;
	}

	bool setEndTime(const CTime endTime)
	{
		m_endTime = endTime;
		return true;
	}

	bool markAsDeprecated(const bool isDeprecated)
	{
		m_isDeprecated = isDeprecated;
		return true;
	}

protected:

	CBuffer m_buffer;
	CTime m_startTime = 0;
	CTime m_endTime   = 0;
	bool m_isDeprecated  = false;
};

class CSimulatedBox final : public TKernelObject<IBoxIO>
{
public:

	CSimulatedBox(const IKernelContext& ctx, CScheduler& scheduler);
	~CSimulatedBox() override;

	bool setScenarioIdentifier(const CIdentifier& scenarioID);
	bool getBoxIdentifier(CIdentifier& boxId) const;
	bool setBoxIdentifier(const CIdentifier& boxId);

	bool initialize();
	bool uninitialize();

	bool processClock();
	bool processInput(const size_t index, const CChunk& chunk);
	bool process();
	bool isReadyToProcess() const;

	CString getName() const;
	const IScenario& getScenario() const;

	/** \name IBoxIO inputs handling */
	//@{
	size_t getInputChunkCount(const size_t index) const override;
	bool getInputChunk(const size_t inputIdx, const size_t chunkIdx, CTime& startTime, CTime& endTime, size_t& size, const uint8_t*& buffer) const override;
	const IMemoryBuffer* getInputChunk(const size_t inputIdx, const size_t chunkIdx) const override;
	CTime getInputChunkStartTime(const size_t inputIdx, const size_t chunkIdx) const override;
	CTime getInputChunkEndTime(const size_t inputIdx, const size_t chunkIdx) const override;
	bool markInputAsDeprecated(const size_t inputIdx, const size_t chunkIdx) override;
	//@}

	/** \name IBoxIO outputs handling */
	//@{
	size_t getOutputChunkSize(const size_t outputIdx) const override;
	bool setOutputChunkSize(const size_t outputIdx, const size_t size, const bool discard = true) override;
	uint8_t* getOutputChunkBuffer(const size_t outputIdx) override;
	bool appendOutputChunkData(const size_t outputIdx, const uint8_t* buffer, const size_t size) override;
	IMemoryBuffer* getOutputChunk(const size_t outputIdx) override;
	bool markOutputAsReadyToSend(const size_t outputIdx, const CTime startTime, const CTime endTime) override;
	//@}

	_IsDerivedFromClass_Final_(TKernelObject<IBoxIO>, OVK_ClassId_Kernel_Player_SimulatedBox)

	CScheduler& getScheduler() const { return m_scheduler; }

protected:

	bool m_readyToProcess                        = false;
	bool m_chunkConsistencyChecking              = false;
	ELogLevel m_chunkConsistencyCheckingLogLevel = LogLevel_Warning;

	Plugins::IBoxAlgorithm* m_boxAlgorithm = nullptr;
	const IScenario* m_scenario            = nullptr;
	const IBox* m_box                      = nullptr;
	CScheduler& m_scheduler;

	CTime m_lastClockActivationDate;
	CTime m_clockFrequency;
	CTime m_clockActivationStep;

public:

	std::vector<std::deque<CChunk>> m_Inputs;
	std::vector<std::deque<CChunk>> m_Outputs;
	std::vector<CChunk> m_CurrentOutputs;
	std::vector<CTime> m_LastOutputStartTimes;
	std::vector<CTime> m_LastOutputEndTimes;
};
} // namespace Kernel
} // namespace OpenViBE

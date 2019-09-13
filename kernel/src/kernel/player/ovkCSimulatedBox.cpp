// #if defined __MY_COMPILE_ALL

#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"
#include "ovkCBoxAlgorithmContext.h"
#include "ovkCMessageClock.h"
#include "ovkCMessageEvent.h"
#include "ovkCMessageSignal.h"

#include <openvibe/ovITimeArithmetics.h>

#include <cstdlib>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

// ________________________________________________________________________________________________________________
//

#define OV_IncorrectTime 0xffffffffffffffffULL

CSimulatedBox::CSimulatedBox(const IKernelContext& ctx, CScheduler& rScheduler)
	: TKernelObject<IBoxIO>(ctx), m_rScheduler(rScheduler), m_ui64LastClockActivationDate(OV_IncorrectTime) {}

CSimulatedBox::~CSimulatedBox() {}

bool CSimulatedBox::setScenarioIdentifier(const CIdentifier& scenarioID)
{
	OV_ERROR_UNLESS_KRF(m_rScheduler.getPlayer().getRuntimeScenarioManager().isScenario(scenarioID),
						"Scenario with identifier " << scenarioID.toString() << " does not exist",
						ErrorType::ResourceNotFound);

	m_pScenario = &m_rScheduler.getPlayer().getRuntimeScenarioManager().getScenario(scenarioID);
	return true;
}

bool CSimulatedBox::getBoxIdentifier(CIdentifier& boxId) const
{
	OV_ERROR_UNLESS_KRF(m_pBox, "Simulated box not initialized", ErrorType::BadCall);

	boxId = m_pBox->getIdentifier();
	return true;
}

bool CSimulatedBox::setBoxIdentifier(const CIdentifier& boxId)
{
	OV_ERROR_UNLESS_KRF(m_pScenario, "No scenario set", ErrorType::BadCall);

	m_pBox = m_pScenario->getBoxDetails(boxId);
	return m_pBox != nullptr;
}

bool CSimulatedBox::initialize()
{
	OV_ERROR_UNLESS_KRF(m_pBox, "Simulated box not initialized", ErrorType::BadCall);
	OV_ERROR_UNLESS_KRF(m_pScenario, "No scenario set", ErrorType::BadCall);

	m_bChunkConsistencyChecking = this->getConfigurationManager().expandAsBoolean("${Kernel_CheckChunkConsistency}", true);
	m_vInput.resize(m_pBox->getInputCount());
	m_vOutput.resize(m_pBox->getOutputCount());
	m_vCurrentOutput.resize(m_pBox->getOutputCount());
	m_vLastOutputStartTime.resize(m_pBox->getOutputCount(), 0);
	m_vLastOutputEndTime.resize(m_pBox->getOutputCount(), 0);

	m_ui64LastClockActivationDate = OV_IncorrectTime;
	m_ui64ClockFrequency          = 0;
	m_ui64ClockActivationStep     = 0;

	m_pBoxAlgorithm = getPluginManager().createBoxAlgorithm(m_pBox->getAlgorithmClassIdentifier(), nullptr);

	OV_ERROR_UNLESS_KRF(m_pBoxAlgorithm, "Could not create box algorithm with class id " << m_pBox->getAlgorithmClassIdentifier().toString(),
						ErrorType::BadResourceCreation);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(m_pBoxAlgorithm->initialize(l_oBoxAlgorithmContext), "Box algorithm <" << m_pBox->getName() << "> initialization failed",
								ErrorType::Internal);
		}
	}

	return true;
}

bool CSimulatedBox::uninitialize()
{
	if (!m_pBoxAlgorithm) { return true; }

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(m_pBoxAlgorithm->uninitialize(l_oBoxAlgorithmContext), "Box algorithm <" << m_pBox->getName() << "> uninitialization failed",
								ErrorType::Internal);
		}
	}

	getPluginManager().releasePluginObject(m_pBoxAlgorithm);
	m_pBoxAlgorithm = nullptr;

	return true;
}

bool CSimulatedBox::processClock()
{
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			uint64_t l_ui64NewClockFrequency = m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
			if (l_ui64NewClockFrequency == 0)
			{
				m_ui64ClockActivationStep     = OV_IncorrectTime;
				m_ui64LastClockActivationDate = OV_IncorrectTime;
			}
			else
			{
				OV_ERROR_UNLESS_KRF(l_ui64NewClockFrequency <= m_rScheduler.getFrequency()<<32,
									"Box " << m_pBox->getName() << " requested higher clock frequency ("
									<< l_ui64NewClockFrequency << " == " << ITimeArithmetics::timeToSeconds(l_ui64NewClockFrequency) << "hz) " <<
									"than what the scheduler can handle ("
									<< (m_rScheduler.getFrequency()<<32) << " == " << ITimeArithmetics::timeToSeconds(m_rScheduler.getFrequency()<<32) << "hz)",
									ErrorType::BadConfig);

				// note: 1LL should be left shifted 64 bits but this
				//       would result in an integer over shift (the one
				//       would exit). Thus the left shift of 63 bits
				//       and the left shift of 1 bit after the division
				m_ui64ClockActivationStep = ((1ULL << 63) / l_ui64NewClockFrequency) << 1;
			}
			m_ui64ClockFrequency = l_ui64NewClockFrequency;
		}
	}

	if ((m_ui64ClockFrequency != 0) && (m_ui64LastClockActivationDate == OV_IncorrectTime || m_rScheduler.getCurrentTime() - m_ui64LastClockActivationDate >=
										m_ui64ClockActivationStep))
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			if (m_ui64LastClockActivationDate == OV_IncorrectTime) { m_ui64LastClockActivationDate = m_rScheduler.getCurrentTime(); }
			else { m_ui64LastClockActivationDate = m_ui64LastClockActivationDate + m_ui64ClockActivationStep; }

			CMessageClock l_oClockMessage(this->getKernelContext());
			l_oClockMessage.setTime(m_ui64LastClockActivationDate);

			OV_ERROR_UNLESS_KRF(m_pBoxAlgorithm->processClock(l_oBoxAlgorithmContext, l_oClockMessage),
								"Box algorithm <" << m_pBox->getName() << "> processClock() function failed", ErrorType::Internal);

			m_bReadyToProcess |= l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
		}
	}

	return true;
}

bool CSimulatedBox::processInput(const uint32_t index, const CChunk& rChunk)
{
	m_vInput[index].push_back(rChunk);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, index),
								"Box algorithm <" << m_pBox->getName() << "> processInput() function failed", ErrorType::Internal);
		}
		m_bReadyToProcess |= l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
	}

	return true;
}

bool CSimulatedBox::process()
{
	if (!m_bReadyToProcess) { return true; }

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(m_pBoxAlgorithm->process(l_oBoxAlgorithmContext), "Box algorithm <" << m_pBox->getName() << "> processInput function failed",
								ErrorType::Internal);
		}
	}

	// perform output sending
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		m_pScenario->getLinkIdentifierFromBoxList(m_pBox->getIdentifier(), &listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const ILink* l_pLink = m_pScenario->getLinkDetails(listID[i]);
			if (l_pLink)
			{
				CIdentifier l_oTargetBoxIdentifier = l_pLink->getTargetBoxIdentifier();
				uint32_t l_ui32TargetBoxInputIndex = l_pLink->getTargetBoxInputIndex();

				uint32_t l_ui32SourceOutputIndex = l_pLink->getSourceBoxOutputIndex();
				for (auto& chunk : m_vOutput[l_ui32SourceOutputIndex]) { m_rScheduler.sendInput(chunk, l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex); }
			}
		}
		m_pScenario->releaseIdentifierList(listID);
	}

	// perform input cleaning
	auto socketIterator = m_vInput.begin();
	while (socketIterator != m_vInput.end())
	{
		auto inputChunkIterator = socketIterator->begin();
		while (inputChunkIterator != socketIterator->end())
		{
			if (inputChunkIterator->isDeprecated()) { inputChunkIterator = socketIterator->erase(inputChunkIterator); }
			else { ++inputChunkIterator; }
		}
		++socketIterator;
	}

	// flushes sent output chunks
	for (auto& socket : m_vOutput) { socket.resize(0); }

	// discards waiting output chunks
	for (const auto& chunk : m_vCurrentOutput)
	{
		OV_FATAL_UNLESS_K(chunk.getBuffer().getSize() == 0, "Output buffer filled but not marked as ready to send. Possible loss of data.",
						  ErrorType::Internal);
	}

	m_bReadyToProcess = false;

	return true;
}

bool CSimulatedBox::isReadyToProcess() const { return m_bReadyToProcess; }

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// - --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

CString CSimulatedBox::getName() const { return m_pBox->getName(); }

const IScenario& CSimulatedBox::getScenario() const { return *m_pScenario; }

// ________________________________________________________________________________________________________________
//

uint32_t CSimulatedBox::getInputChunkCount(const uint32_t index) const
{
	OV_ERROR_UNLESS_KRF(index < m_vInput.size(),
						"Input index = [" << index << "] is out of range (max index = [" << uint32_t(m_vInput.size() - 1) << "])",
						ErrorType::OutOfBound);

	return uint32_t(m_vInput[index].size());
}

bool CSimulatedBox::getInputChunk(const uint32_t inputIdx, const uint32_t chunkIdx, uint64_t& rStartTime, uint64_t& rEndTime, uint64_t& rChunkSize,
								  const uint8_t*& rpChunkBuffer) const
{
	OV_ERROR_UNLESS_KRF(inputIdx < m_vInput.size(),
						"Input index = [" << inputIdx << "] is out of range (max index = [" << uint32_t(m_vInput.size() - 1) << "])", ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRF(chunkIdx < m_vInput[inputIdx].size(),
						"Input chunk index = [" << chunkIdx << "] is out of range (max index = [" << uint32_t(m_vInput[inputIdx].size() - 1) << "])",
						ErrorType::OutOfBound);

	const CChunk& l_rChunk = m_vInput[inputIdx][chunkIdx];
	rStartTime             = l_rChunk.getStartTime();
	rEndTime               = l_rChunk.getEndTime();
	rChunkSize             = l_rChunk.getBuffer().getSize();
	rpChunkBuffer          = l_rChunk.getBuffer().getDirectPointer();
	return true;
}

const IMemoryBuffer* CSimulatedBox::getInputChunk(const uint32_t inputIdx, const uint32_t chunkIdx) const
{
	OV_ERROR_UNLESS_KRN(inputIdx < m_vInput.size(),
						"Input index = [" << inputIdx << "] is out of range (max index = [" << uint32_t(m_vInput.size() - 1) << "])",
						ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRN(chunkIdx < m_vInput[inputIdx].size(),
						"Input chunk index = [" << chunkIdx << "] is out of range (max index = [" << uint32_t(m_vInput[inputIdx].size() - 1) << "])",
						ErrorType::OutOfBound);

	return &(m_vInput[inputIdx][chunkIdx]).getBuffer();
}

uint64_t CSimulatedBox::getInputChunkStartTime(const uint32_t inputIdx, const uint32_t chunkIdx) const
{
	OV_ERROR_UNLESS_KRZ(inputIdx < m_vInput.size(),
						"Input index = [" << inputIdx << "] is out of range (max index = [" << uint32_t(m_vInput.size() - 1) << "])",
						ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRZ(chunkIdx < m_vInput[inputIdx].size(),
						"Input chunk index = [" << chunkIdx << "] is out of range (max index = [" << uint32_t(m_vInput[inputIdx].size() - 1) << "])",
						ErrorType::OutOfBound);

	const CChunk& l_rChunk = m_vInput[inputIdx][chunkIdx];
	return l_rChunk.getStartTime();
}

uint64_t CSimulatedBox::getInputChunkEndTime(const uint32_t inputIdx, const uint32_t chunkIdx) const
{
	OV_ERROR_UNLESS_KRZ(inputIdx < m_vInput.size(),
						"Input index = [" << inputIdx << "] is out of range (max index = [" << uint32_t(m_vInput.size() - 1) << "])",
						ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRZ(chunkIdx < m_vInput[inputIdx].size(),
						"Input chunk index = [" << chunkIdx << "] is out of range (max index = [" << uint32_t(m_vInput[inputIdx].size() - 1) << "])",
						ErrorType::OutOfBound);

	const CChunk& l_rChunk = m_vInput[inputIdx][chunkIdx];
	return l_rChunk.getEndTime();
}

bool CSimulatedBox::markInputAsDeprecated(const uint32_t inputIdx, const uint32_t chunkIdx)
{
	OV_ERROR_UNLESS_KRZ(inputIdx < m_vInput.size(),
						"Input index = [" << inputIdx << "] is out of range (max index = [" << uint32_t(m_vInput.size() - 1) << "])",
						ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRZ(chunkIdx < m_vInput[inputIdx].size(),
						"Input chunk index = [" << chunkIdx << "] is out of range (max index = [" << uint32_t(m_vInput[inputIdx].size() - 1) << "])",
						ErrorType::OutOfBound);

	m_vInput[inputIdx][chunkIdx].markAsDeprecated(true);
	return true;
}

// ________________________________________________________________________________________________________________
//

uint64_t CSimulatedBox::getOutputChunkSize(const uint32_t outputIdx) const
{
	OV_ERROR_UNLESS_KRZ(outputIdx < m_vCurrentOutput.size(),
						"Output index = [" << outputIdx << "] is out of range (max index = [" << uint32_t(m_vCurrentOutput.size() - 1) << "])",
						ErrorType::OutOfBound);

	return m_vCurrentOutput[outputIdx].getBuffer().getSize();
}

bool CSimulatedBox::setOutputChunkSize(const uint32_t outputIdx, const uint64_t size, const bool bDiscard)
{
	OV_ERROR_UNLESS_KRF(outputIdx < m_vCurrentOutput.size(),
						"Output index = [" << outputIdx << "] is out of range (max index = [" << uint32_t(m_vCurrentOutput.size() - 1) << "])",
						ErrorType::OutOfBound);

	return m_vCurrentOutput[outputIdx].getBuffer().setSize(size, bDiscard);
}

uint8_t* CSimulatedBox::getOutputChunkBuffer(const uint32_t outputIdx)
{
	OV_ERROR_UNLESS_KRN(outputIdx < m_vCurrentOutput.size(),
						"Output index = [" << outputIdx << "] is out of range (max index = [" << uint32_t(m_vCurrentOutput.size() - 1) << "])",
						ErrorType::OutOfBound);

	return m_vCurrentOutput[outputIdx].getBuffer().getDirectPointer();
}

bool CSimulatedBox::appendOutputChunkData(const uint32_t outputIdx, const uint8_t* buffer, const uint64_t size)
{
	OV_ERROR_UNLESS_KRF(outputIdx < m_vCurrentOutput.size(),
						"Output index = [" << outputIdx << "] is out of range (max index = [" << uint32_t(m_vCurrentOutput.size() - 1) << "])",
						ErrorType::OutOfBound);

	return m_vCurrentOutput[outputIdx].getBuffer().append(buffer, size);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(const uint32_t outputIdx)
{
	OV_ERROR_UNLESS_KRN(outputIdx < m_vCurrentOutput.size(),
						"Output index = [" << outputIdx << "] is out of range (max index = [" << uint32_t(m_vCurrentOutput.size() - 1) << "])",
						ErrorType::OutOfBound);

	return &m_vCurrentOutput[outputIdx].getBuffer();
}

bool CSimulatedBox::markOutputAsReadyToSend(const uint32_t outputIdx, const uint64_t ui64StartTime, const uint64_t ui64EndTime)
{
	OV_ERROR_UNLESS_KRF(outputIdx < m_vCurrentOutput.size(),
						"Output index = [" << outputIdx << "] is out of range (max index = [" << uint32_t(m_vCurrentOutput.size() - 1) << "])",
						ErrorType::OutOfBound);

	if (m_bChunkConsistencyChecking)
	{
		bool l_bIsConsistent           = true;
		const char* l_sSpecificMessage = nullptr;

		// checks chunks consistency
		CIdentifier l_oType;
		m_pBox->getOutputType(outputIdx, l_oType);
		if (l_oType == OV_TypeId_Stimulations)
		{
			if (m_vLastOutputEndTime[outputIdx] != ui64StartTime)
			{
				l_bIsConsistent    = false;
				l_sSpecificMessage = "'Stimulations' streams should have continuously dated chunks";
			}
		}

		if (m_vLastOutputEndTime[outputIdx] > ui64EndTime)
		{
			l_bIsConsistent    = false;
			l_sSpecificMessage = "Current 'end time' can not be earlier than previous 'end time'";
		}

		if (m_vLastOutputStartTime[outputIdx] > ui64StartTime)
		{
			l_bIsConsistent    = false;
			l_sSpecificMessage = "Current 'start time' can not be earlier than previous 'start time'";
		}

		if (!l_bIsConsistent)
		{
			this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << "Box <" << m_pBox->getName() << "> sends inconsistent chunk dates on output [" <<
					outputIdx << "] (current chunk dates are [" << ui64StartTime << "," << ui64EndTime << "] whereas previous chunk dates were [" <<
					m_vLastOutputStartTime[outputIdx] << "," << m_vLastOutputEndTime[outputIdx] << "])\n";
			if (l_sSpecificMessage) { this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << l_sSpecificMessage << "\n"; }
			this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << "Please report to box author and attach your scenario\n";
			this->getLogManager() << LogLevel_Trace << "Previous warning can be disabled setting Kernel_CheckChunkConsistency to false\n";
			m_eChunkConsistencyCheckingLogLevel = LogLevel_Trace;
		}

		// sets last times
		m_vLastOutputStartTime[outputIdx] = ui64StartTime;
		m_vLastOutputEndTime[outputIdx]   = ui64EndTime;
	}

	// sets start and end time
	m_vCurrentOutput[outputIdx].setStartTime(std::min(ui64StartTime, ui64EndTime));
	m_vCurrentOutput[outputIdx].setEndTime(std::max(ui64StartTime, ui64EndTime));

	// copies chunk
	m_vOutput[outputIdx].push_back(m_vCurrentOutput[outputIdx]);

	// resets chunk size
	m_vCurrentOutput[outputIdx].getBuffer().setSize(0, true);

	return true;
}

// #endif // __MY_COMPILE_ALL

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
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

// ________________________________________________________________________________________________________________
//

#define OV_IncorrectTime 0xffffffffffffffffULL
static const CNameValuePairList s_oDummyNameValuePairList;

CSimulatedBox::CSimulatedBox(const IKernelContext& rKernelContext, CScheduler& rScheduler)
	:TKernelObject<IBoxIO>(rKernelContext)
	,m_bReadyToProcess(false)
	,m_bChunkConsistencyChecking(false)
	,m_eChunkConsistencyCheckingLogLevel(LogLevel_Warning)
	,m_pBoxAlgorithm(nullptr)
	,m_pScenario(nullptr)
	,m_pBox(nullptr)
	,m_rScheduler(rScheduler)
	,m_ui64LastClockActivationDate(OV_IncorrectTime)
	,m_ui64ClockFrequency(0)
	,m_ui64ClockActivationStep(0)
{
}

CSimulatedBox::~CSimulatedBox(void)
{
}

bool CSimulatedBox::setScenarioIdentifier(const CIdentifier& rScenarioIdentifier)
{
	OV_ERROR_UNLESS_KRF(
		m_rScheduler.getPlayer().getRuntimeScenarioManager().isScenario(rScenarioIdentifier),
		"Scenario with identifier " << rScenarioIdentifier.toString() << " does not exist",
		ErrorType::ResourceNotFound
	);

	m_pScenario=&m_rScheduler.getPlayer().getRuntimeScenarioManager().getScenario(rScenarioIdentifier);
	return true;
}

bool CSimulatedBox::getBoxIdentifier(CIdentifier& rBoxIdentifier) const
{
	OV_ERROR_UNLESS_KRF(m_pBox, "Simulated box not initialized", ErrorType::BadCall);

	rBoxIdentifier = m_pBox->getIdentifier();
	return true;
}

bool CSimulatedBox::setBoxIdentifier(const CIdentifier& rBoxIdentifier)
{
	OV_ERROR_UNLESS_KRF(m_pScenario, "No scenario set", ErrorType::BadCall);

	m_pBox = m_pScenario->getBoxDetails(rBoxIdentifier);
	return m_pBox!=NULL;
}

bool CSimulatedBox::initialize(void)
{
	OV_ERROR_UNLESS_KRF(m_pBox, "Simulated box not initialized", ErrorType::BadCall);
	OV_ERROR_UNLESS_KRF(m_pScenario, "No scenario set", ErrorType::BadCall);

	m_bChunkConsistencyChecking = this->getConfigurationManager().expandAsBoolean("${Kernel_CheckChunkConsistency}", true);
	m_vInput.resize(m_pBox->getInputCount());
	m_vOutput.resize(m_pBox->getOutputCount());
	m_vCurrentOutput.resize(m_pBox->getOutputCount());
	m_vLastOutputStartTime.resize(m_pBox->getOutputCount(), 0);
	m_vLastOutputEndTime.resize(m_pBox->getOutputCount(), 0);

	m_ui64LastClockActivationDate=OV_IncorrectTime;
	m_ui64ClockFrequency=0;
	m_ui64ClockActivationStep=0;

	m_pBoxAlgorithm=getPluginManager().createBoxAlgorithm(m_pBox->getAlgorithmClassIdentifier(), NULL);

	OV_ERROR_UNLESS_KRF(
		m_pBoxAlgorithm,
		"Could not create box algorithm with class id " << m_pBox->getAlgorithmClassIdentifier().toString(),
		ErrorType::BadResourceCreation
	);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(
				m_pBoxAlgorithm->initialize(l_oBoxAlgorithmContext),
				"Box algorithm <" << m_pBox->getName() << "> initialization failed",
				ErrorType::Internal
			);
		}
	}

	return true ;
}

boolean CSimulatedBox::uninitialize(void)
{
	if (!m_pBoxAlgorithm)
	{
		return true;
	}

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(
				m_pBoxAlgorithm->uninitialize(l_oBoxAlgorithmContext),
				"Box algorithm <" << m_pBox->getName() << "> uninitialization failed",
				ErrorType::Internal
			);
		}
	}

	getPluginManager().releasePluginObject(m_pBoxAlgorithm);
	m_pBoxAlgorithm=NULL;

	return true;
}

boolean CSimulatedBox::processClock(void)
{
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			uint64 l_ui64NewClockFrequency=m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
			if(l_ui64NewClockFrequency==0)
			{
				m_ui64ClockActivationStep=OV_IncorrectTime;
				m_ui64LastClockActivationDate=OV_IncorrectTime;
			}
			else
			{
				OV_ERROR_UNLESS_KRF(
					l_ui64NewClockFrequency <= m_rScheduler.getFrequency()<<32,
					"Box " << m_pBox->getName()
						   << " requested higher clock frequency (" << l_ui64NewClockFrequency << " == "
						   << ITimeArithmetics::timeToSeconds(l_ui64NewClockFrequency) << "hz) "
						   << "than what the scheduler can handle (" << (m_rScheduler.getFrequency()<<32) << " == "
						   << ITimeArithmetics::timeToSeconds(m_rScheduler.getFrequency()<<32) << "hz)",
					ErrorType::BadConfig
				);

				// note: 1LL should be left shifted 64 bits but this
				//       would result in an integer over shift (the one
				//       would exit). Thus the left shift of 63 bits
				//       and the left shift of 1 bit after the division
				m_ui64ClockActivationStep=((1ULL<<63)/l_ui64NewClockFrequency)<<1;
			}
			m_ui64ClockFrequency=l_ui64NewClockFrequency;
		}
	}

	if((m_ui64ClockFrequency!=0) && (m_ui64LastClockActivationDate==OV_IncorrectTime || m_rScheduler.getCurrentTime()-m_ui64LastClockActivationDate>=m_ui64ClockActivationStep))
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			if(m_ui64LastClockActivationDate==OV_IncorrectTime)
			{
				m_ui64LastClockActivationDate=m_rScheduler.getCurrentTime();
			}
			else
			{
				m_ui64LastClockActivationDate=m_ui64LastClockActivationDate+m_ui64ClockActivationStep;
			}

			CMessageClock l_oClockMessage(this->getKernelContext());
			l_oClockMessage.setTime(m_ui64LastClockActivationDate);

			OV_ERROR_UNLESS_KRF(
				m_pBoxAlgorithm->processClock(l_oBoxAlgorithmContext, l_oClockMessage),
				"Box algorithm <" << m_pBox->getName() << "> processClock() function failed",
				ErrorType::Internal
			);

			m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
		}
	}

	return true;
}

boolean CSimulatedBox::processInput(const uint32 ui32InputIndex, const CChunk& rChunk)
{
	m_vInput[ui32InputIndex].push_back(rChunk);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(
				m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, ui32InputIndex),
				"Box algorithm <" << m_pBox->getName() << "> processInput() function failed",
				ErrorType::Internal
			);
		}
		m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
	}

	return true;
}

boolean CSimulatedBox::process(void)
{
	if (!m_bReadyToProcess)
	{
		return true;
	}

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			OV_ERROR_UNLESS_KRF(
				m_pBoxAlgorithm->process(l_oBoxAlgorithmContext),
				"Box algorithm <" << m_pBox->getName() << "> processInput function failed",
				ErrorType::Internal
			);
		}
	}

	// perform output sending
	{
		CIdentifier* identifierList = nullptr;
		size_t nbElems = 0;
		m_pScenario->getLinkIdentifierFromBoxList(m_pBox->getIdentifier(), &identifierList, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const ILink* l_pLink = m_pScenario->getLinkDetails(identifierList[i]);
			if (l_pLink)
			{
				CIdentifier l_oTargetBoxIdentifier = l_pLink->getTargetBoxIdentifier();
				uint32_t l_ui32TargetBoxInputIndex = l_pLink->getTargetBoxInputIndex();

				uint32_t l_ui32SourceOutputIndex = l_pLink->getSourceBoxOutputIndex();
				for (auto& chunk : m_vOutput[l_ui32SourceOutputIndex])
				{
					m_rScheduler.sendInput(chunk, l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);
				}
			}
		}
		m_pScenario->releaseIdentifierList(identifierList);
	}

	// iterators for input and output chunks
	vector < deque< CChunk > >::iterator socketIterator;
	deque < CChunk >::iterator inputChunkIterator;

	// perform input cleaning
	socketIterator=m_vInput.begin();
	while(socketIterator!=m_vInput.end())
	{
		inputChunkIterator=socketIterator->begin();
		while(inputChunkIterator!=socketIterator->end())
		{
			if(inputChunkIterator->isDeprecated())
			{
				inputChunkIterator=socketIterator->erase(inputChunkIterator);
			}
			else
			{
				++inputChunkIterator;
			}
		}
		++socketIterator;
	}

	// flushes sent output chunks
	for (auto& socket : m_vOutput)
	{
		socket.resize(0);
	}

	// discards waiting output chunks
	for (const auto& chunk : m_vCurrentOutput)
	{
		OV_FATAL_UNLESS_K(
			chunk.getBuffer().getSize() == 0,
			"Output buffer filled but not marked as ready to send. Possible loss of data.",
			ErrorType::Internal
		);
	}

	m_bReadyToProcess=false;

	return true;
}

boolean CSimulatedBox::isReadyToProcess(void) const
{
	return m_bReadyToProcess;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// - --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- -
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

CString CSimulatedBox::getName(void) const
{
	return m_pBox->getName();
}

const IScenario& CSimulatedBox::getScenario(void) const
{
	return *m_pScenario;
}

// ________________________________________________________________________________________________________________
//

uint32 CSimulatedBox::getInputChunkCount(
	const uint32 ui32InputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		ui32InputIndex < m_vInput.size(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return m_vInput[ui32InputIndex].size();
}

boolean CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex,
	uint64& rStartTime,
	uint64& rEndTime,
	uint64& rChunkSize,
	const uint8*& rpChunkBuffer) const
{
	OV_ERROR_UNLESS_KRF(
		ui32InputIndex < m_vInput.size(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		ui32ChunkIndex < m_vInput[ui32InputIndex].size(),
		"Input chunk index = [" << ui32ChunkIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput[ui32InputIndex].size() - 1) << "])",
		ErrorType::OutOfBound
	);

	const CChunk& l_rChunk = m_vInput[ui32InputIndex][ui32ChunkIndex];
	rStartTime=l_rChunk.getStartTime();
	rEndTime=l_rChunk.getEndTime();
	rChunkSize=l_rChunk.getBuffer().getSize();
	rpChunkBuffer=l_rChunk.getBuffer().getDirectPointer();
	return true;
}

const IMemoryBuffer* CSimulatedBox::getInputChunk(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	OV_ERROR_UNLESS_KRN(
		ui32InputIndex < m_vInput.size(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRN(
		ui32ChunkIndex < m_vInput[ui32InputIndex].size(),
		"Input chunk index = [" << ui32ChunkIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput[ui32InputIndex].size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return &(m_vInput[ui32InputIndex][ui32ChunkIndex]).getBuffer();
}

uint64 CSimulatedBox::getInputChunkStartTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	OV_ERROR_UNLESS_KRZ(
		ui32InputIndex < m_vInput.size(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRZ(
		ui32ChunkIndex < m_vInput[ui32InputIndex].size(),
		"Input chunk index = [" << ui32ChunkIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput[ui32InputIndex].size() - 1) << "])",
		ErrorType::OutOfBound
	);

	const CChunk& l_rChunk = m_vInput[ui32InputIndex][ui32ChunkIndex];
	return l_rChunk.getStartTime();
}

uint64 CSimulatedBox::getInputChunkEndTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	OV_ERROR_UNLESS_KRZ(
		ui32InputIndex < m_vInput.size(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRZ(
		ui32ChunkIndex < m_vInput[ui32InputIndex].size(),
		"Input chunk index = [" << ui32ChunkIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput[ui32InputIndex].size() - 1) << "])",
		ErrorType::OutOfBound
	);

	const CChunk& l_rChunk = m_vInput[ui32InputIndex][ui32ChunkIndex];
	return l_rChunk.getEndTime();
}

boolean CSimulatedBox::markInputAsDeprecated(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex)
{
	OV_ERROR_UNLESS_KRZ(
		ui32InputIndex < m_vInput.size(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRZ(
		ui32ChunkIndex < m_vInput[ui32InputIndex].size(),
		"Input chunk index = [" << ui32ChunkIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput[ui32InputIndex].size() - 1) << "])",
		ErrorType::OutOfBound
	);

	m_vInput[ui32InputIndex][ui32ChunkIndex].markAsDeprecated(true);
	return true;
}

// ________________________________________________________________________________________________________________
//

uint64 CSimulatedBox::getOutputChunkSize(
	const uint32 ui32OutputIndex) const
{
	OV_ERROR_UNLESS_KRZ(
		ui32OutputIndex < m_vCurrentOutput.size(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vCurrentOutput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getSize();
}

boolean CSimulatedBox::setOutputChunkSize(
	const uint32 ui32OutputIndex,
	const uint64 ui64Size,
	const boolean bDiscard)
{
	OV_ERROR_UNLESS_KRF(
		ui32OutputIndex < m_vCurrentOutput.size(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vCurrentOutput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(ui64Size, bDiscard);
}

uint8* CSimulatedBox::getOutputChunkBuffer(
	const uint32 ui32OutputIndex)
{
	OV_ERROR_UNLESS_KRN(
		ui32OutputIndex < m_vCurrentOutput.size(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vCurrentOutput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getDirectPointer();
}

boolean CSimulatedBox::appendOutputChunkData(
	const uint32 ui32OutputIndex,
	const uint8* pBuffer,
	const uint64 ui64BufferSize)
{
	OV_ERROR_UNLESS_KRF(
		ui32OutputIndex < m_vCurrentOutput.size(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vCurrentOutput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return m_vCurrentOutput[ui32OutputIndex].getBuffer().append(pBuffer, ui64BufferSize);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(
	const uint32 ui32OutputIndex)
{
	OV_ERROR_UNLESS_KRN(
		ui32OutputIndex < m_vCurrentOutput.size(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vCurrentOutput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	return &m_vCurrentOutput[ui32OutputIndex].getBuffer();
}

boolean CSimulatedBox::markOutputAsReadyToSend(
	const uint32 ui32OutputIndex,
	const uint64 ui64StartTime,
	const uint64 ui64EndTime)
{
	OV_ERROR_UNLESS_KRF(
		ui32OutputIndex < m_vCurrentOutput.size(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vCurrentOutput.size() - 1) << "])",
		ErrorType::OutOfBound
	);

	if(m_bChunkConsistencyChecking)
	{
		boolean l_bIsConsistent = true;
		const char* l_sSpecificMessage = NULL;

		// checks chunks consistency
		CIdentifier l_oType;
		m_pBox->getOutputType(ui32OutputIndex, l_oType);
		if(l_oType == OV_TypeId_Stimulations)
		{
			if(m_vLastOutputEndTime[ui32OutputIndex] != ui64StartTime)
			{
				l_bIsConsistent = false;
				l_sSpecificMessage = "'Stimulations' streams should have continuously dated chunks";
			}
		}

		if(m_vLastOutputEndTime[ui32OutputIndex] > ui64EndTime)
		{
			l_bIsConsistent = false;
			l_sSpecificMessage = "Current 'end time' can not be earlier than previous 'end time'";
		}

		if(m_vLastOutputStartTime[ui32OutputIndex] > ui64StartTime)
		{
			l_bIsConsistent = false;
			l_sSpecificMessage = "Current 'start time' can not be earlier than previous 'start time'";
		}

		if(!l_bIsConsistent)
		{
			this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << "Box <" << m_pBox->getName() << "> sends inconsistent chunk dates on output [" << ui32OutputIndex << "] (current chunk dates are [" << ui64StartTime << "," << ui64EndTime << "] whereas previous chunk dates were [" << m_vLastOutputStartTime[ui32OutputIndex] << "," << m_vLastOutputEndTime[ui32OutputIndex] << "])\n";
			if(l_sSpecificMessage) this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << l_sSpecificMessage << "\n";
			this->getLogManager() << m_eChunkConsistencyCheckingLogLevel << "Please report to box author and attach your scenario\n";
			this->getLogManager() << LogLevel_Trace                      << "Previous warning can be disabled setting Kernel_CheckChunkConsistency to false\n";
			m_eChunkConsistencyCheckingLogLevel = LogLevel_Trace;
		}

		// sets last times
		m_vLastOutputStartTime[ui32OutputIndex] = ui64StartTime;
		m_vLastOutputEndTime[ui32OutputIndex] = ui64EndTime;
	}

	// sets start and end time
	m_vCurrentOutput[ui32OutputIndex].setStartTime(std::min(ui64StartTime, ui64EndTime));
	m_vCurrentOutput[ui32OutputIndex].setEndTime(std::max(ui64StartTime, ui64EndTime));

	// copies chunk
	m_vOutput[ui32OutputIndex].push_back(m_vCurrentOutput[ui32OutputIndex]);

	// resets chunk size
	m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(0, true);

	return true;
}

// #endif // __MY_COMPILE_ALL

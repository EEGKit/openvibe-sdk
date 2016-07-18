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

#define OV_IncorrectTime 0xffffffffffffffffll
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
	,m_oSceneIdentifier(OV_UndefinedIdentifier)
{
}

CSimulatedBox::~CSimulatedBox(void)
{
	m_mSimulatedObjects.clear();
}

CIdentifier CSimulatedBox::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	std::map<CIdentifier, CString>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_mSimulatedObjects.find(l_oResult);
	}
	while(i!=m_mSimulatedObjects.end() || l_oResult==OV_UndefinedIdentifier);
	//TODO : browse window and scene maps as well to guarantee ID unicity!
	return l_oResult;
}

bool CSimulatedBox::setScenarioIdentifier(const CIdentifier& rScenarioIdentifier)
{
	// FIXME: test if rScenario is a scenario identifier
	if (!m_rScheduler.getPlayer().getRuntimeScenarioManager().isScenario(rScenarioIdentifier))
	{
		this->getLogManager()<< LogLevel_Error << "Scenario with identifier " << rScenarioIdentifier << " does not exist" << "\n";
		return false;
	}
	m_pScenario=&m_rScheduler.getPlayer().getRuntimeScenarioManager().getScenario(rScenarioIdentifier);
	return true;
}

bool CSimulatedBox::getBoxIdentifier(CIdentifier& rBoxIdentifier) const
{
	if (!m_pBox)
	{
		return false;
	}

	rBoxIdentifier = m_pBox->getIdentifier();
	return true;
}

bool CSimulatedBox::setBoxIdentifier(const CIdentifier& rBoxIdentifier)
{
	if (!m_pScenario)
	{
		return false;
	}

	m_pBox = m_pScenario->getBoxDetails(rBoxIdentifier);
	return m_pBox!=NULL;
}

bool CSimulatedBox::initialize(void)
{
	// FIXME test for already initialized boxes etc
	if (!m_pBox)
	{
		return false;
	}

	if (!m_pScenario)
	{
		return false;
	}

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
	if(!m_pBoxAlgorithm)
	{
		getLogManager() << LogLevel_Error << "Could not create box algorithm with class id " << m_pBox->getAlgorithmClassIdentifier() << "...\n";
		return false;
	}

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			if(!m_pBoxAlgorithm->initialize(l_oBoxAlgorithmContext))
			{
				getLogManager() << LogLevel_ImportantWarning << "Box algorithm <" << m_pBox->getName() << "> initialization failed\n";
				return false;
			}
		}
	}

	return true ;
}

boolean CSimulatedBox::uninitialize(void)
{
	bool l_bResult = true;
	if(!m_pBoxAlgorithm) return false;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
			{
				if(!m_pBoxAlgorithm->uninitialize(l_oBoxAlgorithmContext))
				{
					getLogManager() << LogLevel_Error << "Box algorithm <" << m_pBox->getName() << "> uninitialization failed\n";
					l_bResult = false;
				}
			}
		}
	}

	getPluginManager().releasePluginObject(m_pBoxAlgorithm);
	m_pBoxAlgorithm=NULL;

	return l_bResult ;
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
				if(l_ui64NewClockFrequency > m_rScheduler.getFrequency()<<32)
				{
					this->getLogManager() << LogLevel_ImportantWarning << "Box " << m_pBox->getName() 
						<< " requested higher clock frequency (" << l_ui64NewClockFrequency << " == " 
						<< ITimeArithmetics::timeToSeconds(l_ui64NewClockFrequency) << "hz) "
						<< "than what the scheduler can handle (" << (m_rScheduler.getFrequency()<<32) << " == "
						<< ITimeArithmetics::timeToSeconds(m_rScheduler.getFrequency()<<32) << "hz)\n";
				}

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
			if(!m_pBoxAlgorithm->processClock(l_oBoxAlgorithmContext, l_oClockMessage))
			{
				getLogManager() << LogLevel_Error << "Box algorithm <" << m_pBox->getName() << "> processClock() function failed\n";
				return false;
			}
				
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
			if(!m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, ui32InputIndex))
			{
				getLogManager() << LogLevel_Error << "Box algorithm <" << m_pBox->getName() << "> processInput() failed\n";
				return false;
			}
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
			if(!m_pBoxAlgorithm->process(l_oBoxAlgorithmContext))
			{
				getLogManager() << LogLevel_Error << "Box algorithm <" << m_pBox->getName() << "> process() function failed\n";
				return false;
			}
		}
	}

	// perform output sending
	CIdentifier l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(OV_UndefinedIdentifier, m_pBox->getIdentifier());
	while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
	{
		const ILink* l_pLink=m_pScenario->getLinkDetails(l_oLinkIdentifier);
		if(l_pLink)
		{
			CIdentifier l_oTargetBoxIdentifier=l_pLink->getTargetBoxIdentifier();
			uint32 l_ui32TargetBoxInputIndex=l_pLink->getTargetBoxInputIndex();

			uint32 l_ui32SourceOutputIndex=l_pLink->getSourceBoxOutputIndex();
			deque < CChunk >::iterator i=m_vOutput[l_ui32SourceOutputIndex].begin();
			while(i!=m_vOutput[l_ui32SourceOutputIndex].end())
			{
				m_rScheduler.sendInput(*i, l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);
				++i;
			}
		}
		l_oLinkIdentifier=m_pScenario->getNextLinkIdentifierFromBox(l_oLinkIdentifier, m_pBox->getIdentifier());
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
	socketIterator=m_vOutput.begin();
	while(socketIterator!=m_vOutput.end())
	{
		socketIterator->resize(0);
		++socketIterator;
	}

	vector < CChunk >::iterator outputChunkIterator;
	// discards waiting output chunks
	outputChunkIterator=m_vCurrentOutput.begin();
	while(outputChunkIterator!=m_vCurrentOutput.end())
	{
		// checks buffer size
		if(outputChunkIterator->getBuffer().getSize())
		{
			// the buffer has been (partially ?) filled but not sent
			CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
			l_oBoxAlgorithmContext.getPlayerContext()->getLogManager() << LogLevel_Warning << "Output buffer filled but not marked as ready to send\n"; // $$$ should use log
			outputChunkIterator->getBuffer().setSize(0, true);
		}

		++outputChunkIterator;
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

namespace
{
	void warningOnOutOfBoundInput(ILogManager& rLogManager, const CString& sName, uint32 ui32InputIndex, size_t ui32InputCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound input (" << ui32InputIndex << "/" << static_cast<uint64>(ui32InputCount) << ")\n";
	}

	void warningOnOutOfBoundInputChunk(ILogManager& rLogManager, const CString& sName, uint32 ui32InputIndex, uint32 ui32InputChunkIndex, size_t ui32InputChunkCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound input (" << ui32InputIndex << ":" << ui32InputChunkIndex << "/" << static_cast<uint64>(ui32InputChunkCount) << ")\n";
	}
}

// ________________________________________________________________________________________________________________
//

uint32 CSimulatedBox::getInputChunkCount(
	const uint32 ui32InputIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		warningOnOutOfBoundInput(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return 0;
	}
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
	if(ui32InputIndex>=m_vInput.size())
	{
		warningOnOutOfBoundInput(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		warningOnOutOfBoundInputChunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

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
	if(ui32InputIndex>=m_vInput.size())
	{
		warningOnOutOfBoundInput(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return NULL;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		warningOnOutOfBoundInputChunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return NULL;
	}
	return &(m_vInput[ui32InputIndex][ui32ChunkIndex]).getBuffer();
}

uint64 CSimulatedBox::getInputChunkStartTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		warningOnOutOfBoundInput(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		warningOnOutOfBoundInputChunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk = m_vInput[ui32InputIndex][ui32ChunkIndex];
	return l_rChunk.getStartTime();
}

uint64 CSimulatedBox::getInputChunkEndTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		warningOnOutOfBoundInput(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		warningOnOutOfBoundInputChunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk = m_vInput[ui32InputIndex][ui32ChunkIndex];
	return l_rChunk.getEndTime();
}

boolean CSimulatedBox::markInputAsDeprecated(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex)
{
	if(ui32InputIndex>=m_vInput.size())
	{
		warningOnOutOfBoundInput(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		warningOnOutOfBoundInputChunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}
	m_vInput[ui32InputIndex][ui32ChunkIndex].markAsDeprecated(true);
	return true;
}

// ________________________________________________________________________________________________________________
//

namespace
{
	void warningOnOutOfBoundOutput(ILogManager& rLogManager, const CString& sName, uint32 ui32OutputIndex, size_t ui32OutputCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound output (" << ui32OutputIndex << "/" << static_cast<uint64>(ui32OutputCount) << ")\n";
	}
}

// ________________________________________________________________________________________________________________
//

uint64 CSimulatedBox::getOutputChunkSize(
	const uint32 ui32OutputIndex) const
{
	if(ui32OutputIndex>=m_vCurrentOutput.size())
	{
		warningOnOutOfBoundOutput(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return 0;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getSize();
}

boolean CSimulatedBox::setOutputChunkSize(
	const uint32 ui32OutputIndex,
	const uint64 ui64Size,
	const boolean bDiscard)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		warningOnOutOfBoundOutput(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(ui64Size, bDiscard);
}

uint8* CSimulatedBox::getOutputChunkBuffer(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		warningOnOutOfBoundOutput(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return NULL;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().getDirectPointer();
}

boolean CSimulatedBox::appendOutputChunkData(
	const uint32 ui32OutputIndex,
	const uint8* pBuffer,
	const uint64 ui64BufferSize)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		warningOnOutOfBoundOutput(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().append(pBuffer, ui64BufferSize);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		warningOnOutOfBoundOutput(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return NULL;
	}
	return &m_vCurrentOutput[ui32OutputIndex].getBuffer();
}

boolean CSimulatedBox::markOutputAsReadyToSend(
	const uint32 ui32OutputIndex,
	const uint64 ui64StartTime,
	const uint64 ui64EndTime)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		warningOnOutOfBoundOutput(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}

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

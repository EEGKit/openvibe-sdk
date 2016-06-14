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

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

#define boolean OpenViBE::boolean

//#define _BoxAlgorithm_ScopeTester_
//#define _SimulatedBox_ScopeTester_

#define __OV_FUNC__ CString("unknown_function_name")
#define __OV_LINE__ uint32(__LINE__)
#define __OV_FILE__ CString(__FILE__)

// ________________________________________________________________________________________________________________
//

namespace
{
	template <typename T>
	T& _my_get_(deque<T>& rList, uint32 ui32Index)
	{
		typename deque<T>::iterator it=rList.begin()+ui32Index;
		return *it;
	}

	template <typename T>
	const T& _my_get_(const deque<T>& rList, uint32 ui32Index)
	{
		typename deque<T>::const_iterator it=rList.begin()+ui32Index;
		return *it;
	}
}

// ________________________________________________________________________________________________________________
//

#define _Bad_Time_ 0xffffffffffffffffll
static const CNameValuePairList s_oDummyNameValuePairList;

CSimulatedBox::CSimulatedBox(const IKernelContext& rKernelContext, CScheduler& rScheduler)
	:TKernelObject<IBoxIO>(rKernelContext)
	,m_bReadyToProcess(false)
	,m_bChunkConsistencyChecking(false)
	,m_eChunkConsistencyCheckingLogLevel(LogLevel_Warning)
	,m_pBoxAlgorithm(NULL)
	,m_pScenario(NULL)
	,m_pBox(NULL)
	,m_rScheduler(rScheduler)
	,m_ui64LastClockActivationDate(_Bad_Time_)
	,m_ui64ClockFrequency(0)
	,m_ui64ClockActivationStep(0)
	,m_oSceneIdentifier(OV_UndefinedIdentifier)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif
}

CSimulatedBox::~CSimulatedBox(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	//clear simulated objects map
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

boolean CSimulatedBox::setScenarioIdentifier(const CIdentifier& rScenarioIdentifier)
{
	// FIXME test if rScenario is a scenario identifier
	m_pScenario=&m_rScheduler.getPlayer().getRuntimeScenarioManager().getScenario(rScenarioIdentifier);
	return m_pScenario!=NULL;
}

boolean CSimulatedBox::getBoxIdentifier(CIdentifier& rBoxIdentifier) const
{
	if(m_pBox == NULL)
	{
		return false;
	}
	rBoxIdentifier = m_pBox->getIdentifier();
	return true;
}

boolean CSimulatedBox::setBoxIdentifier(const CIdentifier& rBoxIdentifier)
{
	if(!m_pScenario)
	{
		return false;
	}

	m_pBox=m_pScenario->getBoxDetails(rBoxIdentifier);
	return m_pBox!=NULL;
}

boolean CSimulatedBox::initialize(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	// FIXME test for already initialized boxes etc
	if(!m_pBox) return false;
	if(!m_pScenario) return false;

	m_bChunkConsistencyChecking=this->getConfigurationManager().expandAsBoolean("${Kernel_CheckChunkConsistency}", true);
	m_vInput.resize(m_pBox->getInputCount());
	m_vOutput.resize(m_pBox->getOutputCount());
	m_vCurrentOutput.resize(m_pBox->getOutputCount());
	m_vLastOutputStartTime.resize(m_pBox->getOutputCount(), 0);
	m_vLastOutputEndTime.resize(m_pBox->getOutputCount(), 0);

	m_oBenchmarkChronoProcessClock.reset(1024);
	m_oBenchmarkChronoProcessInput.reset(1024);
	m_oBenchmarkChronoProcess.reset(1024);

	m_ui64LastClockActivationDate=_Bad_Time_;
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
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::initialize)"));
#endif

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
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif
	
	bool l_bResult = true;
	if(!m_pBoxAlgorithm) return false;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::uninitialize)"));
#endif
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
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::getClockFrequency)"));
#endif
			uint64 l_ui64NewClockFrequency=m_pBoxAlgorithm->getClockFrequency(l_oBoxAlgorithmContext);
			if(l_ui64NewClockFrequency==0)
			{
				m_ui64ClockActivationStep=_Bad_Time_;
				m_ui64LastClockActivationDate=_Bad_Time_;
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
				m_ui64ClockActivationStep=((1LL<<63)/l_ui64NewClockFrequency)<<1;
			}
			m_ui64ClockFrequency=l_ui64NewClockFrequency;
		}
	}

	if((m_ui64ClockFrequency!=0) && (m_ui64LastClockActivationDate==_Bad_Time_ || m_rScheduler.getCurrentTime()-m_ui64LastClockActivationDate>=m_ui64ClockActivationStep))
	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processClock)"));
#endif	
			m_oBenchmarkChronoProcessClock.stepIn();

			if(m_ui64LastClockActivationDate==_Bad_Time_)
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
			m_oBenchmarkChronoProcessClock.stepOut();
				
			m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
		}
	}

	return true;
}

boolean CSimulatedBox::processInput(const uint32 ui32InputIndex, const CChunk& rChunk)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	m_vInput[ui32InputIndex].push_back(rChunk);

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::processInput)"));
#endif
			m_oBenchmarkChronoProcessInput.stepIn();
			if(!m_pBoxAlgorithm->processInput(l_oBoxAlgorithmContext, ui32InputIndex))
			{
				getLogManager() << LogLevel_Error << "Box algorithm <" << m_pBox->getName() << "> processInput() failed\n";
				return false;
			}
			m_oBenchmarkChronoProcessInput.stepOut();
		}
		m_bReadyToProcess|=l_oBoxAlgorithmContext.isAlgorithmReadyToProcess();
	}

	return true;
}

boolean CSimulatedBox::process(void)
{
#if defined _SimulatedBox_ScopeTester_
	this->getLogManager() << LogLevel_Debug << __OV_FUNC__ << " - " << __OV_FILE__ << ":" << __OV_LINE__ << "\n";
#endif

	if(!m_bReadyToProcess) return true;

	{
		CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
		{
#if defined _BoxAlgorithm_ScopeTester_
			Tools::CScopeTester l_oScopeTester(getKernelContext(), m_pBox->getName() + CString(" (IBoxAlgorithm::process)"));
#endif
			m_oBenchmarkChronoProcess.stepIn();
			if(!m_pBoxAlgorithm->process(l_oBoxAlgorithmContext))
			{
				getLogManager() << LogLevel_Error << "Box algorithm <" << m_pBox->getName() << "> process() function failed\n";
				return false;
			}
			m_oBenchmarkChronoProcess.stepOut();
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
	vector < deque< CChunk > >::iterator i;
	deque < CChunk >::iterator j;
	vector < CChunk >::iterator k;

	// perform input cleaning
	i=m_vInput.begin();
	while(i!=m_vInput.end())
	{
		j=i->begin();
		while(j!=i->end())
		{
			if(j->isDeprecated())
			{
				j=i->erase(j);
			}
			else
			{
				++j;
			}
		}
		++i;
	}

	// flushes sent output chunks
	i=m_vOutput.begin();
	while(i!=m_vOutput.end())
	{
		i->resize(0);
		++i;
	}

	// discards waiting output chunks
	k=m_vCurrentOutput.begin();
	while(k!=m_vCurrentOutput.end())
	{
		// checks buffer size
		if(k->getBuffer().getSize())
		{
			// the buffer has been (partially ?) filled but not sent
			CBoxAlgorithmContext l_oBoxAlgorithmContext(getKernelContext(), this, m_pBox);
			l_oBoxAlgorithmContext.getPlayerContext()->getLogManager() << LogLevel_Warning << "Output buffer filled but not marked as ready to send\n"; // $$$ should use log
			k->getBuffer().setSize(0, true);
		}

		++k;
	}

	m_bReadyToProcess=false;

#if 1
/*-----------------------------------------------*/
/* TODO send this messages with better frequency */
	if(m_oBenchmarkChronoProcessClock.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process clock" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessClock.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcessInput.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process input" << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcessInput.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
	if(m_oBenchmarkChronoProcess.hasNewEstimation())
	{
		this->getLogManager() << LogLevel_Benchmark
			<< "<" << LogColor_PushStateBit << LogColor_ForegroundBlue << "Player" << LogColor_PopStateBit
			<< "::" << LogColor_PushStateBit << LogColor_ForegroundBlue << "process      " << LogColor_PopStateBit
			<< "::" << m_pBox->getName() << "> "
			<< "Average computing time is " << ((m_oBenchmarkChronoProcess.getAverageStepInDuration()*1000000)>>32) << "us\n";
	}
/* TODO Thank you for reading :)                 */
/*-----------------------------------------------*/
#endif

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
	void __out_of_bound_input(ILogManager& rLogManager, const CString& sName, uint32 ui32InputIndex, uint32 ui32InputCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound input (" << ui32InputIndex << "/" << ui32InputCount << ")\n";
	}

	void __out_of_bound_input_chunk(ILogManager& rLogManager, const CString& sName, uint32 ui32InputIndex, uint32 ui32InputChunkIndex, uint32 ui32InputChunkCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound input (" << ui32InputIndex << ":" << ui32InputChunkIndex << "/" << ui32InputChunkCount << ")\n";
	}
}

// ________________________________________________________________________________________________________________
//

uint32 CSimulatedBox::getInputChunkCount(
	const uint32 ui32InputIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
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
		__out_of_bound_input(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
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
		__out_of_bound_input(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return NULL;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return NULL;
	}
	return &_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).getBuffer();
}

uint64 CSimulatedBox::getInputChunkStartTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getStartTime();
}

uint64 CSimulatedBox::getInputChunkEndTime(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}

	const CChunk& l_rChunk=_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex);
	return l_rChunk.getEndTime();
}

boolean CSimulatedBox::markInputAsDeprecated(
	const uint32 ui32InputIndex,
	const uint32 ui32ChunkIndex)
{
	if(ui32InputIndex>=m_vInput.size())
	{
		__out_of_bound_input(this->getLogManager(), this->getName(), ui32InputIndex, m_vInput.size());
		return false;
	}
	if(ui32ChunkIndex>=m_vInput[ui32InputIndex].size())
	{
		__out_of_bound_input_chunk(this->getLogManager(), this->getName(), ui32InputIndex, ui32ChunkIndex, m_vInput[ui32InputIndex].size());
		return false;
	}
	_my_get_(m_vInput[ui32InputIndex], ui32ChunkIndex).markAsDeprecated(true);
	return true;
}

// ________________________________________________________________________________________________________________
//

namespace
{
	void __out_of_bound_output(ILogManager& rLogManager, const CString& sName, uint32 ui32OutputIndex, uint32 ui32OutputCount)
	{
		rLogManager << LogLevel_ImportantWarning << "<" << sName << "> Access request on out-of-bound output (" << ui32OutputIndex << "/" << ui32OutputCount << ")\n";
	}
}

// ________________________________________________________________________________________________________________
//

uint64 CSimulatedBox::getOutputChunkSize(
	const uint32 ui32OutputIndex) const
{
	if(ui32OutputIndex>=m_vCurrentOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
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
		__out_of_bound_output(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().setSize(ui64Size, bDiscard);
}

uint8* CSimulatedBox::getOutputChunkBuffer(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
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
		__out_of_bound_output(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
		return false;
	}
	return m_vCurrentOutput[ui32OutputIndex].getBuffer().append(pBuffer, ui64BufferSize);
}

IMemoryBuffer* CSimulatedBox::getOutputChunk(
	const uint32 ui32OutputIndex)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		__out_of_bound_output(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
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
		__out_of_bound_output(this->getLogManager(), this->getName(), ui32OutputIndex, m_vCurrentOutput.size());
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

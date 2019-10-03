#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"

using namespace OpenViBE;
using namespace Kernel;

void CBoxAlgorithmLogManager::log(const ELogLevel logLevel)
{
	CIdentifier boxId;
	m_SimulatedBox.getBoxIdentifier(boxId);

	m_LogManager << logLevel << "At time " << time64(m_PlayerContext.getCurrentTime()) << " <" << LogColor_PushStateBit << LogColor_ForegroundBlue
			<< "Box algorithm" << LogColor_PopStateBit << "::" << boxId << " aka " << m_SimulatedBox.getName() << "> ";
}

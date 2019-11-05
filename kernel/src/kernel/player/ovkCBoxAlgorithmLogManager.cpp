#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"

using namespace OpenViBE;
using namespace Kernel;

void CBoxAlgorithmLogManager::log(const ELogLevel logLevel)
{
	CIdentifier boxId;
	m_simulatedBox.getBoxIdentifier(boxId);

	m_logManager << logLevel << "At time " << time64(m_playerCtx.getCurrentTime()) << " <" << LogColor_PushStateBit << LogColor_ForegroundBlue
			<< "Box algorithm" << LogColor_PopStateBit << "::" << boxId << " aka " << m_simulatedBox.getName() << "> ";
}

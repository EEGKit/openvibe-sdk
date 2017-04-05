#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"
#include "ovkCScheduler.h"
#include "ovkCPlayer.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

void CBoxAlgorithmLogManager::log(const OpenViBE::Kernel::ELogLevel logLevel) {
	OpenViBE::CIdentifier boxId;
	m_SimulatedBox.getBoxIdentifier(boxId);

	m_LogManager
	        << logLevel
	        << "At time "
	        << OpenViBE::time64(m_PlayerContext.getCurrentTime())
	        << " <"
	        << OpenViBE::Kernel::LogColor_PushStateBit
	        << OpenViBE::Kernel::LogColor_ForegroundBlue
	        << "Box algorithm"
	        << OpenViBE::Kernel::LogColor_PopStateBit
	        << "::"
	        << boxId
	        << " aka "
	        << m_SimulatedBox.getName()
	        << "> ";
}


#include "ovkCBoxAlgorithmContext.h"
#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"

#include "lepton/Lepton.h"

using namespace OpenViBE;
using namespace Kernel;

CBoxAlgorithmContext::CBoxAlgorithmContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox, const IBox* pBox)
	: TKernelObject<IBoxAlgorithmContext>(rKernelContext)
	  , m_pStaticBoxContext(pBox)
	  , m_pDynamicBoxContext(pSimulatedBox)
	  , m_oPlayerContext(rKernelContext, pSimulatedBox)
	  , m_bReadyToProcess(false) {}

CBoxAlgorithmContext::~CBoxAlgorithmContext() {}

const IBox* CBoxAlgorithmContext::getStaticBoxContext()
{
	return m_pStaticBoxContext;
}

IBoxIO* CBoxAlgorithmContext::getDynamicBoxContext()
{
	return m_pDynamicBoxContext;
}

IPlayerContext* CBoxAlgorithmContext::getPlayerContext()
{
	return &m_oPlayerContext;
}

bool CBoxAlgorithmContext::markAlgorithmAsReadyToProcess()
{
	m_bReadyToProcess = true;
	return true;
}

bool CBoxAlgorithmContext::isAlgorithmReadyToProcess()
{
	return m_bReadyToProcess;
}

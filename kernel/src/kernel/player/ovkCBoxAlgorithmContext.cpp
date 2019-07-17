#include "ovkCBoxAlgorithmContext.h"
#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"

#include "lepton/Lepton.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CBoxAlgorithmContext::CBoxAlgorithmContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox, const IBox* pBox)
	: TKernelObject<IBoxAlgorithmContext>(rKernelContext)
	  , m_pStaticBoxContext(pBox)
	  , m_pDynamicBoxContext(pSimulatedBox)
	  , m_oPlayerContext(rKernelContext, pSimulatedBox)
	  , m_bReadyToProcess(false) {}

CBoxAlgorithmContext::~CBoxAlgorithmContext(void) {}

const IBox* CBoxAlgorithmContext::getStaticBoxContext(void)
{
	return m_pStaticBoxContext;
}

IBoxIO* CBoxAlgorithmContext::getDynamicBoxContext(void)
{
	return m_pDynamicBoxContext;
}

IPlayerContext* CBoxAlgorithmContext::getPlayerContext(void)
{
	return &m_oPlayerContext;
}

bool CBoxAlgorithmContext::markAlgorithmAsReadyToProcess(void)
{
	m_bReadyToProcess = true;
	return true;
}

bool CBoxAlgorithmContext::isAlgorithmReadyToProcess(void)
{
	return m_bReadyToProcess;
}

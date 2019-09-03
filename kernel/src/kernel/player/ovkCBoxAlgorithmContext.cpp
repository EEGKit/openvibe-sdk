#include "ovkCBoxAlgorithmContext.h"
#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"

#include "lepton/Lepton.h"

using namespace OpenViBE;
using namespace Kernel;

CBoxAlgorithmContext::CBoxAlgorithmContext(const IKernelContext& ctx, CSimulatedBox* pSimulatedBox, const IBox* pBox)
	: TKernelObject<IBoxAlgorithmContext>(ctx)
	  , m_pStaticBoxContext(pBox)
	  , m_pDynamicBoxContext(pSimulatedBox)
	  , m_oPlayerContext(ctx, pSimulatedBox) {}

CBoxAlgorithmContext::~CBoxAlgorithmContext() {}

const IBox* CBoxAlgorithmContext::getStaticBoxContext() { return m_pStaticBoxContext; }

IBoxIO* CBoxAlgorithmContext::getDynamicBoxContext() { return m_pDynamicBoxContext; }

IPlayerContext* CBoxAlgorithmContext::getPlayerContext() { return &m_oPlayerContext; }

bool CBoxAlgorithmContext::markAlgorithmAsReadyToProcess()
{
	m_bReadyToProcess = true;
	return true;
}

bool CBoxAlgorithmContext::isAlgorithmReadyToProcess() { return m_bReadyToProcess; }

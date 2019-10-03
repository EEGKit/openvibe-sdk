#include "ovkCBoxAlgorithmContext.h"
#include "ovkCPlayerContext.h"
#include "ovkCSimulatedBox.h"

using namespace OpenViBE;
using namespace Kernel;

CBoxAlgorithmCtx::CBoxAlgorithmCtx(const IKernelContext& ctx, CSimulatedBox* pSimulatedBox, const IBox* pBox)
	: TKernelObject<IBoxAlgorithmContext>(ctx), m_pStaticBoxContext(pBox), m_pDynamicBoxContext(pSimulatedBox), m_oPlayerContext(ctx, pSimulatedBox) {}


bool CBoxAlgorithmCtx::markAlgorithmAsReadyToProcess()
{
	m_bReadyToProcess = true;
	return true;
}

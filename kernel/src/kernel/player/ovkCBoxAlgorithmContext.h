#pragma once

#include "../ovkTKernelObject.h"
#include "ovkCPlayerContext.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CBoxAlgorithmContext : public TKernelObject<IBoxAlgorithmContext>
		{
		public:

			CBoxAlgorithmContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox, const IBox* pBox);
			~CBoxAlgorithmContext() override;
			const IBox* getStaticBoxContext() override;
			IBoxIO* getDynamicBoxContext() override;
			IPlayerContext* getPlayerContext() override;
			bool markAlgorithmAsReadyToProcess() override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxAlgorithmContext>, OVK_ClassId_Kernel_Player_BoxAlgorithmContext)

			virtual bool isAlgorithmReadyToProcess();

		protected:

			const IBox* m_pStaticBoxContext;
			IBoxIO* m_pDynamicBoxContext = nullptr;
			// here we prefer value type over reference/pointer
			// in order to improve performance at runtime (no heap allocation)
			CPlayerContext m_oPlayerContext;
			bool m_bReadyToProcess = false;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



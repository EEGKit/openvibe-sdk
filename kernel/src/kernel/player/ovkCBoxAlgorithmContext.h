#pragma once

#include "../ovkTKernelObject.h"
#include "ovkCPlayerContext.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CBoxAlgorithmContext final : public TKernelObject<IBoxAlgorithmContext>
		{
		public:

			CBoxAlgorithmContext(const IKernelContext& ctx, CSimulatedBox* pSimulatedBox, const IBox* pBox);
			~CBoxAlgorithmContext() override;
			const IBox* getStaticBoxContext() override;
			IBoxIO* getDynamicBoxContext() override;
			IPlayerContext* getPlayerContext() override;
			bool markAlgorithmAsReadyToProcess() override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxAlgorithmContext>, OVK_ClassId_Kernel_Player_BoxAlgorithmContext)

			bool isAlgorithmReadyToProcess();

		protected:

			const IBox* m_pStaticBoxContext = nullptr;
			IBoxIO* m_pDynamicBoxContext    = nullptr;
			// here we prefer value type over reference/pointer
			// in order to improve performance at runtime (no heap allocation)
			CPlayerContext m_oPlayerContext;
			bool m_bReadyToProcess = false;
		};
	} // namespace Kernel
} // namespace OpenViBE

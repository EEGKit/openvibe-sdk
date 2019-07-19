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
			virtual ~CBoxAlgorithmContext();

			virtual const IBox* getStaticBoxContext();
			virtual IBoxIO* getDynamicBoxContext();
			virtual IPlayerContext* getPlayerContext();

			virtual bool markAlgorithmAsReadyToProcess();

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxAlgorithmContext>, OVK_ClassId_Kernel_Player_BoxAlgorithmContext)

			virtual bool isAlgorithmReadyToProcess();

		protected:

			const IBox* m_pStaticBoxContext;
			IBoxIO* m_pDynamicBoxContext;
			// here we prefer value type over reference/pointer
			// in order to improve performance at runtime (no heap allocation)
			CPlayerContext m_oPlayerContext;
			bool m_bReadyToProcess;
		};
	};
};



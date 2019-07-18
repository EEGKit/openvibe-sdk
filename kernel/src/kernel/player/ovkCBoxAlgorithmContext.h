#ifndef __OpenViBEKernel_Kernel_Player_CBoxAlgorithmContext_H__
#define __OpenViBEKernel_Kernel_Player_CBoxAlgorithmContext_H__

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
			virtual ~CBoxAlgorithmContext(void);

			virtual const IBox* getStaticBoxContext(void);
			virtual IBoxIO* getDynamicBoxContext(void);
			virtual IPlayerContext* getPlayerContext(void);

			virtual bool markAlgorithmAsReadyToProcess(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxAlgorithmContext>, OVK_ClassId_Kernel_Player_BoxAlgorithmContext)

			virtual bool isAlgorithmReadyToProcess(void);

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

#endif // __OpenViBEKernel_Kernel_Player_CBoxAlgorithmContext_H__

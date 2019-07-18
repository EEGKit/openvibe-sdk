#ifndef __OpenViBEKernel_Kernel_CKernelObjectFactory_H__
#define __OpenViBEKernel_Kernel_CKernelObjectFactory_H__

#include "ovkTKernelObject.h"

#include <vector>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CKernelObjectFactory : public TKernelObject<IKernelObjectFactory>
		{
		public:

			explicit CKernelObjectFactory(const IKernelContext& rKernelContext);

			virtual IObject* createObject(
				const CIdentifier& rClassIdentifier);
			virtual bool releaseObject(
				IObject* pObject);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObjectFactory, OVK_ClassId_Kernel_KernelObjectFactory)

		protected:

			std::vector<IObject*> m_oCreatedObjects;

			std::mutex m_oMutex;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CObjectFactory_H__

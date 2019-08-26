#pragma once

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
			IObject* createObject(const CIdentifier& rClassIdentifier) override;
			bool releaseObject(IObject* pObject) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObjectFactory, OVK_ClassId_Kernel_KernelObjectFactory)

		protected:

			std::vector<IObject*> m_oCreatedObjects;

			std::mutex m_oMutex;
		};
	} // namespace Kernel
} // namespace OpenViBE

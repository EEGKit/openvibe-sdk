#ifndef __OpenViBEKernel_Tools_CKernelObjectFactoryHelper_H__
#define __OpenViBEKernel_Tools_CKernelObjectFactoryHelper_H__

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Tools
	{
		class CKernelObjectFactoryHelper : public IObject
		{
		public:

			explicit CKernelObjectFactoryHelper(Kernel::IKernelObjectFactory& rKernelObjectFactory)
				: m_rKernelObjectFactory(rKernelObjectFactory) { }

			template <class T>
			T createObject(const CIdentifier& rClassIdentifier)
			{
				IObject* l_pObject = m_rKernelObjectFactory.createObject(rClassIdentifier);
				T l_tResult                  = dynamic_cast<T>(l_pObject);
				if (l_pObject && !l_tResult)
				{
					m_rKernelObjectFactory.releaseObject(l_pObject);
				}
				return l_tResult;
			}

			template <class T>
			bool releaseObject(T tObject)
			{
				return m_rKernelObjectFactory.releaseObject(tObject);
			}

			_IsDerivedFromClass_Final_(OpenViBE::IObject, OVK_ClassId_Tools_KernelObjectFactoryHelper);

		protected:

			Kernel::IKernelObjectFactory& m_rKernelObjectFactory;

		private:

			CKernelObjectFactoryHelper(void);
		};
	};
};

#endif // __OpenViBEKernel_Tools_CKernelObjectFactoryHelper_H__

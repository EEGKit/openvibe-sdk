#pragma once

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Tools
	{
		class CKernelObjectFactoryHelper final : public IObject
		{
		public:

			explicit CKernelObjectFactoryHelper(Kernel::IKernelObjectFactory& rKernelObjectFactory)
				: m_rKernelObjectFactory(rKernelObjectFactory) { }

			template <class T>
			T createObject(const CIdentifier& classID)
			{
				IObject* obj = m_rKernelObjectFactory.createObject(classID);
				T res        = dynamic_cast<T>(obj);
				if (obj && !res) { m_rKernelObjectFactory.releaseObject(obj); }
				return res;
			}

			template <class T>
			bool releaseObject(T tObject) { return m_rKernelObjectFactory.releaseObject(tObject); }

			_IsDerivedFromClass_Final_(OpenViBE::IObject, OVK_ClassId_Tools_KernelObjectFactoryHelper)

		protected:

			Kernel::IKernelObjectFactory& m_rKernelObjectFactory;

		private:

			CKernelObjectFactoryHelper();
		};
	} // namespace Tools
} // namespace OpenViBE

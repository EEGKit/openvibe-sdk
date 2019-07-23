#pragma once

#include "../ovkTKernelObject.h"

#include <map>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy;

		class CAlgorithmManager : public TKernelObject<IAlgorithmManager>
		{
		public:

			explicit CAlgorithmManager(const IKernelContext& rKernelContext);
			virtual ~CAlgorithmManager();

			virtual CIdentifier createAlgorithm(const CIdentifier& rAlgorithmClassIdentifier);
			virtual CIdentifier createAlgorithm(const Plugins::IAlgorithmDesc& rAlgorithmDesc);
			virtual bool releaseAlgorithm(const CIdentifier& rAlgorithmIdentifier);
			virtual bool releaseAlgorithm(IAlgorithmProxy& rAlgorithm);
			virtual IAlgorithmProxy& getAlgorithm(const CIdentifier& rAlgorithmIdentifier);
			virtual CIdentifier getNextAlgorithmIdentifier(const CIdentifier& rPreviousIdentifier) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IAlgorithmManager>, OVK_ClassId_Kernel_Algorithm_AlgorithmManager)

		protected:

			virtual CIdentifier getUnusedIdentifier() const;

		protected:

			using AlgorithmMap = std::map<CIdentifier, CAlgorithmProxy*>;
			AlgorithmMap m_vAlgorithms;

			mutable std::mutex m_oMutex;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



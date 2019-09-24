#pragma once

#include "../ovkTKernelObject.h"

#include <map>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy;

		class CAlgorithmManager final : public TKernelObject<IAlgorithmManager>
		{
		public:

			explicit CAlgorithmManager(const IKernelContext& ctx);
			~CAlgorithmManager() override;
			CIdentifier createAlgorithm(const CIdentifier& algorithmClassID) override;
			CIdentifier createAlgorithm(const Plugins::IAlgorithmDesc& rAlgorithmDesc) override;
			bool releaseAlgorithm(const CIdentifier& rAlgorithmIdentifier) override;
			bool releaseAlgorithm(IAlgorithmProxy& rAlgorithm) override;
			IAlgorithmProxy& getAlgorithm(const CIdentifier& rAlgorithmIdentifier) override;
			CIdentifier getNextAlgorithmIdentifier(const CIdentifier& previousID) const override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IAlgorithmManager>, OVK_ClassId_Kernel_Algorithm_AlgorithmManager)

		protected:

			CIdentifier getUnusedIdentifier() const;

			using AlgorithmMap = std::map<CIdentifier, CAlgorithmProxy*>;
			AlgorithmMap m_vAlgorithms;

			mutable std::mutex m_oMutex;
		};
	} // namespace Kernel
} // namespace OpenViBE

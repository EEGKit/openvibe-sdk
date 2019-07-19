#pragma once

#include <openvibe/kernel/scenario/ovIMetadata.h>
#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CMetadata final : public TKernelObject<IMetadata>
		{
		public:

			CMetadata(const IKernelContext& kernelContext, CScenario& ownerScenario);
			~CMetadata();

			CIdentifier getIdentifier() const override;
			CIdentifier getType() const override;
			CString getData() const override;
			bool setIdentifier(const CIdentifier& identifier) override;
			bool setType(const CIdentifier& identifier) override;
			bool setData(const CString& data) override;

			bool initializeFromExistingMetadata(const IMetadata& exisitingMetadata) override;

			bool acceptVisitor(IObjectVisitor& objectVisitor) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMetadata>, OVK_ClassId_Kernel_Scenario_Metadata)

		private:

			CScenario& m_OwnerScenario;

			CIdentifier m_Identifier;
			CIdentifier m_Type;
			CString m_Data;
		};
	}
}

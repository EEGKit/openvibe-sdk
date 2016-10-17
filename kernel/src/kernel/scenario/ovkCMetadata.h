#pragma once

#include <openvibe/kernel/scenario/ovIMetadata.h>
#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CMetadata final : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMetadata>
		{
		public:

			CMetadata(const OpenViBE::Kernel::IKernelContext& kernelContext, OpenViBE::Kernel::CScenario& ownerScenario);
			~CMetadata(void);

			OpenViBE::CIdentifier getIdentifier(void) const override;
			OpenViBE::CIdentifier getType(void) const override;
			OpenViBE::CString getData(void) const override;
			bool setIdentifier(const OpenViBE::CIdentifier& identifier) override;
			bool setType(const OpenViBE::CIdentifier& identifier) override;
			bool setData(const OpenViBE::CString& data) override;

			bool initializeFromExistingMetadata(const OpenViBE::Kernel::IMetadata& exisitingMetadata) override;

			bool acceptVisitor(OpenViBE::IObjectVisitor& objectVisitor) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMetadata>, OVK_ClassId_Kernel_Scenario_Metadata)

		private:

			OpenViBE::Kernel::CScenario& m_OwnerScenario;

			OpenViBE::CIdentifier m_Identifier;
			OpenViBE::CIdentifier m_Type;
			OpenViBE::CString m_Data;
		};
	}
}


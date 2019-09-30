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

			CMetadata(const IKernelContext& ctx, CScenario& ownerScenario);
			~CMetadata() override;

			CIdentifier getIdentifier() const override;
			CIdentifier getType() const override;
			CString getData() const override;
			bool setIdentifier(const CIdentifier& identifier) override;
			bool setType(const CIdentifier& typeID) override;
			bool setData(const CString& data) override;

			bool initializeFromExistingMetadata(const IMetadata& existingMetadata) override;

			bool acceptVisitor(IObjectVisitor& objectVisitor) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMetadata>, OVK_ClassId_Kernel_Scenario_Metadata)

		private:

			CScenario& m_OwnerScenario;

			CIdentifier m_id = OV_UndefinedIdentifier;
			CIdentifier m_Type       = OV_UndefinedIdentifier;
			CString m_Data;
		};
	} // namespace Kernel
} // namespace OpenViBE

#pragma once

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CLink : public TAttributable<TKernelObject<ILink>>
		{
		public:

			CLink(const IKernelContext& rKernelContext, CScenario& rOwnerScenario);
			bool InitializeFromExistingLink(const ILink& link) override;
			bool setIdentifier(const CIdentifier& rIdentifier) override;
			CIdentifier getIdentifier() const override;
			bool setSource(const CIdentifier& rBoxIdentifier, uint32_t ui32BoxOutputIndex, CIdentifier rBoxOutputIdentifier) override;
			bool setTarget(const CIdentifier& rBoxIdentifier, uint32_t ui32BoxInputIndex, CIdentifier rBoxInputIdentifier) override;
			bool getSource(CIdentifier& rBoxIdentifier, uint32_t& ui32BoxOutputIndex, CIdentifier& rBoxOutputIdentifier) const override;
			CIdentifier getSourceBoxIdentifier() const override;
			uint32_t getSourceBoxOutputIndex() const override;
			CIdentifier getSourceBoxOutputIdentifier() const override;
			bool getTarget(CIdentifier& rTargetBoxIdentifier, uint32_t& ui32BoxInputIndex, CIdentifier& rTargetBoxInputIdentifier) const override;
			CIdentifier getTargetBoxIdentifier() const override;
			uint32_t getTargetBoxInputIndex() const override;
			CIdentifier getTargetBoxInputIdentifier() const override;
			bool acceptVisitor(IObjectVisitor& rObjectVisitor) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::ILink > >, OVK_ClassId_Kernel_Scenario_Link)

		protected:

			CScenario& m_rOwnerScenario;
			CIdentifier m_oIdentifier                = OV_UndefinedIdentifier;
			CIdentifier m_oSourceBoxIdentifier       = OV_UndefinedIdentifier;
			CIdentifier m_oTargetBoxIdentifier       = OV_UndefinedIdentifier;
			uint32_t m_ui32SourceOutputIndex         = 0;
			CIdentifier m_oSourceBoxOutputIdentifier = OV_UndefinedIdentifier;
			uint32_t m_ui32TargetInputIndex          = 0;
			CIdentifier m_oTargetBoxInputIdentifier  = OV_UndefinedIdentifier;
		};
	}  // namespace Kernel
}  // namespace OpenViBE

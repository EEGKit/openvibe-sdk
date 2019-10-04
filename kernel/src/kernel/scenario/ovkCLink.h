#pragma once

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CLink final : public TAttributable<TKernelObject<ILink>>
		{
		public:

			CLink(const IKernelContext& ctx, CScenario& rOwnerScenario);
			bool initializeFromExistingLink(const ILink& link) override;
			bool setIdentifier(const CIdentifier& identifier) override;
			CIdentifier getIdentifier() const override;
			bool setSource(const CIdentifier& boxId, uint32_t boxOutputIdx, CIdentifier boxOutputID) override;
			bool setTarget(const CIdentifier& boxId, uint32_t boxInputIdx, CIdentifier boxInputID) override;
			bool getSource(CIdentifier& boxId, uint32_t& boxOutputIdx, CIdentifier& boxOutputID) const override;
			CIdentifier getSourceBoxIdentifier() const override;
			uint32_t getSourceBoxOutputIndex() const override;
			CIdentifier getSourceBoxOutputIdentifier() const override;
			bool getTarget(CIdentifier& rTargetBoxIdentifier, uint32_t& ui32BoxInputIndex, CIdentifier& rTargetBoxInputIdentifier) const override;
			CIdentifier getTargetBoxIdentifier() const override;
			uint32_t getTargetBoxInputIndex() const override;
			CIdentifier getTargetBoxInputIdentifier() const override;
			bool acceptVisitor(IObjectVisitor& rObjectVisitor) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::ILink > >,
									   OVK_ClassId_Kernel_Scenario_Link)

		protected:

			CScenario& m_rOwnerScenario;
			CIdentifier m_id                = OV_UndefinedIdentifier;
			CIdentifier m_oSourceBoxID       = OV_UndefinedIdentifier;
			CIdentifier m_oTargetBoxID       = OV_UndefinedIdentifier;
			uint32_t m_ui32SourceOutputIdx         = 0;
			CIdentifier m_oSourceBoxOutputID = OV_UndefinedIdentifier;
			uint32_t m_ui32TargetInputIdx          = 0;
			CIdentifier m_oTargetBoxInputID  = OV_UndefinedIdentifier;
		};
	} // namespace Kernel
} // namespace OpenViBE

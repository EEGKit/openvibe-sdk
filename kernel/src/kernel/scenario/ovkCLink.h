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

			CLink(const IKernelContext& ctx, CScenario& ownerScenario);
			bool initializeFromExistingLink(const ILink& link) override;
			bool setIdentifier(const CIdentifier& identifier) override;
			CIdentifier getIdentifier() const override;
			bool setSource(const CIdentifier& boxId, size_t boxOutputIdx, CIdentifier boxOutputID) override;
			bool setTarget(const CIdentifier& boxId, size_t boxInputIdx, CIdentifier boxInputID) override;
			bool getSource(CIdentifier& boxId, size_t& boxOutputIdx, CIdentifier& boxOutputID) const override;
			CIdentifier getSourceBoxIdentifier() const override;
			size_t getSourceBoxOutputIndex() const override;
			CIdentifier getSourceBoxOutputIdentifier() const override;
			bool getTarget(CIdentifier& dstBoxID, size_t& boxInputIndex, CIdentifier& dstBoxInputID) const override;
			CIdentifier getTargetBoxIdentifier() const override;
			size_t getTargetBoxInputIndex() const override;
			CIdentifier getTargetBoxInputIdentifier() const override;
			bool acceptVisitor(IObjectVisitor& visitor) override;

			_IsDerivedFromClass_Final_(TAttributable<TKernelObject<ILink>>, OVK_ClassId_Kernel_Scenario_Link)

		protected:

			CScenario& m_ownerScenario;
			CIdentifier m_id             = OV_UndefinedIdentifier;
			CIdentifier m_srcBoxID       = OV_UndefinedIdentifier;
			CIdentifier m_dstBoxID       = OV_UndefinedIdentifier;
			size_t m_srcOutputIdx        = 0;
			CIdentifier m_srcBoxOutputID = OV_UndefinedIdentifier;
			size_t m_dstInputIdx         = 0;
			CIdentifier m_dstBoxInputID  = OV_UndefinedIdentifier;
		};
	} // namespace Kernel
} // namespace OpenViBE

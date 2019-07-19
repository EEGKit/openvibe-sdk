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

			virtual bool InitializeFromExistingLink(const ILink& link);

			virtual bool setIdentifier(
				const CIdentifier& rIdentifier);
			virtual CIdentifier getIdentifier(void) const;

			virtual bool setSource(
				const CIdentifier& rBoxIdentifier,
				uint32_t ui32BoxOutputIndex,
				CIdentifier rBoxOutputIdentifier);
			virtual bool setTarget(
				const CIdentifier& rBoxIdentifier,
				uint32_t ui32BoxInputIndex,
				CIdentifier rBoxInputIdentifier);
			virtual bool getSource(
				CIdentifier& rBoxIdentifier,
				uint32_t& ui32BoxOutputIndex,
				CIdentifier& rBoxOutputIdentifier) const;
			virtual CIdentifier getSourceBoxIdentifier(void) const;
			virtual uint32_t getSourceBoxOutputIndex(void) const;
			virtual CIdentifier getSourceBoxOutputIdentifier(void) const;
			virtual bool getTarget(
				CIdentifier& rTargetBoxIdentifier,
				uint32_t& ui32BoxInputIndex,
				CIdentifier& rTargetBoxInputIdentifier) const;
			virtual CIdentifier getTargetBoxIdentifier(void) const;
			virtual uint32_t getTargetBoxInputIndex(void) const;
			virtual CIdentifier getTargetBoxInputIdentifier(void) const;


			virtual bool acceptVisitor(
				IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::ILink > >, OVK_ClassId_Kernel_Scenario_Link)

		protected:

			CScenario& m_rOwnerScenario;
			CIdentifier m_oIdentifier;
			CIdentifier m_oSourceBoxIdentifier;
			CIdentifier m_oTargetBoxIdentifier;
			uint32_t m_ui32SourceOutputIndex;
			CIdentifier m_oSourceBoxOutputIdentifier;
			uint32_t m_ui32TargetInputIndex;
			CIdentifier m_oTargetBoxInputIdentifier;
		};
	};
};



#ifndef __OpenViBEKernel_Kernel_Scenario_CLink_H__
#define __OpenViBEKernel_Kernel_Scenario_CLink_H__

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CLink : public OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILink>>
		{
		public:

			CLink(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CScenario& rOwnerScenario);

			virtual bool InitializeFromExistingLink(const ILink& link);

			virtual bool setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier);
			virtual OpenViBE::CIdentifier getIdentifier(void) const;

			virtual bool setSource(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32BoxOutputIndex,
				const OpenViBE::CIdentifier rBoxOutputIdentifier);
			virtual bool setTarget(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32BoxInputIndex,
				const OpenViBE::CIdentifier rBoxInputIdentifier);
			virtual bool getSource(
				OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32& ui32BoxOutputIndex,
				OpenViBE::CIdentifier& rBoxOutputIdentifier) const;
			virtual OpenViBE::CIdentifier getSourceBoxIdentifier(void) const;
			virtual OpenViBE::uint32 getSourceBoxOutputIndex(void) const;
			virtual OpenViBE::CIdentifier getSourceBoxOutputIdentifier(void) const;
			virtual bool getTarget(
				OpenViBE::CIdentifier& rTargetBoxIdentifier,
				OpenViBE::uint32& ui32BoxInputIndex,
				OpenViBE::CIdentifier& rTargetBoxInputIdentifier) const;
			virtual OpenViBE::CIdentifier getTargetBoxIdentifier(void) const;
			virtual OpenViBE::uint32 getTargetBoxInputIndex(void) const;
			virtual OpenViBE::CIdentifier getTargetBoxInputIdentifier(void) const;


			virtual bool acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::ILink > >, OVK_ClassId_Kernel_Scenario_Link)

		protected:

			OpenViBE::Kernel::CScenario& m_rOwnerScenario;
			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oSourceBoxIdentifier;
			OpenViBE::CIdentifier m_oTargetBoxIdentifier;
			OpenViBE::uint32 m_ui32SourceOutputIndex;
			OpenViBE::CIdentifier m_oSourceBoxOutputIdentifier;
			OpenViBE::uint32 m_ui32TargetInputIndex;
			OpenViBE::CIdentifier m_oTargetBoxInputIdentifier;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CLink_H__

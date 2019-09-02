#pragma once

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

#include <vector>
#include <string>
#include <iostream>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CComment : public TAttributable<TKernelObject<IComment>>
		{
		public:

			CComment(const IKernelContext& rKernelContext, CScenario& rOwnerScenario);
			~CComment() override;
			CIdentifier getIdentifier() const override;
			CString getText() const override;
			bool setIdentifier(const CIdentifier& rIdentifier) override;
			bool setText(const CString& sText) override;
			bool initializeFromExistingComment(const IComment& rExisitingComment) override;
			bool acceptVisitor(IObjectVisitor& rObjectVisitor) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IComment > >,
									   OVK_ClassId_Kernel_Scenario_Comment)

		protected:

			CScenario& m_rOwnerScenario;

			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			CString m_sText;
		};
	} // namespace Kernel
} // namespace OpenViBE

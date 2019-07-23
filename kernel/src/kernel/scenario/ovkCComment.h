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
			virtual ~CComment();

			virtual CIdentifier getIdentifier() const;
			virtual CString getText() const;
			virtual bool setIdentifier(const CIdentifier& rIdentifier);
			virtual bool setText(const CString& sText);

			virtual bool initializeFromExistingComment(const IComment& rExisitingComment);

			virtual bool acceptVisitor(IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IComment > >, OVK_ClassId_Kernel_Scenario_Comment)

		protected:

			CScenario& m_rOwnerScenario;

			CIdentifier m_oIdentifier;
			CString m_sText;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



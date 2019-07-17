#ifndef __OpenViBEKernel_Kernel_Scenario_CComment_H__
#define __OpenViBEKernel_Kernel_Scenario_CComment_H__

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

		class CComment : public OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IComment>>
		{
		public:

			CComment(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CScenario& rOwnerScenario);
			virtual ~CComment(void);

			virtual OpenViBE::CIdentifier getIdentifier(void) const;
			virtual OpenViBE::CString getText(void) const;
			virtual bool setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier);
			virtual bool setText(
				const OpenViBE::CString& sText);

			virtual bool initializeFromExistingComment(
				const OpenViBE::Kernel::IComment& rExisitingComment);

			virtual bool acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IComment > >, OVK_ClassId_Kernel_Scenario_Comment)

		protected:

			OpenViBE::Kernel::CScenario& m_rOwnerScenario;

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CString m_sText;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CComment_H__

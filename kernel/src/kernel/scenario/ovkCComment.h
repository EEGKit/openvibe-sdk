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

		class CComment : public TAttributable<TKernelObject<IComment>>
		{
		public:

			CComment(const IKernelContext& rKernelContext, CScenario& rOwnerScenario);
			virtual ~CComment(void);

			virtual CIdentifier getIdentifier(void) const;
			virtual CString getText(void) const;
			virtual bool setIdentifier(
				const CIdentifier& rIdentifier);
			virtual bool setText(
				const CString& sText);

			virtual bool initializeFromExistingComment(
				const IComment& rExisitingComment);

			virtual bool acceptVisitor(
				IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IComment > >, OVK_ClassId_Kernel_Scenario_Comment)

		protected:

			CScenario& m_rOwnerScenario;

			CIdentifier m_oIdentifier;
			CString m_sText;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CComment_H__

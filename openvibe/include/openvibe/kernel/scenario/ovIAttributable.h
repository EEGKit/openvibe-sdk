#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IAttributable
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-12-07
		 * \brief
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 */
		class OV_API IAttributable : public IKernelObject
		{
		public:

			virtual bool addAttribute(const CIdentifier& rAttributeIdentifier, const CString& sAttributeValue) = 0;
			virtual bool removeAttribute(const CIdentifier& rAttributeIdentifier) = 0;
			virtual bool removeAllAttributes() = 0;

			virtual CString getAttributeValue(const CIdentifier& rAttributeIdentifier) const = 0;
			virtual bool setAttributeValue(const CIdentifier& rAttributeIdentifier, const CString& sAttributeValue) = 0;

			virtual bool hasAttribute(const CIdentifier& rAttributeIdentifier) const = 0;
			virtual bool hasAttributes() const = 0;

			virtual CIdentifier getNextAttributeIdentifier(const CIdentifier& rPreviousIdentifier) const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_Attributable)
		};
	};
};

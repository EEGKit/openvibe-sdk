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

			virtual bool addAttribute(const CIdentifier& id, const CString& value) = 0;
			virtual bool removeAttribute(const CIdentifier& id) = 0;
			virtual bool removeAllAttributes() = 0;

			virtual CString getAttributeValue(const CIdentifier& id) const = 0;
			virtual bool setAttributeValue(const CIdentifier& id, const CString& value) = 0;

			virtual bool hasAttribute(const CIdentifier& id) const = 0;
			virtual bool hasAttributes() const = 0;

			virtual CIdentifier getNextAttributeIdentifier(const CIdentifier& previousID) const = 0;

			_IsDerivedFromClass_(IKernelObject, OV_ClassId_Kernel_Scenario_Attributable)
		};
	} // namespace Kernel
} // namespace OpenViBE

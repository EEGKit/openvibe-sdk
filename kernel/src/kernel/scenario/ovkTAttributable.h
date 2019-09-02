#pragma once

#include "../../ovk_base.h"
#include "../../ovk_tools.h"

#include <openvibe/ov_all.h>

#include <map>
#include <iostream>

namespace OpenViBE
{
	namespace Kernel
	{
		template <class T>
		class TAttributable : public T
		{
		public:

			explicit TAttributable(const IKernelContext& rKernelContext)
				: T(rKernelContext) { }

			virtual bool addAttribute(const CIdentifier& attributeID, const CString& sAttributeValue)
			{
				const std::map<CIdentifier, CString>::iterator itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute != m_vAttribute.end()) { return false; }
				m_vAttribute[attributeID] = sAttributeValue;
				return true;
			}

			virtual bool removeAttribute(const CIdentifier& attributeID)
			{
				const std::map<CIdentifier, CString>::iterator itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end()) { return false; }
				m_vAttribute.erase(itAttribute);
				return true;
			}

			virtual bool removeAllAttributes()
			{
				m_vAttribute.clear();
				return true;
			}

			virtual CString getAttributeValue(const CIdentifier& attributeID) const
			{
				const std::map<CIdentifier, CString>::const_iterator itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end()) { return CString(""); }
				return itAttribute->second;
			}

			virtual bool setAttributeValue(const CIdentifier& attributeID, const CString& sAttributeValue)
			{
				std::map<CIdentifier, CString>::iterator itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end())
				{
					m_vAttribute[attributeID] = sAttributeValue;
					return true;
				}
				itAttribute->second = sAttributeValue;
				return true;
			}

			virtual bool hasAttribute(const CIdentifier& attributeID) const
			{
				const std::map<CIdentifier, CString>::const_iterator itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end()) { return false; }
				return true;
			}

			virtual bool hasAttributes() const { return !m_vAttribute.empty(); }

			virtual CIdentifier getNextAttributeIdentifier(const CIdentifier& previousID) const
			{
				return getNextIdentifier<CString>(m_vAttribute, previousID);
			}

			_IsDerivedFromClass_(T, OVK_ClassId_Kernel_Scenario_AttributableT)

		protected:

			std::map<CIdentifier, CString> m_vAttribute;
		};
	} // namespace Kernel
} // namespace OpenViBE

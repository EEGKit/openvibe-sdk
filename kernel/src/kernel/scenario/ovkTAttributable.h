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

			explicit TAttributable(const IKernelContext& ctx)
				: T(ctx) { }

			bool addAttribute(const CIdentifier& attributeID, const CString& sAttributeValue) override
			{
				const auto itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute != m_vAttribute.end()) { return false; }
				m_vAttribute[attributeID] = sAttributeValue;
				return true;
			}

			bool removeAttribute(const CIdentifier& attributeID) override
			{
				const auto itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end()) { return false; }
				m_vAttribute.erase(itAttribute);
				return true;
			}

			bool removeAllAttributes() override
			{
				m_vAttribute.clear();
				return true;
			}

			CString getAttributeValue(const CIdentifier& attributeID) const override
			{
				const auto itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end()) { return CString(""); }
				return itAttribute->second;
			}

			bool setAttributeValue(const CIdentifier& attributeID, const CString& value) override
			{
				auto itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end())
				{
					m_vAttribute[attributeID] = value;
					return true;
				}
				itAttribute->second = value;
				return true;
			}

			bool hasAttribute(const CIdentifier& attributeID) const override
			{
				const auto itAttribute = m_vAttribute.find(attributeID);
				if (itAttribute == m_vAttribute.end()) { return false; }
				return true;
			}

			bool hasAttributes() const override { return !m_vAttribute.empty(); }

			CIdentifier getNextAttributeIdentifier(const CIdentifier& previousID) const override { return getNextIdentifier<CString>(m_vAttribute, previousID); }

			_IsDerivedFromClass_(T, OVK_ClassId_Kernel_Scenario_AttributableT)

		protected:

			std::map<CIdentifier, CString> m_vAttribute;
		};
	} // namespace Kernel
} // namespace OpenViBE

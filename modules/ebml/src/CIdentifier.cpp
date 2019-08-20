#include "ebml/CIdentifier.h"

using namespace EBML;

CIdentifier::CIdentifier() {}

CIdentifier::CIdentifier(const uint64_t ui64Identifier)
	: m_ui64Identifier(ui64Identifier) {}

CIdentifier::CIdentifier(const uint32_t ui32Identifier1, const uint32_t ui32Identifier2)
{
	m_ui64Identifier = (((uint64_t)ui32Identifier1) << 32) + ui32Identifier2;
}

CIdentifier::CIdentifier(const CIdentifier& rIdentifier)
	: m_ui64Identifier(rIdentifier.m_ui64Identifier) {}

const CIdentifier& CIdentifier::operator=(const CIdentifier& rIdentifier)
{
	m_ui64Identifier = rIdentifier.m_ui64Identifier;
	return *this;
}

namespace EBML
{
	bool operator==(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier == rIdentifier2.m_ui64Identifier;
	}

	bool operator!=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier != rIdentifier2.m_ui64Identifier;
	}

	bool operator<=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier <= rIdentifier2.m_ui64Identifier;
	}

	bool operator>=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier >= rIdentifier2.m_ui64Identifier;
	}

	bool operator<(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier < rIdentifier2.m_ui64Identifier;
	}

	bool operator>(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2) { return rIdentifier1.m_ui64Identifier > rIdentifier2.m_ui64Identifier; }
}  // namespace EBML

CIdentifier::operator uint64_t() const { return this->toUInteger(); }

uint64_t CIdentifier::toUInteger() const { return m_ui64Identifier; }

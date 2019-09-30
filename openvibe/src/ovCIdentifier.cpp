#include "ovCIdentifier.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <random>

using namespace OpenViBE;

CIdentifier::CIdentifier()
	: m_ui64Identifier(0xffffffffffffffffLL) {}

CIdentifier::CIdentifier(const uint32_t id1, const uint32_t id2)
	: m_ui64Identifier((uint64_t(id1) << 32) + id2) {}

CIdentifier::CIdentifier(const uint64_t id)
	: m_ui64Identifier(id) {}

CIdentifier::CIdentifier(const CIdentifier& id)
	: m_ui64Identifier(id.m_ui64Identifier) {}

CIdentifier& CIdentifier::operator=(const CIdentifier& id)
{
	m_ui64Identifier = id.m_ui64Identifier;
	return *this;
}

CIdentifier& CIdentifier::operator++()
{
	if (m_ui64Identifier != 0xffffffffffffffffLL)
	{
		m_ui64Identifier++;
		if (m_ui64Identifier == 0xffffffffffffffffLL) { m_ui64Identifier = 0LL; }
	}
	return *this;
}

CIdentifier& CIdentifier::operator--()
{
	if (m_ui64Identifier != 0xffffffffffffffffLL)
	{
		m_ui64Identifier--;
		if (m_ui64Identifier == 0xffffffffffffffffLL) { m_ui64Identifier = 0xfffffffffffffffeLL; }
	}
	return *this;
}

namespace OpenViBE
{
	bool operator==(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_ui64Identifier == id2.m_ui64Identifier; }

	bool operator!=(const CIdentifier& id1, const CIdentifier& id2) { return !(id1 == id2); }

	bool operator<(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_ui64Identifier < id2.m_ui64Identifier; }

	bool operator>(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_ui64Identifier > id2.m_ui64Identifier; }
} // namespace OpenViBE

CString CIdentifier::toString() const
{
	char l_sBuffer[1024];
	uint32_t l_uiIdentifier1 = uint32_t(m_ui64Identifier >> 32);
	uint32_t l_uiIdentifier2 = uint32_t(m_ui64Identifier);
	sprintf(l_sBuffer, "(0x%08x, 0x%08x)", l_uiIdentifier1, l_uiIdentifier2);
	return CString(l_sBuffer);
}

bool CIdentifier::fromString(const CString& str)
{
	const char* buffer = str;
	uint32_t id1;
	uint32_t id2;
	if (sscanf(buffer, "(0x%x, 0x%x)", &id1, &id2) != 2) { return false; }
	m_ui64Identifier = (uint64_t(id1) << 32) + id2;
	return true;
}

uint64_t CIdentifier::toUInteger() const { return m_ui64Identifier; }

CIdentifier CIdentifier::random()
{
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::uniform_int_distribution<uint64_t> uni(0, std::numeric_limits<uint64_t>::max() - 1); // This exclude OV_UndefinedIdentifier value
	return CIdentifier(uni(rng));
}

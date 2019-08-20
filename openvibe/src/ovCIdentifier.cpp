#include "ovCIdentifier.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <random>

using namespace OpenViBE;

CIdentifier::CIdentifier()
	: m_ui64Identifier(0xffffffffffffffffll) {}

CIdentifier::CIdentifier(const uint32_t ui32Identifier1, const uint32_t ui32Identifier2)
	: m_ui64Identifier((uint64_t(ui32Identifier1) << 32) + ui32Identifier2) {}

CIdentifier::CIdentifier(const uint64_t ui64Identifier)
	: m_ui64Identifier(ui64Identifier) {}

CIdentifier::CIdentifier(const CIdentifier& rIdentifier)
	: m_ui64Identifier(rIdentifier.m_ui64Identifier) {}

CIdentifier& CIdentifier::operator=(const CIdentifier& rIdentifier)
{
	m_ui64Identifier = rIdentifier.m_ui64Identifier;
	return *this;
}

CIdentifier& CIdentifier::operator++()
{
	if (m_ui64Identifier != 0xffffffffffffffffll)
	{
		m_ui64Identifier++;
		if (m_ui64Identifier == 0xffffffffffffffffll)
		{
			m_ui64Identifier = 0ll;
		}
	}
	return *this;
}

CIdentifier& CIdentifier::operator--()
{
	if (m_ui64Identifier != 0xffffffffffffffffll)
	{
		m_ui64Identifier--;
		if (m_ui64Identifier == 0xffffffffffffffffll)
		{
			m_ui64Identifier = 0xfffffffffffffffell;
		}
	}
	return *this;
}

namespace OpenViBE
{
	bool operator==(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier == rIdentifier2.m_ui64Identifier;
	}

	bool operator!=(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2) { return !(rIdentifier1 == rIdentifier2); }

	bool operator<(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2)
	{
		return rIdentifier1.m_ui64Identifier < rIdentifier2.m_ui64Identifier;
	}

	bool operator>(const CIdentifier& rIdentifier1, const CIdentifier& rIdentifier2) { return rIdentifier1.m_ui64Identifier > rIdentifier2.m_ui64Identifier; }
}  // namespace OpenViBE

CString CIdentifier::toString() const
{
	char l_sBuffer[1024];
	unsigned int l_uiIdentifier1 = (unsigned int)(m_ui64Identifier >> 32);
	unsigned int l_uiIdentifier2 = (unsigned int)(m_ui64Identifier);
	sprintf(l_sBuffer, "(0x%08x, 0x%08x)", l_uiIdentifier1, l_uiIdentifier2);
	return CString(l_sBuffer);
}

bool CIdentifier::fromString(const CString& rString)
{
	const char* l_sBuffer = rString;
	unsigned int l_uiIdentifier1;
	unsigned int l_uiIdentifier2;
	if (sscanf(l_sBuffer, "(0x%x, 0x%x)", &l_uiIdentifier1, &l_uiIdentifier2) != 2) { return false; }
	m_ui64Identifier = (((uint64_t)l_uiIdentifier1) << 32) + l_uiIdentifier2;
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

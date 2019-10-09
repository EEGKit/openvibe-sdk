#include "ovCIdentifier.h"

#include <cstdio>
#include <random>

using namespace OpenViBE;

CIdentifier& CIdentifier::operator=(const CIdentifier& id)
{
	m_id = id.m_id;
	return *this;
}

CIdentifier& CIdentifier::operator++()
{
	if (m_id != ULLONG_MAX)
	{
		m_id++;
		if (m_id == ULLONG_MAX) { m_id = 0; }
	}
	return *this;
}

CIdentifier& CIdentifier::operator--()
{
	if (m_id != ULLONG_MAX)
	{
		m_id--;
		if (m_id == ULLONG_MAX) { m_id = ULLONG_MAX - 1; }
	}
	return *this;
}

namespace OpenViBE
{
	bool operator==(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id == id2.m_id; }

	bool operator!=(const CIdentifier& id1, const CIdentifier& id2) { return !(id1 == id2); }

	bool operator<(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id < id2.m_id; }

	bool operator>(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id > id2.m_id; }
} // namespace OpenViBE

CString CIdentifier::toString() const
{
	char buffer[1024];
	const uint32_t id1 = uint32_t(m_id >> 32);
	const uint32_t id2 = uint32_t(m_id);
	sprintf(buffer, "(0x%08x, 0x%08x)", id1, id2);
	return CString(buffer);
}

bool CIdentifier::fromString(const CString& str)
{
	const char* buffer = str;
	uint32_t id1;
	uint32_t id2;
	if (sscanf(buffer, "(0x%x, 0x%x)", &id1, &id2) != 2) { return false; }
	m_id = (uint64_t(id1) << 32) + id2;
	return true;
}

CIdentifier CIdentifier::random()
{
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::uniform_int_distribution<size_t> uni(0, std::numeric_limits<size_t>::max() - 1); // This exclude OV_UndefinedIdentifier value
	return CIdentifier(uni(rng));
}

#include "ovCIdentifier.h"

#include <cstdio>
#include <random>
#include <climits>
#include <iomanip>
#include <sstream>

using namespace OpenViBE;

CIdentifier::CIdentifier(): m_id(ULLONG_MAX) { }
CIdentifier::CIdentifier(const uint32_t id1, const uint32_t id2): m_id((uint64_t(id1) << 32) + id2) { }
CIdentifier::CIdentifier(const uint64_t id): m_id(id) { }
CIdentifier::CIdentifier(const CIdentifier& id): m_id(id.m_id) { }

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

CString CIdentifier::toString() const
{
	std::stringstream ss;
	ss.fill('0');
	ss << "(0x" << std::setw(8) << std::hex << uint32_t(m_id >> 32) << ", 0x" << std::setw(8) << std::hex << uint32_t(m_id) << ")";
	return CString(ss.str().c_str());
}

bool CIdentifier::fromString(const CString& str)
{
	const std::string s = str.toASCIIString();
	if (s.size() != 24) { return false; }
	std::stringstream ss;
	uint64_t id1, id2;
	ss << std::hex << s.substr(3, 8);
	ss >> id1;
	ss.str("");
	ss.clear();
	ss << std::hex << s.substr(15, 8);
	ss >> id2;
	m_id = (id1 << 32) + id2;
	return true;
}

CIdentifier CIdentifier::random()
{
	std::random_device rd;
	std::default_random_engine rng(rd());
	std::uniform_int_distribution<uint64_t> uni(0, std::numeric_limits<uint64_t>::max() - 1); // This exclude OV_UndefinedIdentifier value
	return CIdentifier(uni(rng));
}

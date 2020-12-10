#include "ebml/CIdentifier.h"

using namespace EBML;

const CIdentifier& CIdentifier::operator=(const CIdentifier& id)
{
	m_id = id.m_id;
	return *this;
}

namespace EBML {
bool operator==(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id == id2.m_id; }
bool operator!=(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id != id2.m_id; }
bool operator<=(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id <= id2.m_id; }
bool operator>=(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id >= id2.m_id; }
bool operator<(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id < id2.m_id; }
bool operator>(const CIdentifier& id1, const CIdentifier& id2) { return id1.m_id > id2.m_id; }
}  // namespace EBML

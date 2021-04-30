#include "ovCStimulationSet.h"

#include <vector>

namespace OpenViBE {
namespace {
class CStimulation
{
public:

	CStimulation() { }

	CStimulation(const uint64_t id, const uint64_t date, const uint64_t duration)
		: m_Id(id), m_Date(date), m_Duration(duration) { }

	uint64_t m_Id       = 0;
	uint64_t m_Date     = 0;
	uint64_t m_Duration = 0;
};

class CStimulationSetImpl final : public IStimulationSet
{
public:
	void clear() override { m_stimulations.clear(); }
	size_t getStimulationCount() const override { return m_stimulations.size(); }
	uint64_t getStimulationIdentifier(const size_t index) const override { return m_stimulations[index].m_Id; }
	uint64_t getStimulationDate(const size_t index) const override { return m_stimulations[index].m_Date; }
	uint64_t getStimulationDuration(const size_t index) const override { return m_stimulations[index].m_Duration; }
	bool setStimulationCount(const size_t n) override;
	bool setStimulationIdentifier(const size_t index, const uint64_t id) override;
	bool setStimulationDate(const size_t index, const uint64_t date) override;
	bool setStimulationDuration(const size_t index, const uint64_t duration) override;
	size_t appendStimulation(const uint64_t id, const uint64_t date, const uint64_t duration) override;
	size_t insertStimulation(const size_t index, const uint64_t id, const uint64_t date, const uint64_t duration) override;
	bool removeStimulation(const size_t index) override;

	_IsDerivedFromClass_Final_(IStimulationSet, OV_ClassId_StimulationSetImpl)

private:

	std::vector<CStimulation> m_stimulations;
};
}  // namespace

// ________________________________________________________________________________________________________________
//

bool CStimulationSetImpl::setStimulationCount(const size_t n)
{
	m_stimulations.resize(size_t(n));
	return true;
}

bool CStimulationSetImpl::setStimulationIdentifier(const size_t index, const uint64_t id)
{
	m_stimulations[size_t(index)].m_Id = id;
	return true;
}

bool CStimulationSetImpl::setStimulationDate(const size_t index, const uint64_t date)
{
	m_stimulations[size_t(index)].m_Date = date;
	return true;
}

bool CStimulationSetImpl::setStimulationDuration(const size_t index, const uint64_t duration)
{
	m_stimulations[size_t(index)].m_Duration = duration;
	return true;
}

size_t CStimulationSetImpl::appendStimulation(const uint64_t id, const uint64_t date, const uint64_t duration)
{
	m_stimulations.push_back(CStimulation(id, date, duration));
	return m_stimulations.size() - 1;
}

size_t CStimulationSetImpl::insertStimulation(const size_t index, const uint64_t id, const uint64_t date, const uint64_t duration)
{
	if (index > m_stimulations.size()) { return false; }
	if (index == m_stimulations.size()) { m_stimulations.push_back(CStimulation(id, date, duration)); }
	else { m_stimulations.insert(m_stimulations.begin() + size_t(index), CStimulation(id, date, duration)); }
	return true;
}

bool CStimulationSetImpl::removeStimulation(const size_t index)
{
	if (index >= m_stimulations.size()) { return false; }
	m_stimulations.erase(m_stimulations.begin() + size_t(index));
	return true;
}

// ________________________________________________________________________________________________________________
//

CStimulationSet::CStimulationSet() { m_stimSet = new CStimulationSetImpl(); }

}  // namespace OpenViBE

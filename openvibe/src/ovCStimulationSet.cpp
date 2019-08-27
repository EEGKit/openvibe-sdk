#include "ovCStimulationSet.h"

#include <vector>

using namespace OpenViBE;
using namespace std;

namespace OpenViBE
{
	namespace
	{
		class CStimulation
		{
		public:

			CStimulation() { }

			CStimulation(const uint64_t ui64Identifier, const uint64_t ui64Date, const uint64_t ui64Duration)
				: m_Id(ui64Identifier), m_date(ui64Date), m_duration(ui64Duration) { }

			uint64_t m_Id = 0;
			uint64_t m_date = 0;
			uint64_t m_duration = 0;
		};

		class CStimulationSetImpl : public IStimulationSet
		{
		public:
			void clear() override { m_stimulations.clear(); }
			uint64_t getStimulationCount() const override { return m_stimulations.size(); }
			uint64_t getStimulationIdentifier(const uint64_t index) const override { return m_stimulations[size_t(index)].m_Id; }
			uint64_t getStimulationDate(const uint64_t index) const override { return m_stimulations[size_t(index)].m_date; }
			uint64_t getStimulationDuration(const uint64_t index) const override { return m_stimulations[size_t(index)].m_duration; }
			bool setStimulationCount(const uint64_t n) override;
			bool setStimulationIdentifier(const uint64_t index, const uint64_t identifier) override;
			bool setStimulationDate(const uint64_t index, const uint64_t date) override;
			bool setStimulationDuration(const uint64_t index, const uint64_t duration) override;
			uint64_t appendStimulation(const uint64_t identifier, const uint64_t date, const uint64_t duration) override;
			uint64_t insertStimulation(const uint64_t index, const uint64_t identifier, const uint64_t date, const uint64_t duration) override;
			bool removeStimulation(const uint64_t index) override;

			_IsDerivedFromClass_Final_(IStimulationSet, OV_ClassId_StimulationSetImpl)

		private:

			vector<CStimulation> m_stimulations;
		};
	} // namespace
} // namespace OpenViBE

// ________________________________________________________________________________________________________________
//

bool CStimulationSetImpl::setStimulationCount(const uint64_t n)
{
	m_stimulations.resize(size_t(n));
	return true;
}

bool CStimulationSetImpl::setStimulationIdentifier(const uint64_t index, const uint64_t identifier)
{
	m_stimulations[size_t(index)].m_Id = identifier;
	return true;
}

bool CStimulationSetImpl::setStimulationDate(const uint64_t index, const uint64_t date)
{
	m_stimulations[size_t(index)].m_date = date;
	return true;
}

bool CStimulationSetImpl::setStimulationDuration(const uint64_t index, const uint64_t duration)
{
	m_stimulations[size_t(index)].m_duration = duration;
	return true;
}

uint64_t CStimulationSetImpl::appendStimulation(const uint64_t identifier, const uint64_t date, const uint64_t duration)
{
	m_stimulations.push_back(CStimulation(identifier, date, duration));
	return m_stimulations.size() - 1;
}

uint64_t CStimulationSetImpl::insertStimulation(const uint64_t index, const uint64_t identifier, const uint64_t date, const uint64_t duration)
{
	if (index > m_stimulations.size()) { return false; }
	if (index == m_stimulations.size()) { m_stimulations.push_back(CStimulation(identifier, date, duration)); }
	else { m_stimulations.insert(m_stimulations.begin() + size_t(index), CStimulation(identifier, date, duration)); }
	return true;
}

bool CStimulationSetImpl::removeStimulation(const uint64_t index)
{
	if (index >= m_stimulations.size()) { return false; }
	m_stimulations.erase(m_stimulations.begin() + size_t(index));
	return true;
}

// ________________________________________________________________________________________________________________
//

CStimulationSet::CStimulationSet() { m_stimulationSet = new CStimulationSetImpl(); }

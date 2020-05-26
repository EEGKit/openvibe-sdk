#include "CStimulationSet.hpp"

namespace OpenViBE {
//--------------------------------------------------------------------------------
void CStimulationSet::copy(CStimulationSet& dst, const CStimulationSet& src, const CTime& shift)
{
	dst.clear();
	append(dst, src, shift);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CStimulationSet::append(CStimulationSet& dst, const CStimulationSet& src, const CTime& shift)
{
	for (const auto& s : src) { dst.append(CStimulation(s.m_ID, s.m_Date + shift, s.m_Duration)); }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CStimulationSet::appendRange(CStimulationSet& dst, const CStimulationSet& src, const CTime& startTime, const CTime& endTime, const CTime& shift)
{
	for (const auto& s : src)
	{
		const CTime date = s.m_Date;
		if (startTime <= date && date < endTime) { dst.append(CStimulation(s.m_ID, s.m_Date + shift, s.m_Duration)); }
	}
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CStimulationSet::removeRange(CStimulationSet& set, const CTime& startTime, const CTime& endTime)
{
	for (size_t i = 0; i < set.size(); ++i)
	{
		const CTime date = set[i].m_Date;
		if (startTime <= date && date < endTime) { set.remove(i--); }
	}
}
//--------------------------------------------------------------------------------
}  // namespace OpenViBE

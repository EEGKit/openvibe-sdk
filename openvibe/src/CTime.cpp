#include "CTime.hpp"

#include <sstream>
#include <iomanip>


namespace OpenViBE {

//--------------------------------------------------------------------------------
CTime& CTime::operator=(const CTime& time)
{
	m_time = time.m_time;
	return *this;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
CTime& CTime::operator+=(const CTime& time)
{
	m_time += time.m_time;
	return *this;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
CTime& CTime::operator-=(const CTime& time)
{
	m_time -= time.m_time;
	return *this;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CTime::getTime(size_t& hours, size_t& minutes, size_t& seconds, size_t& milliseconds) const
{
	const double time = toSeconds();
	milliseconds      = size_t(time * 1000) % 1000;
	seconds           = (size_t(time)) % 60;
	minutes           = (size_t(time) / 60) % 60;
	hours             = (size_t(time) / 3600);
}
//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
std::string CTime::getTimeStr() const
{
	size_t h, m, s, ms;
	getTime(h, m, s, ms);
	std::stringstream ss;
	ss.fill('0');
	if (h != 0) { ss << std::setw(2) << h << "h " << std::setw(2) << m << "m " << std::setw(2) << s << "s " << std::setw(3) << ms << "ms"; }
	else if (m != 0) { ss << std::setw(2) << m << "m " << std::setw(2) << s << "s " << std::setw(3) << ms << "ms"; }
	else if (s != 0) { ss << std::setw(2) << s << "s " << std::setw(3) << ms << "ms"; }
	else { ss << std::setw(3) << ms << "ms"; }
	return ss.str();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
std::string CTime::str(const bool inSecond, const bool inHexa) const
{
	std::stringstream res;
	// Precision for 3 after dot and fixed to force 0 display to have always 3 after precision
	if (inSecond) { res << std::setprecision(3) << std::fixed << toSeconds() << " sec"; }
	else { res << m_time; }

	// Force to have 16 number and 0 for empty number (space character by default)
	if (inHexa) { res << " (0x" << std::setfill('0') << std::setw(16) << std::hex << m_time << ")"; }
	return res.str();
}
//--------------------------------------------------------------------------------

}  // namespace OpenViBE

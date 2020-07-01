///-------------------------------------------------------------------------------------------------
/// 
/// \file CTime.hpp
/// \brief Time Class for OpenViBE.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 04/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "ov_common_defines.h"
#include <cstdint>
#include <limits>
#include <string>

namespace OpenViBE {
/// <summary>  This class is the basic class to use, compute and display time in the OpenViBE platform.\n
/// The time in the OpenViBE platform is based on 64 bits integers. </summary>
class OV_API CTime
{
public:

	//--------------------------------------------------
	//------------ Constructor / Destructor ------------
	//--------------------------------------------------

	/// <summary> Default constructor. </summary>
	CTime() = default;

	/// <summary> 64 bits integer based constructor. </summary>
	/// <param name="time"> The original time in <c>uint64_t</c>.</param>
	/// <remarks> the template is used to avoid the ambiguity with the double constructor and can be used with all integer type.
	/// We don't use explicit qualifier to allow expression as <c>CTime t = 0;</c> </remarks>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	CTime(const T time) : m_time(time) {}

	/// <summary> Constructor with time in seconds. </summary>
	/// <param name="seconds"> The time in seconds. </param>
	explicit CTime(const double seconds) : m_time(seconds >= 0.0 ? uint64_t(seconds * double(1LL << 32)) : 0) {}

	/// <summary> Copy constructor. </summary>
	/// <param name="time">The original time.</param>
	CTime(const CTime& time) : m_time(time.m_time) {}

	/// <summary> Constructor sampling and sample count to time. </summary>
	/// <param name="sampling"> The sampling frequency.</param>
	/// <param name="sampleCount"> The sample count.</param>
	/// <remarks> It assumes, if you indicate a sampling with 0 return max. </remarks>
	CTime(const uint64_t sampling, const uint64_t sampleCount)
		: m_time(sampling == 0 ? std::numeric_limits<uint64_t>::max() : (sampleCount << 32) / sampling) { }

	/// <summary> Default Destructor. </summary>
	~CTime() = default;

	/// <summary> Maximum value for the time. </summary>
	static CTime max() { return CTime(std::numeric_limits<uint64_t>::max()); }

	/// <summary> Minimum value for the time. </summary>
	static CTime min() { return CTime(std::numeric_limits<uint64_t>::min()); }

	//--------------------------------------------------
	//------------------ Conversions -------------------
	//--------------------------------------------------

	/// <summary> Given a fixed point time and the sampling rate, returns the number of samples obtained. </summary>
	/// <param name="sampling"> The sampling rate of the signal. </param>
	/// <returns> Sample count corresponding to the time with this sampling. </returns>
	uint64_t toSampleCount(const uint64_t sampling) const { return (sampling == 0 ? std::numeric_limits<uint64_t>::max() : (m_time + 1) * sampling - 1) >> 32; }

	/// <summary> Get the time in seconds. </summary>
	/// <returns> Regular floating point time in seconds. </returns>
	double toSeconds() const { return m_time / double(1LL << 32); }

	/// <summary> Get the number of hours, minutes, seconds, milliseconds of the time. </summary>
	/// <param name="hours"> The hours. </param>
	/// <param name="minutes"> The minutes. </param>
	/// <param name="seconds"> The seconds. </param>
	/// <param name="milliseconds"> The milliseconds. </param>
	void getTime(size_t& hours, size_t& minutes, size_t& seconds, size_t& milliseconds) const;

	/// <summary> Get the string of the time with the format "XXh XXm XXs XXXms". </summary>
	std::string getTimeStr() const;

	
	/// <summary> Ceil the time. \n
	/// - <c>1LL << 32</c> corresponds to <c>1</c> followed by 32 <c>0</c> so the maximum of an int of 32bit + <c>1</c> (\f$ 2^{32} \f$).
	/// - <c>0xFFFFFFFFLL << 32</c> corresponds to 32 <c>1</c> followed by 32 <c>0</c> (we see 8 character because 4 bit is used by Hexadecimal characters).
	/// - <c>(m_time & (0xFFFFFFFFLL << 32))</c> corresponds to all bits after the position 31 for the member <see cref="m_times"/> followed by 32 <c>0</c>.
	/// 
	/// So this formula, place all bits in position <c>0</c> to <c>31</c> to <c>0</c>,
	/// next add 1 at position 32 (so add \f$ 2^{32} \f$), of course this last operation can modify other bits.</summary>
	/// <returns> The time ceiled. </returns>
	CTime ceil() const { return CTime(uint64_t(((m_time << 32) > 0) ? ((m_time & (0xFFFFFFFFLL << 32)) + (1LL << 32)) : m_time)); }

	/// <summary> Display the time in second and/or in hexa. </summary>
	/// <returns> the time. </returns>
	std::string str(const bool inSecond = true, const bool inHexa = false) const;

	/// <summary> Get the time. </summary>
	/// <returns> the time. </returns>
	uint64_t time() const { return m_time; }

	//--------------------------------------------------
	//------------------- Operators --------------------
	//--------------------------------------------------

	//---------- With Time ----------
	/// <summary> Copy Assignment Operator. </summary>
	/// <param name="time">The time.</param>
	/// <returns> himself. </returns>
	CTime& operator=(const CTime& time);

	/// <summary> Add Assignment Operator. </summary>
	/// <param name="time"> The time to add. </param>
	/// <returns> himself. </returns>
	CTime& operator+=(const CTime& time);

	/// <summary> Substract Assignment Operator. </summary>
	/// <param name="time"> The time to remove. </param>
	/// <returns> himself. </returns>
	CTime& operator-=(const CTime& time);

	/// <summary> "Equal" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if equals, <c>false</c> otherwise. </returns>
	bool operator==(const CTime& time) const { return m_time == time.m_time; }

	/// <summary> "Difference" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if different, <c>false</c> otherwise. </returns>
	bool operator!=(const CTime& time) const { return m_time != time.m_time; }

	/// <summary> "Less than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if less than the test, <c>false</c> otherwise. </returns>
	bool operator<(const CTime& time) const { return m_time < time.m_time; }

	/// <summary> "Greater than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if greater than the test, <c>false</c> otherwise. </returns>
	bool operator>(const CTime& time) const { return m_time > time.m_time; }

	/// <summary> "Less or equal than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if less or equal than, <c>false</c> otherwise. </returns>
	bool operator<=(const CTime& time) const { return m_time <= time.m_time; }

	/// <summary> "Greater or equal than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if greater or equal than the test, <c>false</c> otherwise. </returns>
	bool operator>=(const CTime& time) const { return m_time >= time.m_time; }

	//---------- With Template ----------
	/// <summary> Copy Assignment Operator. </summary>
	/// <param name="time">The time.</param>
	/// <returns> himself. </returns>
	/// <remarks> Template function must be define in header to keep the template system in extern program. </remarks>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	CTime& operator=(const T time)
	{
		m_time = time;
		return *this;
	}

	/// <summary> Add Assignment Operator. </summary>
	/// <param name="time"> The time to add. </param>
	/// <returns> himself. </returns>
	/// <remarks> Template function must be define in header to keep the template system in extern program. </remarks>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	CTime& operator+=(const T time)
	{
		m_time += time;
		return *this;
	}

	/// <summary> Substract Assignment Operator. </summary>
	/// <param name="time"> The time to remove. </param>
	/// <returns> himself. </returns>
	/// <remarks> Template function must be define in header to keep the template system in extern program. </remarks>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	CTime& operator-=(const T time)
	{
		m_time -= time;
		return *this;
	}

	/// <summary> "Equal" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if equals, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator==(const T time) const { return m_time == time; }

	/// <summary> "Difference" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if different, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator!=(const T time) const { return m_time != time; }

	/// <summary> "Less than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if less than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator<(const T time) const { return m_time < time; }

	/// <summary> "Greater than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if greater than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator>(const T time) const { return m_time > time; }

	/// <summary> "Less or equal than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if less or equal than, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator<=(const T time) const { return m_time <= time; }

	/// <summary> "Greater or equal than" test operator. </summary>
	/// <param name="time"> The time to compare. </param>
	/// <returns> <c>true</c> if greater or equal than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator>=(const T time) const { return m_time >= time; }

	/// <summary> Implements the operator for integral type. </summary>
	/// <returns> The result of the operator. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	operator T() const { return m_time; }

	/// <summary> Implements the operator for const integral type. </summary>
	/// <returns> The result of the operator. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	operator const T() const { return m_time; }

	//--------------------------------------------------
	//--- friends (must be in header files for link) ---
	//--------------------------------------------------

	/// <summary> Override the ostream operator. </summary>
	/// <param name="os"> The ostream. </param>
	/// <param name="obj"> The object. </param>
	/// <returns> Return the modified ostream. </returns>
	friend std::ostream& operator<<(std::ostream& os, const CTime& obj)
	{
		os << obj.str();
		return os;
	}

	/// <summary> Addition Operator. </summary>
	/// <param name="left">	The fisrt time. </param>
	/// <param name="right"> The second time. </param>
	/// <returns> The result of the operation. </returns>
	friend CTime operator+(CTime left, const CTime& right)
	{
		left += right;
		return left;
	}

	/// <summary> Substract Operator. </summary>
	/// <param name="left"> The fisrt time. </param>
	/// <param name="right"> The second time. </param>
	/// <returns> The result of the operation. </returns>
	friend CTime operator-(CTime left, const CTime& right)
	{
		left -= right;
		return left;
	}

protected:
	uint64_t m_time = 0; ///< the 64 bit time value
};
}  // namespace OpenViBE

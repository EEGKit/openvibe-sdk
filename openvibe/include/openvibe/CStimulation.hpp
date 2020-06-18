///-------------------------------------------------------------------------------------------------
/// 
/// \file CStimulation.hpp
/// \brief Stimulation Class for OpenViBE.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 25/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include <iostream>
#include "ov_common_defines.h"
#include "CTime.hpp"

namespace OpenViBE {
/// <summary>  This class is used for stimulations which have an id a date and a duration. </summary>
class OV_API CStimulation
{
public:
	//----- Constructor -----
	CStimulation() = default;	///< Default constructor.
	CStimulation(const size_t id, const CTime& date, const CTime& duration) : m_ID(id), m_Date(date), m_Duration(duration) {}	///< Specific constructor.
	CStimulation(const CStimulation& other) : m_ID(other.m_ID), m_Date(other.m_Date), m_Duration(other.m_Duration) {}			///< Copy constructor.
	~CStimulation() = default;	///< Default Destructor.

	/// <summary> Affectation Operator. </summary>
	/// <param name="stim"> The stimulation. </param>
	/// <returns> himself. </returns>
	CStimulation& operator=(const CStimulation& stim)
	{
		if (this == &stim) return *this;
		m_ID       = stim.m_ID;
		m_Date     = stim.m_Date;
		m_Duration = stim.m_Duration;
		return *this;
	}

	/// <summary> "Equal" test operator. </summary>
	/// <param name="stim"> The stimulation to compare. </param>
	/// <returns> <c>true</c> if equals, <c>false</c> otherwise. </returns>
	bool operator==(const CStimulation& stim) const { return m_ID == stim.m_ID && m_Date == stim.m_Date && m_Duration == stim.m_Duration; }

	/// <summary> "Difference" test operator. </summary>
	/// <param name="stim"> The stimulation to compare. </param>
	/// <returns> <c>true</c> if different, <c>false</c> otherwise. </returns>
	bool operator!=(const CStimulation& stim) const { return m_ID != stim.m_ID || m_Date != stim.m_Date || m_Duration != stim.m_Duration; }
	
	/// <summary> Override the ostream operator. </summary>
	/// <param name="os"> The ostream. </param>
	/// <param name="obj"> The object. </param>
	/// <returns> Return the modified ostream. </returns>
	friend std::ostream& operator<<(std::ostream& os, const CStimulation& obj)
	{
		os << obj.m_ID << ", at " << obj.m_Date << " during " << obj.m_Duration;
		return os;
	}
	
	//----- Members (useless to protect members) -----
	size_t m_ID = std::numeric_limits<size_t>::max();	///< The Id of the Stimulation (Identifier OpenViBE are useless).
	CTime m_Date;										///< Date of the Stimulation.
	CTime m_Duration;									///< Duration of the Stimulation.
};
}  // namespace OpenViBE

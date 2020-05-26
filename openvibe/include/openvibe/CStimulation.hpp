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
	~CStimulation() = default;	///< Default Destructor.

	//----- Members (useless to protect members) -----
	size_t m_ID = std::numeric_limits<size_t>::max();	///< The Id of the Stimulation (Identifier OpenViBE are useless).
	CTime m_Date;										///< Date of the Stimulation.
	CTime m_Duration;									///< Duration of the Stimulation.
};
}  // namespace OpenViBE

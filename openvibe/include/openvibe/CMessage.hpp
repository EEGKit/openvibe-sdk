///-------------------------------------------------------------------------------------------------
/// 
/// \file CMessage.hpp
/// \brief Message Class for OpenViBE.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 25/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "ov_common_defines.h"
#include "CIdentifier.hpp"
#include "CTime.hpp"

namespace OpenViBE {
/// <summary>  This class is used to send an id and a time for event, clock or signal. </summary>
class OV_API CMessage
{
public:
	//----- Constructor -----
	CMessage() = default;	///< Default constructor.
	CMessage(const CIdentifier& id, const CTime& time) : m_ID(id), m_Time(time) {}	///< Specific Constructor.
	~CMessage() = default;	///< Default Destructor.

	//----- Members (useless to protect members) -----
	CIdentifier m_ID = OV_UndefinedIdentifier;	///< The Id of the message
	CTime m_Time;								///< The time of the message
};
}  // namespace OpenViBE

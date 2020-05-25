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
#include "ovCIdentifier.h"
#include "CTime.hpp"

namespace OpenViBE {
/// <summary>  This class is used to send an id and a time for event, clock or signal. </summary>
class OV_API CMessage
{
public:
	CMessage() = default;	///< Default constructor.
	CMessage(const CIdentifier& id, const CTime& time) : m_id(id), m_time(time) {}	///< Default constructor.
	~CMessage() = default;	///< Default Destructor.

	CIdentifier getId() const { return m_id; }	///< Identifier Getter.
	CTime getTime() const { return m_time; }	///< Time Getter.

	void setIdentifier(const CIdentifier& id) { m_id = id; }	///< Identifier Setter.
	void setTime(const CTime& time) { m_time = time; }			///< Time Setter.

protected:
	CIdentifier m_id = OV_UndefinedIdentifier;	///< The Id of the message
	CTime m_time;								///< The time of the message
};
} // namespace OpenViBE

///-------------------------------------------------------------------------------------------------
/// 
/// \file utils.hpp
/// \brief Some constants and functions for google tests
/// \author Thibaut Monseigne (Inria).
/// \version 0.1.
/// \date 26/10/2018.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------

#pragma once

#include <cmath>
#include <sstream>
#include "openvibe/CTime.hpp"

const std::string SEP = "\n====================\n";

//*****************************************************************
//********** Error Message Standardization for googltest **********
//*****************************************************************
/// <summary>	Error message for numeric value. </summary>
/// <param name="name">	The name of the test. </param>
/// <param name="ref"> 	The reference value. </param>
/// <param name="calc">	The calculate value. </param>
/// <returns>	Error message. </returns>
/// <typeparam name="T">	Generic numeric type parameter. </typeparam>
template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
std::string ErrorMsg(const std::string& name, const T ref, const T calc)
{
	std::stringstream ss;
	ss << SEP << name << " : Reference : " << ref << ", \tCompute : " << calc << SEP;
	return ss.str();
}

/// <summary>	Error message for CTime value. </summary>
/// <inheritdoc cref="ErrorMsg(const std::string&, const T, const T)"/>
inline std::string ErrorMsg(const std::string& name, const OpenViBE::CTime& ref, const OpenViBE::CTime& calc)
{
	std::stringstream ss;
	ss << SEP << name << " : Reference : " << ref.str(true, true) << ", \tCompute : " << calc.str(true, true) << SEP;
	return ss.str();
}

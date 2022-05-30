///-------------------------------------------------------------------------------------------------
/// 
/// \brief Some constants and functions for google tests
/// \author Thibaut Monseigne (Inria).
/// \version 0.1.
/// \date 26/10/2018.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "openvibe/CIdentifier.hpp"
#include "openvibe/CTime.hpp"
#include <openvibe/CMatrix.hpp>

#include <cmath>
#include <sstream>

#define SEP "\n====================\n"	//const std::string cause Clang warning (with new compiler constexpr change defines)

//---------------------------------------------------------------------------------------------------
/// <summary> Check if double are almost equals. </summary>
/// <param name="a"> The first number. </param>
/// <param name="b"> The second number. </param>
/// <param name="epsilon"> The tolerance. </param>
/// <returns> <c>true</c> if almmost equals, <c>false</c> otherwise. </returns>
inline bool AlmostEqual(const double a, const double b, const double epsilon = OV_EPSILON) { return std::fabs(a - b) < std::fabs(epsilon); }

//*****************************************************************
//********** Error Message Standardization for googltest **********
//*****************************************************************
//---------------------------------------------------------------------------------------------------
/// <summary>	Error message for numeric value. </summary>
/// <param name="name">	The name of the test. </param>
/// <param name="ref"> 	The reference value. </param>
/// <param name="calc">	The calculate value. </param>
/// <returns>	Error message. </returns>
/// <typeparam name="T">	Generic numeric type parameter. </typeparam>
template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value, T>>
std::string ErrorMsg(const std::string& name, const T ref, const T calc)
{
	std::stringstream ss;
	ss << SEP << name << " : Reference : " << ref << ", \tCompute : " << calc << SEP;
	return ss.str();
}

//---------------------------------------------------------------------------------------------------
/// <summary>	Error message for string value. </summary>
/// <inheritdoc cref="ErrorMsg(const std::string&, const T, const T)"/>
inline std::string ErrorMsg(const std::string& name, const std::string& ref, const std::string& calc)
{
	std::stringstream ss;
	ss << SEP << name << " : Reference : " << ref << ", \tCompute : " << calc << SEP;
	return ss.str();
}

//---------------------------------------------------------------------------------------------------
/// <summary>	Error message for CTime value. </summary>
/// <inheritdoc cref="ErrorMsg(const std::string&, const T, const T)"/>
inline std::string ErrorMsg(const std::string& name, const OpenViBE::CTime& ref, const OpenViBE::CTime& calc)
{
	std::stringstream ss;
	ss << SEP << name << " : Reference : " << ref.str(true, true) << ", \tCompute : " << calc.str(true, true) << SEP;
	return ss.str();
}

//---------------------------------------------------------------------------------------------------
/// <summary>	Error message for CIdentifier value. </summary>
/// <inheritdoc cref="ErrorMsg(const std::string&, const T, const T)"/>
inline std::string ErrorMsg(const std::string& name, const OpenViBE::CIdentifier& ref, const OpenViBE::CIdentifier& calc)
{
	std::stringstream ss;
	ss << SEP << name << " : Reference : " << ref << ", \tCompute : " << calc << SEP;
	return ss.str();
}

//---------------------------------------------------------------------------------------------------
/// <summary>	Error message for CIdentifier value. </summary>
/// <inheritdoc cref="ErrorMsg(const std::string&, const T, const T)"/>
inline std::string ErrorMsg(const std::string& name, const OpenViBE::CMatrix& ref, const OpenViBE::CMatrix& calc)
{
	std::stringstream ss;
	ss << SEP << name << " : " << std::endl << "********** Reference **********\n" << ref << std::endl << "********** Compute **********\n" << calc << SEP;
	return ss.str();
}

///-------------------------------------------------------------------------------------------------
/// 
/// \file CIdentifier.hpp
/// \brief Globally used identification class.
/// \author  Yann Renard (INRIA/IRISA) & Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 16/06/2006.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "defines.hpp"
#include <limits>
#include <string>

namespace OpenViBE {

/// <summary> Globally used identification class.\n
///
/// This class is the basic class to use in order to identify objects in the OpenViBE platform.\n
/// It can be used for class identification, for object identification and any user needed identification process.\n
///
/// The identification of the OpenViBE platform is based on 64 bits integers.\n
///
/// This class is heavily used in the OpenViBE::IObject class.\n
/// Also, the OpenViBE specification gives serveral already defined class identifiers the developer should know of.\n
/// For this, let you have a look to the documentation of defines.hpp !
/// 
/// </summary>
/// <seealso cref="defines.hpp"/>
class OV_API CIdentifier
{
public:

	/** \name Constructors */
	//@{

	/// <summary> Default constructor.\n Builds up the 64 bits identifier initialized to <c>undefined</c>. </summary>
	CIdentifier() : m_id(undefined().id()) {}

	/// <summary> 32 bits integer based constructor.\n Builds up the 64 bits identifier given its two 32 bits components. </summary>
	/// <param name="id1">The first part of the identifier.</param>
	/// <param name="id2">The second part of the identifier.</param>
	CIdentifier(const size_t id1, const size_t id2) : m_id((uint64_t(id1) << 32) + id2) {}

	/// <summary> 64 bits integer based constructor. </summary>
	/// <param name="id"> The identifier. </param>
	CIdentifier(const uint64_t id) : m_id(id) {}

	/// <summary> string based constructor. </summary>
	/// <param name="str"> The string with identifier. </param>
	CIdentifier(const std::string& str) { if (!fromString(str)) { m_id = undefined().id(); } }

	/// <summary> Copy constructor.\n Builds up the 64 bits identifier exacly the same as given identifier parameter. </summary>
	/// <param name="id"> the identifier to initialize this identifier from. </param>
	CIdentifier(const CIdentifier& id) : m_id(id.m_id) {}

	/// <summary> Undefined Identifier (the same as default constructor. </summary>
	/// <returns> Identifier define as undefined. </returns>
	static CIdentifier undefined() { return CIdentifier(std::numeric_limits<uint64_t>::max()); }

	//@}
	/** \name Operators */
	//@{

	/// <summary> Copy Assignment Operator. </summary>
	/// <param name="id">The identifier.</param>
	/// <returns> himself. </returns>
	CIdentifier& operator=(const CIdentifier& id);

	/// <summary> Increments this identifier by 1. </summary>
	/// <returns> himself. </returns>
	/// <remarks> If this identifier is \c CIdentifier::undefined(), it is not incremented.\n
	/// If this idenfitier is not \c CIdentifier::undefined(), it can not becomre \c CIdentifier::undefined() after being incremented. </remarks>
	CIdentifier& operator++();

	/// <summary> Decrements this identifier by 1. </summary>
	/// <returns> himself. </returns>
	/// <remarks> If this identifier is \c CIdentifier::undefined(), it is not decremented.\n
	/// If this idenfitier is not \c CIdentifier::undefined(), it can not become \c CIdentifier::undefined() after being decremented. </remarks>
	CIdentifier& operator--();

	/// <summary> "Equal" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if equals, <c>false</c> otherwise. </returns>
	bool operator==(const CIdentifier& id) const { return m_id == id.id(); }

	/// <summary> "Difference" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if different, <c>false</c> otherwise. </returns>
	bool operator!=(const CIdentifier& id) const { return m_id != id.id(); }

	/// <summary> "Less than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if less than the test, <c>false</c> otherwise. </returns>
	bool operator<(const CIdentifier& id) const { return m_id < id.id(); }

	/// <summary> "Greater than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if greater than the test, <c>false</c> otherwise. </returns>
	bool operator>(const CIdentifier& id) const { return m_id > id.id(); }

	/// <summary> "Less or equal than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if  less or equal than the test, <c>false</c> otherwise. </returns>
	bool operator<=(const CIdentifier& id) const { return m_id <= id.id(); }

	/// <summary> "Greater or equal than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if greater or equal than the test, <c>false</c> otherwise. </returns>
	bool operator>=(const CIdentifier& id) const { return m_id >= id.id(); }

	//---------- With Template ----------
	/// <summary> Copy Assignment Operator. </summary>
	/// <param name="id"> The identifier. </param>
	/// <returns> Himself. </returns>
	/// <remarks> Template function must be define in header to keep the template system in extern program. </remarks>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	CIdentifier& operator=(const T id)
	{
		m_id = id;
		return *this;
	}

	/// <summary> "Equal" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if equals, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator==(const T id) const { return m_id == id; }

	/// <summary> "Difference" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if different, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator!=(const T id) const { return m_id != id; }

	/// <summary> "Less than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if less than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator<(const T id) const { return m_id < id; }

	/// <summary> "Greater than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if greater than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator>(const T id) const { return m_id > id; }

	/// <summary> "Less or equal than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if  less or equal than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator<=(const T id) const { return m_id <= id; }

	/// <summary> "Greater or equal than" test operator. </summary>
	/// <param name="id"> The identifier to compare. </param>
	/// <returns> <c>true</c> if greater or equal than the test, <c>false</c> otherwise. </returns>
	template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
	bool operator>=(const T id) const { return m_id >= id; }

	//@}

	/// <summary> Converts this identifier into a string. </summary>
	/// <returns> This identifier represented as a <c>std::string</c>. </returns>
	std::string str() const;

	/// <summary> Reads a a string to extract this identifier. </summary>
	/// <param name="str"> the string to convert. </param>
	/// <returns> <c>true</c> in case of success, <c>false</c> otherwise. </returns>
	bool fromString(const std::string& str);
	
	/// <summary> Get the ID. </summary>
	/// <returns> The unsigned integer identifier. </returns>
	/// <remarks> Use this function with care, identifiers should not be considered as integers.
	/// Actually, the internal 64 bits representation may change, resulting in code port needs if you use this function. </remarks>
	uint64_t id() const { return m_id; }

	/// <summary> Creates a random identifier. </summary>
	/// <returns> A random identifier. </returns>
	/// <remarks> The returned identifier can not be \c CIdentifier::undefined(). </remarks>
	static CIdentifier random();

	/// <summary> Override the ostream operator. </summary>
	/// <param name="os"> The ostream. </param>
	/// <param name="obj"> The object. </param>
	/// <returns> Return the modified ostream. </returns>
	friend std::ostream& operator<<(std::ostream& os, const CIdentifier& obj)
	{
		os << obj.str();
		return os;
	}

protected:

	uint64_t m_id = 0;	///< the 64 bit identifier value
};
}  // namespace OpenViBE

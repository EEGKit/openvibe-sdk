///-------------------------------------------------------------------------------------------------
/// 
/// \file CIdentifier.hpp
/// \brief Globally used identification class.
/// \author  Yann Renard (INRIA/IRISA).
/// \version 1.0.
/// \date 16/06/2006.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "defines.hpp"
#include "ovCString.h"
#include <limits>
#include <string>

#define OV_UndefinedIdentifier	OpenViBE::CIdentifier(0xffffffff, 0xffffffff)

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

	/// <summary> Default constructor.\n Builds up the 64 bits identifier initialized to \c undefined.</summary>
	CIdentifier() : m_id(std::numeric_limits<uint64_t>::max()) {}

	/// <summary> 32 bits integer based constructor.\n Builds up the 64 bits identifier given its two 32 bits components. </summary>
	/// <param name="id1">The first part of the identifier.</param>
	/// <param name="id2">The second part of the identifier.</param>
	CIdentifier(const size_t id1, const size_t id2) : m_id((uint64_t(id1) << 32) + id2) {}

	/// <summary> 64 bits integer based constructor. </summary>
	/// <param name="id"> The identifier. </param>
	CIdentifier(const uint64_t id) : m_id(id) {}
	
	/// <summary> string based constructor. </summary>
	/// <param name="str"> The string with identifier. </param>
	CIdentifier(const std::string& str) { fromString(str); }

	/// <summary> Copy constructor.\n Builds up the 64 bits identifier exacly the same as given identifier parameter. </summary>
	/// <param name="id"> the identifier to initialize this identifier from. </param>
	CIdentifier(const CIdentifier& id) : m_id(id.m_id) {}

	/// <summary> Undefined Identifier (the same as default constructor. </summary>
	/// <returns> Identifier define as undefined. </returns>
	static CIdentifier undefined() { return CIdentifier(std::numeric_limits<uint64_t>::max()); }

	//@}
	/** \name Operators */
	//@{

	/**
	 * \brief Affectation operator
	 * \param id [in] : the identifier to initialize
	 *        this identifier from
	 * \return this identifier
	 *
	 * Reinitializes the 64 bits identifier exactly the same as
	 * given identifier parameter.
	 */
	CIdentifier& operator=(const CIdentifier& id);
	/**
	 * \brief Increments this identifier by 1
	 * \return this identifier
	 * \note if this identifier is \c OV_UndefinedIdentifier, it is not incremented
	 * \note if this idenfitier is not \c OV_UndefinedIdentifier, it can not becomre \c OV_UndefinedIdentifier after being incremented
	 */
	CIdentifier& operator++();
	/**
	 * \brief Decrements this identifier by 1
	 * \return this identifier
	 * \note if this identifier is \c OV_UndefinedIdentifier, it is not decremented
	 * \note if this idenfitier is not \c OV_UndefinedIdentifier, it can not becomre \c OV_UndefinedIdentifier after being decremented
	 */
	CIdentifier& operator--();
	/**
	 * \brief Equality test operator
	 * \param id1 [in] : the first identifier to compare
	 * \param id2 [in] : the second identifier to compare
	 * \return \e true if the two identifiers are equal,
	 * \return \e false if the two identifiers are different
	 *
	 * Compares both 32 bits parts of the two identifiers and
	 * checks if those are equal or not.
	 *
	 * \sa operator!=
	 */
	friend OV_API bool operator==(const CIdentifier& id1, const CIdentifier& id2);
	/**
	 * \brief Difference test operator
	 * \param id1 [in] : the first identifier to compare
	 * \param id2 [in] : the second identifier to compare
	 * \return \e true if the two identifiers are different,
	 * \return \e false if the two identifiers are equal
	 *
	 * Compares both 32 bits parts of the two identifiers and
	 * checks if those are equal or not.
	 *
	 * \sa operator==
	 */
	friend OV_API bool operator!=(const CIdentifier& id1, const CIdentifier& id2);
	/**
	 * \brief Order test operator
	 * \param id1 [in] : the first identifier to compare
	 * \param id2 [in] : the second identifier to compare
	 * \return \e true if the first identifier is less than the second one
	 * \return \e false if the first identifier is greater or equal to the second one
	 *
	 * Compares both 32 bits parts of the two identifiers.
	 *
	 * \sa operator>
	 * \sa operator==
	 */
	friend OV_API bool operator<(const CIdentifier& id1, const CIdentifier& id2);
	/**
	 * \brief Order test operator
	 * \param id1 [in] : the first identifier to compare
	 * \param id2 [in] : the second identifier to compare
	 * \return \e true if the first identifier is greater than the second one
	 * \return \e false if the first identifier is less or equal to the second one
	 *
	 * Compares both 32 bits parts of the two identifiers.
	 *
	 * \sa operator<
	 * \sa operator==
	 */
	friend OV_API bool operator>(const CIdentifier& id1, const CIdentifier& id2);
	/**
	 * \brief Order test operator
	 * \param id1 [in] : the first identifier to compare
	 * \param id2 [in] : the second identifier to compare
	 * \return \e true if the first identifier is less or equal than the second one
	 * \return \e false if the first identifier is greater to the second one
	 *
	 * Compares both 32 bits parts of the two identifiers.
	 *
	 * \sa operator>
	 * \sa operator==
	 */
	friend OV_API bool operator<=(const CIdentifier& id1, const CIdentifier& id2) { return !(id1 > id2); }
	/**
	 * \brief Order test operator
	 * \param id1 [in] : the first identifier to compare
	 * \param id2 [in] : the second identifier to compare
	 * \return \e true if the first identifier is greater or equal than the second one
	 * \return \e false if the first identifier is less to the second one
	 *
	 * Compares both 32 bits parts of the two identifiers.
	 *
	 * \sa operator<
	 * \sa operator==
	 */
	friend OV_API bool operator>=(const CIdentifier& id1, const CIdentifier& id2) { return !(id1 < id2); }

	//@}

	/// <summary> Converts this identifier into a string. </summary>
	/// <returns> This identifier represented as a <c>std::string</c>. </returns>
	std::string str() const;
	/**
	 * \brief Reads a an OpenViBE string to extract this identifier
	 * \param str [in] : the string to convert
	 * \return \e true in case of success.
	 * \return \e false in case of error.
	 */
	bool fromString(const std::string& str);
	/**
	 * \brief Converts this identifier into an unsigned 64 bits integer
	 * \return The unsigned integer converted identifier
	 * \warning Use this function with care, identifiers should not be considered
	 *          as integers. Actually, the internal 64 bits representation may
	 *          change, resulting in code port needs if you use this function
	 */
	uint64_t id() const { return m_id; }
	
	/**
	 * \brief Creates a random identifier
	 * \return a random identifier
	 * \note The returned identifier can not be \c OV_UndefinedIdentifier
	 */
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

	uint64_t m_id = 0; ///< the 64 bit identifier value
};
}  // namespace OpenViBE

#pragma once

#include "ov_base.h"
#include "ovCString.h"
#include <climits>

#define OV_UndefinedIdentifier OpenViBE::CIdentifier(0xffffffff, 0xffffffff)

namespace OpenViBE
{
	/**
	 * \class CIdentifier
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-06-16
	 * \brief Globally used identification class
	 * \ingroup Group_Base
	 *
	 * This class is the basic class to use in order to identify objects in the OpenViBE platform.
	 * It can be used for class identification, for object identification and any user needed identification process.
	 *
	 * The identification of the OpenViBE platform is based on 64 bits integers.
	 *
	 * This class is heavily used in the OpenViBE::IObject class.
	 * Also, the OpenViBE specification gives serveral already defined class identifiers the developer should know of.
	 * For this, let you have a look to the documentation of ov_defines.h !
	 *
	 * \sa ov_defines.h
	 */
	class OV_API CIdentifier
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * Builds up the 64 bits identifier initialized to
		 * \c OV_UndefinedIdentifier.
		 */
		CIdentifier() : m_id(ULLONG_MAX) {}
		/**
		 * \brief 32 bits integer based constructor
		 * \param id1 [in] : the first part of the identifier
		 * \param id2 [in] : the second part of the identifier
		 *
		 * Builds up the 64 bits identifier given its two 32 bits
		 * components.
		 */
		CIdentifier(const uint32_t id1, const uint32_t id2) : m_id((uint64_t(id1) << 32) + id2) {}
		/**
		 * \brief 64 bits integer based constructor
		 * \param id [in] : The identifier
		 */
		CIdentifier(const uint64_t id) : m_id(id) {}
		/**
		 * \brief Copy constructor
		 * \param id [in] : the identifier to initialize
		 *        this identifier from
		 *
		 * Builds up the 64 bits identifier exacly the same as
		 * given identifier parameter.
		 */
		CIdentifier(const CIdentifier& id) : m_id(id.m_id) {}

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
		
		/**
		 * \brief Converts this identifier into an OpenViBE string
		 * \return This identifier represented as an OpenViBE string
		 */
		CString toString() const;
		/**
		 * \brief Reads a an OpenViBE string to extract this identifier
		 * \param str [in] : the string to convert
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		bool fromString(const CString& str);
		/**
		 * \brief Converts this identifier into an unsigned 64 bits integer
		 * \return The unsigned integer converted identifier
		 * \warning Use this function with care, identifiers should not be considered
		 *          as integers. Actually, the internal 64 bits representation may
		 *          change, resulting in code port needs if you use this function
		 */
		uint64_t toUInteger() const { return m_id; }
		/**
		 * \brief Creates a random identifier
		 * \return a random identifier
		 * \note The returned identifier can not be \c OV_UndefinedIdentifier
		 */
		static CIdentifier random();

	protected:

		uint64_t m_id = 0; ///< the 64 bit identifier value
	};
} // namespace OpenViBE

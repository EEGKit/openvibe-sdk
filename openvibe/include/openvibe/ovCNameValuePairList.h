#pragma once

#include "ov_defines.h"

namespace OpenViBE {

typedef struct CNameValuePairListImpl CNameValuePairListImpl;

/**
 * \class CNameValuePairList
 * \author Vincent Delannoy (INRIA/IRISA)
 * \date 2008-07
 * \brief This class handles a list of name/value pairs.
 * It handles a (hidden) map associating string keys to string values.
 * \ingroup Group_Base
 *
 * This class avoids the need to include stl's map header file in the C++ interface,
 * thus preventing potential compile/link errors when dynamically loading modules.
 *
 * \note Implementation based on std::map<std::string, std::string>
 */
class OV_API CNameValuePairList final
{
public:

	/** \name Constructor / Destructor */
	//@{

	/**
	 * \brief Default constructor
	 *
	 * Initializes the list.
	 */
	CNameValuePairList();
	/**
	 * \brief Copy constructor
	 * \param pairs [in] : The list to copy
	 *
	 * Copies the contents of \c pairs into the new list.
	 */
	CNameValuePairList(const CNameValuePairList& pairs);
	/**
	 * \brief Destructor
	 *
	 * The destructor releases the std::map implementation !
	 */
	~CNameValuePairList();

	//@}
	/** \name Operators */
	//@{

	/**
	 * \brief Affectation operator (copy)
	 * \param pairs [in] : The list to copy
	 * \return This list.
	 */
	CNameValuePairList& operator=(const CNameValuePairList& pairs);
	//@}

	/**
	 * \brief Insert a name/value pair
	 * \param [in] name Name to add to the list
	 * \param [in] value Value to associate with the name
	 * \return True if pair could be inserted, false otherwise
	 */
	bool setValue(const CString& name, const CString& value) const;

	/**
	 * \brief Insert a name/value pair
	 * \param [in] name Name to add to the list
	 * \param [in] value Value to associate with the name
	 * \return True if pair could be inserted, false otherwise
	 */
	bool setValue(const CString& name, const char* value) const;

	/**
	 * \brief Insert a name/value pair where value is a double
	 * \param [in] name Name to add to the list
	 * \param [in] value Float64 value to associate with the name
	 * \return True if pair could be inserted, false otherwise
	 */
	bool setValue(const CString& name, const double& value) const;

	/**
	 * \brief Insert a name/value pair where value is a boolean
	 * \param [in] name Name to add to the list
	 * \param [in] value Boolean value to associate with the name
	 * \return True if pair could be inserted, false otherwise
	 */
	bool setValue(const CString& name, bool value) const;

	/**
	 * \brief Retrieve a value from the list
	 * \param [in] name Name whose value is to be retrieved
	 * \param [out] value Value to be retrieved from the list
	 * \return True if value could be retrieved, false otherwise
	 */
	bool getValue(const CString& name, CString& value) const;

	/**
	 * \brief Retrieve a double value from the list
	 * \param [in] name Name whose value is to be retrieved
	 * \param [out] value Float64 value to be retrieved
	 * \return True if a double value could be retrieved, false otherwise
	 */
	bool getValue(const CString& name, double& value) const;

	/**
	 * \brief Retrieve a boolean value from the list
	 * In the current implementation a value evaluates to true if its string
	 * equals "1" and to false if it equals "0".
	 * \param name
	 * \param value String to evaluate
	 * \return True if string evaluates to a boolean, false otherwise
	 */
	bool getValue(const CString& name, bool& value) const;

	/**
	 * \brief Retrieve a value from the list
	 * \param [in] index Index whose value is to be retrieved
	 * \param [out] name Name of the value stored in rIndex
	 * \param [out] value Value stored in rIndex
	 * \return True if value could be retrieved, false otherwise
	 */
	bool getValue(const size_t index, CString& name, CString& value) const;


	/**
	 * \brief Retrieve the number of stored elements 
	 * \return the number of stored elements
	 */
	size_t getSize() const;

protected:

	CNameValuePairListImpl* m_impl = nullptr; ///< The list implementation
};
} // namespace OpenViBE

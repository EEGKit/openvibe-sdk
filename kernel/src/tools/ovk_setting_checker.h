#pragma once

#include "lepton/Lepton.h"
#include "../ovk_base.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <functional>

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}

	/**
	* \brief Check the setting value (if the setting is numeric),
	* test if it is a correct arithmetic expression, should be used after
	* retrieving the value with "getSettingValue"
	* \param sValue [in] : The value of setting to check
	* \param oTypeIdentifier [in] : The type of setting to check
	* \return \e true in case of success (numeric value is well formed).
	* \return \e false in case of error. In such case,
	*         \c rValue remains unchanged.
	*/
	bool checkSettingValue(const OpenViBE::CString value, const OpenViBE::CIdentifier typeIdentifier)
	{
		// If the token is a numeric value, it may be an arithmetic operation
		if (typeIdentifier == OV_TypeId_Float || typeIdentifier == OV_TypeId_Integer)
		{
			// parse and expression with no variables or functions
			try
			{
				Lepton::Parser::parse(value.toASCIIString()).evaluate();
			}
			catch (...)
			{
				return false;
			}
		}
		else if (typeIdentifier == OV_TypeId_Boolean)
		{
			std::string val = value.toASCIIString();
			std::transform(val.begin(), val.end(), val.begin(), ::to_lower<std::string::value_type>);

			if (!(val == "true" || val == "on" || val == "1" || val == "false" || val == "off" || val == "0"))
			{
				return false;
			}
		}
		//TODO: else
		return true;
	}

};


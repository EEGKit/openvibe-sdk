///-------------------------------------------------------------------------------------------------
/// 
/// \file assert.hpp
/// \brief Utility macros used to wrap framework behavior in response to : Warning, Error, Fatal error.
///
/// A warning should be triggered to warn the api consumer about an expected and important event that is not a fault.
/// An error should be triggered when a failure occurs. A failure is a faulty expected event that alter the correct behavior of the framework.
/// Most OpenViBE API functions communicates a failure occurrence through their return value (false boolean, null pointer, bad identifier...).  
/// A fatal event should be triggered when failure impacting the sanity of the system occurs and it is impossible to recover from it.
/// 
/// \author Charles Garraud (Inria).
/// \version 1.0.
/// \date 20/07/2016.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include <sstream>

#include "ovCIdentifier.h"

// internal use
#define CONVERT_ERROR_TYPE_TO_STRING(type) #type

namespace OpenViBE {
#define HAS_IMBUED_OSTREAM_WITH_C_IDENTIFIER

inline std::ostream& operator<<(std::ostream& os, const CIdentifier id)
{
	os << id.str();
	return os;
}
}  // namespace OpenViBE

/**
 * \def OV_WARNING_LOG(message, logManager)
 *
 * Log a warning \a message using the provided \a logManager. Should not be used directly (use OV_WARNING* instead)
 */
#define OV_WARNING_LOG(message, logManager) do { logManager << OpenViBE::Kernel::LogLevel_Warning << message << "\n"; } while(0)

/**
 * \def OV_WARNING(message, logManager)
 *
 * Use this macro to trigger a warning. \a message is a warning message and \a logManager a logger used to log this message.
 */
#define OV_WARNING(message, logManager) OV_WARNING_LOG(message, logManager)

/**
 * \def OV_WARNING_UNLESS(expression, message, logManager)
 * \see OV_WARNING(message, logManager)
 *
 * Use this macro to trigger a warning unless the condition expressed by \a expression is true.
 */
#define OV_WARNING_UNLESS(expression, message, logManager) do { if (!(expression)) { OV_WARNING(message, logManager); } } while(0)

/**
 * \def OV_WARNING_K(message)
 * \see OV_WARNING(message, logManager)
 *
 * Shorthand for warning macro launched by objects that have direct access to kernel logger through this->getLogManager().
 *
 * Suffix K stands for Kernel.
 */
#define OV_WARNING_K(message) OV_WARNING(message, this->getLogManager())

/**
 * \def OV_WARNING_UNLESS_K(expression, message)
 * \see OV_WARNING_UNLESS(expression, message, logManager)
 *
 * Shorthand for warning macro launched by objects that have direct access to kernel logger through this->getLogManager().
 *
 * Suffix K stands for Kernel.
 */
#define OV_WARNING_UNLESS_K(expression, message) OV_WARNING_UNLESS(expression, message, this->getLogManager())

#define OV_ERROR_LOG_LOCATION(file, line) ", {Error location} : {" << file << "::" << line << "}"

/**
 * \def OV_ERROR_LOG(description, type, file, line, logManager)
 *
 * Log an error using the provided \a logManager. Should not be used directly (use OV_ERROR* instead)
 */
#define OV_ERROR_LOG(description, type, file, line, logManager) \
do { \
	logManager << OpenViBE::Kernel::LogLevel_Error << "{Error description} : {" << description << "}, {Error type} : {" \
			<< CONVERT_ERROR_TYPE_TO_STRING(type) << " (code " << size_t((type)) << ")}" << OV_ERROR_LOG_LOCATION(file, line) << "\n"; \
} while(0)

/**
 * \def OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Low-level macro used to handle an error. The error is described by its \a description and \a type. The generated block of code is returning a
 * value defined by \a returnValue. Error handling is performed with the given \a errorManager and \a logManager.
 *
 * Use this in very specific cases. Prefer the use of higher level macros instead.
 */
#define OV_ERROR(description, type, returnValue, errorManager, logManager) \
do { \
	errorManager.pushErrorAtLocation(type, static_cast<const std::ostringstream&>(std::ostringstream() << description).str().c_str(), __FILE__, __LINE__ ); \
	OV_ERROR_LOG(description, type, __FILE__, __LINE__ , logManager); \
	return returnValue; \
} while (0)

/**
 * \def OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Low-level macro used to handle an error unless the condition expressed by \a expression is true.
 *
 * Use this in very specific cases. Prefer the use of higher level macros instead.
 */
#define OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager) \
do { if (!(expression)) { OV_ERROR(description, type, returnValue, errorManager, logManager); } } while(0)

/**
 * \def OV_ERROR_K(description, type, returnValue)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for error macro launched by objects that have direct access to kernel logger and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix K stands for Kernel.
 */
#define OV_ERROR_K(description, type, returnValue) OV_ERROR(description, type, returnValue, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_KRF(description, type)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for error macro that returns false and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRF stands for Kernel Return False.
 */
#define OV_ERROR_KRF(description, type) OV_ERROR(description, type, false, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_KRZ(description, type)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for error macro that returns zero and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRZ stands for Kernel Return Zero.
 */
#define OV_ERROR_KRZ(description, type) OV_ERROR(description, type, 0, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_KRO(description, type)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for error macro that returns OV_UndefinedIdentifier and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRU stands for Kernel Return Undefined.
 */
#define OV_ERROR_KRU(description, type) OV_ERROR(description, type, OV_UndefinedIdentifier, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_KRV(description, type)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for error macro that returns nothing and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRV stands for Kernel Return Void.
 */
#define OV_ERROR_KRV(description, type) OV_ERROR(description, type, void(), this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_KRN(description, type)
 * \see OV_ERROR(description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for error macro that returns nullptr and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRN stands for Kernel Return Null.
 */
#define OV_ERROR_KRN(description, type) OV_ERROR(description, type, nullptr, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_UNLESS_K(expression, description, type, returnValue)
 * \see OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for conditional error macro launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix K stands for Kernel.
 */
#define OV_ERROR_UNLESS_K(expression, description, type, returnValue) OV_ERROR_UNLESS(expression, description, type, returnValue, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_UNLESS_KRF(expression, description, type)
 * \see OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for conditional error macro that returns false and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRF stands for Kernel Return False.
 */
#define OV_ERROR_UNLESS_KRF(expression, description, type) OV_ERROR_UNLESS(expression, description, type, false, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_UNLESS_KRZ(expression, description, type)
 * \see OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for conditional error macro that returns zero and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRZ stands for Kernel Return Zero.
 */
#define OV_ERROR_UNLESS_KRZ(expression, description, type) OV_ERROR_UNLESS(expression, description, type, 0, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_UNLESS_KRU(expression, description, type)
 * \see OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for conditional error macro that returns OV_UndefinedIdentifier and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRU stands for Kernel Return Undefined.
 */
#define OV_ERROR_UNLESS_KRU(expression, description, type) OV_ERROR_UNLESS(expression, description, type, OV_UndefinedIdentifier, this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_UNLESS_KRV(expression, description, type)
 * \see OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for conditional error macro that returns void() and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRV stands for Kernel Return Void.
 */
#define OV_ERROR_UNLESS_KRV(expression, description, type) OV_ERROR_UNLESS(expression, description, type, void() , this->getErrorManager(), this->getLogManager())

/**
 * \def OV_ERROR_UNLESS_KRN(expression, description, type)
 * \see OV_ERROR_UNLESS(expression, description, type, returnValue, errorManager, logManager)
 *
 * Shorthand for conditional error macro that returns nullptr and launched by objects that have direct access to kernel logger
 * and error managers through this->getLogManager() and this->getErrorManager().
 *
 * Suffix KRN stands for Kernel Return Null.
 */
#define OV_ERROR_UNLESS_KRN(expression, description, type) OV_ERROR_UNLESS(expression, description, type, nullptr, this->getErrorManager(), this->getLogManager())

#define OV_FATAL_LOG_LOCATION ", {Error location} : {" << __FILE__ << "::" << __LINE__ << "}"

/**
 * \def OV_FATAL(description, type, logManager)
 *
 * Use this macro to handle fatal error described by its \a description and \a type. \a logManager it the logger used to log the fatal message.
 */
#define OV_FATAL(description, type, logManager) \
do { \
	logManager << OpenViBE::Kernel::LogLevel_Fatal << "{Error description} : {" << description << "}, {Error type} : {" \
			   << CONVERT_ERROR_TYPE_TO_STRING(type) << " (code " << size_t((type)) << ")}" << OV_FATAL_LOG_LOCATION << "\n"; \
	std::abort(); \
} while(0)

/**
 * \def OV_FATAL_UNLESS(expression, description, type, logManager)
 * \see OV_FATAL(description, type, logManager)
 *
 * Use this macro to handle fatal errors unless the condition expressed by \a expression is true.
 */
#define OV_FATAL_UNLESS(expression, description, type, logManager) do { if (!(expression)) { OV_FATAL(description, type, logManager); } } while(0)

/**
 * \def OV_FATAL_K(description, type)
 * \see OV_FATAL(description, type, logManager)
 *
 * Shorthand for fatal macro launched by objects that have direct access to kernel logger through this->getLogManager().
 *
 * Suffix K stands for Kernel.
 */
#define OV_FATAL_K(description, type) OV_FATAL(description, type, this->getLogManager())

/**
 * \def OV_FATAL_UNLESS_K(description, type)
 * \see OV_FATAL(expression, description, type)
 *
 * Shorthand for fatal conditional macro launched by objects that have direct access to kernel logger through this->getLogManager().
 *
 * Suffix K stands for Kernel.
 */
#define OV_FATAL_UNLESS_K(expression, description, type) OV_FATAL_UNLESS(expression, description, type, this->getLogManager())

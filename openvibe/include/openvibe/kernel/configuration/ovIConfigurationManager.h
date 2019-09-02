#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IConfigurationKeywordExpandCallback;

		/**
		 * \class IConfigurationManager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2008-11-05
		 * \brief Manager for all kind of configuration
		 * \ingroup Group_Configuration
		 * \ingroup Group_Kernel
		 */
		class OV_API IConfigurationManager : public IKernelObject
		{
		public:

			/**
			 * \brief Clears the content of this configuration manager
			 */
			virtual void clear() = 0;

			/**
			 * \brief Parses a configuration file and adds its content as token configuration
			 * \param rFileNameWildCard [in] : a wildcard of the files to parse & add
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note In case the wildcard matches several filenames, it is up to this
			 *       configuration manager to choose the order it parses the files in.
			 * \sa IConfigurationManager::createConfigurationToken
			 */
			virtual bool addConfigurationFromFile(const CString& rFileNameWildCard) = 0;

			/**
			 * \brief Creates a new configuration token in this configuration manager
			 * \param rConfigurationTokenName [in] : the name of the configuration token
			 * \param rConfigurationTokenValue [in] the value of the configuration token
			 * \return the identifier of the newly created token in case of success
			 * \return \c OV_UndefinedIdentifier in case of error
			 * \sa IConfigurationManager::releaseConfigurationToken
			 * \sa IConfigurationManager::createConfigurationToken
			 */
			virtual CIdentifier createConfigurationToken(const CString& rConfigurationTokenName, const CString& rConfigurationTokenValue) = 0;
			/**
			 * \brief Removes an existing configuration token
			 * \param rConfigurationTokenIdentifier [in] : the identifier of the token to remove
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::createConfigurationToken
			 */
			virtual bool releaseConfigurationToken(const CIdentifier& rConfigurationTokenIdentifier) = 0;
			/**
			 * \brief Iterates on the existing configuration tokens of this configuration manager
			 * \param rPreviousConfigurationTokenIdentifier [in] : the identifier of the token to start the iteration from
			 * \return the identifier of the next configuration token in case of success
			 * \return \c OV_UndefinedIdentifier in case there is no more token to iterate on
			 *
			 * Typicall use of this function is :
			 * \code
			 * CIdentifier l_oTokenIdentifier = OV_UndefinedIdentifier; // defaults to OV_UndefinedIdentifier
			 * while((l_oTokenIdentifier=l_pConfigurationManager->getNextConfigurationTokenIdentifier(l_oTokenIdentifier)) != OV_UndefinedIdentifier)
			 * {
			 *     // do some stuff with current token identified with l_oTokenIdentifier
			 * }
			 * \endcode
			 */
			virtual CIdentifier getNextConfigurationTokenIdentifier(const CIdentifier& rPreviousConfigurationTokenIdentifier) const = 0;

			/**
			 * \brief Gets a token's name from its identifier
			 * \param rConfigurationTokenIdentifier [in] : the token identifier which name should be returned
			 * \return the name of the token in case of success
			 * \return an empty string in case of error
			 * \sa IConfigurationManager::getConfigurationTokenValue
			 */
			virtual CString getConfigurationTokenName(const CIdentifier& rConfigurationTokenIdentifier) const = 0;
			/**
			 * \brief Gets a token's value from its identifier
			 * \param rConfigurationTokenIdentifier [in] : the token identifier which value should be returned
			 * \return the value (unexapanded) of the token in case of success
			 * \return an empty string in case of error
			 * \sa IConfigurationManager::getConfigurationTokenName
			 * \sa IConfigurationManager::expand and others
			 * \note the returned value is not expanded by this configuration manager ; only the
			 *       value that was passed at creation time is returned. If you want to expand
			 *       things, please use IConfigurationManager::expand
			 */
			virtual CString getConfigurationTokenValue(const CIdentifier& rConfigurationTokenIdentifier) const = 0;

			/**
			 * \brief Changes the name of an exisiting token
			 * \param rConfigurationTokenIdentifier [in] : the identifier of the token which name should be changed
			 * \param rConfigurationTokenName [in] : the new name of the configuration token
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note it is not valid to add a token using this function
			 * \sa IConfigurationManager::createConfigurationToken
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::setConfigurationTokenValue
			 */
			virtual bool setConfigurationTokenName(const CIdentifier& rConfigurationTokenIdentifier, const CString& rConfigurationTokenName) = 0;
			/**
			 * \brief Changes the value of an exisiting token
			 * \param rConfigurationTokenIdentifier [in] : the identifier of the token which value should be changed
			 * \param rConfigurationTokenValue [in] : the new value of the configuration token
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note it is not valid to add a token using this function
			 * \sa IConfigurationManager::createConfigurationToken
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::setConfigurationTokenName
			 */
			virtual bool setConfigurationTokenValue(const CIdentifier& rConfigurationTokenIdentifier, const CString& rConfigurationTokenValue) = 0;

			/**
			 * \brief Adds a token or replaces the value of a token.
			 * \param rConfigurationTokenName [in] : the name of the token which value should be changed
			 * \param rConfigurationTokenValue [in] : the new value of the configuration token
			 * \return \e true in case of success
			 * \return \e false in case of error
			 * \note new tokens can be added with this function
			 * \note this call is not recursive
			 * \sa IConfigurationManager::createConfigurationToken
			 * \sa IConfigurationManager::addConfigurationFromFile
			 * \sa IConfigurationManager::setConfigurationTokenName
			 */
			virtual bool addOrReplaceConfigurationToken(const CString& rConfigurationTokenName, const CString& rConfigurationTokenValue) = 0;

			/**
			 * \brief Searches the identifier of a token with a given name
			 * \param rConfigurationTokenName [in] : the name of the token which identifier should be found
			 * \param bRecursive [in] : when set to true, asks this configuration manager to propagate
			 *                          the request to parent configuration manager (if any).
			 * \return the identifier of the token with the actual name in case of success
			 * \return \c OV_UndefinedIdentifier in case of error
			 * \note if \c bRecursive is set to \e true then the returned identifier should
			 *       not be considered as the identifier of an existing token in this configuration manager
			 *       as it may have been returned from a parent configuration manager. Instead, one must consider
			 *       the returned identifier as a kind of boolean value : such token actually exists or such
			 *       token does not exist.
			 */
			virtual CIdentifier lookUpConfigurationTokenIdentifier(const CString& rConfigurationTokenName, bool bRecursive = false) const = 0;
			/**
			 * \brief Searches the value of a token with a given name
			 * \param rConfigurationTokenName [in] : the name of the token which value should be found
			 * \return the value of the token with the actual name in case of success
			 * \note This function differs of \c getConfigurationTokenName in the sense that it
			 *       recursively requests a token value to parent configuration managers until it
			 *       finds one (if any). It also differs from the \c expand function in the sense that
			 *       it takes a token name as input but does not expand its value when it finds it.
			 */
			virtual CString lookUpConfigurationTokenValue(const CString& rConfigurationTokenName) const = 0;


			/**
			  * \brief Adds a new parser for special variables
			  * \param rKeyword [in] : keyword to overload
			  * \param rCallback [in] : handler for the keyword
			  * \return true in case of success
			  * \note This parser provides a function that will handle expanding of
			  *       tokens like $keyword{sometext}. "sometext" will be passed to the
			  *       callback;
			  */
			virtual bool registerKeywordParser(const CString& rKeyword, const IConfigurationKeywordExpandCallback& rCallback) = 0;

			/**
			  * \brief Removes the keyword parser for a given keyword
			  * \param keyword [in] : keyword of the parser to remove
			  * \return true in case of success
			  */
			virtual bool unregisterKeywordParser(const CString& keyword) = 0;

			/**
			  * \brief Removes the keyword parser for a given keyword
			  * \param callback [in] : handler for the keyword
			  * \return true in case of success
			  */
			virtual bool unregisterKeywordParser(const IConfigurationKeywordExpandCallback& callback) = 0;

			/**
			 * \brief Expands a string to an expanded string based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expan
			 * \return the expanded string
			 * \sa IConfigurationManager::expandAsFloat
			 * \sa IConfigurationManager::expandAsInteger
			 * \sa IConfigurationManager::expandAsUInteger
			 * \sa IConfigurationManager::expandAsBoolean
			 * \sa IConfigurationManager::expandAsEnumerationEntryValue
			 *
			 * Typical use of this function is :
			 * \code
			 * l_pConfigurationManager->expand("${TokenName}")
			 * \endcode
			 */
			virtual CString expand(const CString& rExpression) const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Configuration_ConfigurationManager)

			virtual CString expandOnlyKeyword(const CString& rKeyword, const CString& rExpression, bool preserveBackslashes = false) const = 0;
			/**
			 * \brief Expands a string to a floating point value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param f64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                string can not be parsed as a floating point value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual double expandAsFloat(const CString& rExpression, double f64FallbackValue = 0) const = 0;
			/**
			 * \brief Expands a string to an integer value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param i64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                string can not be parsed as an integer value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual int64_t expandAsInteger(const CString& rExpression, int64_t i64FallbackValue = 0) const = 0;
			/**
			 * \brief Expands a string to an unsigned integer value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param ui64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                 string can not be parsed as an unsigned integer value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual uint64_t expandAsUInteger(const CString& rExpression, uint64_t ui64FallbackValue = 0) const = 0;
			/**
			 * \brief Expands a string to a boolean value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param bFallbackValue [in] : a fall back value to return in case the expanded
			 *                              string can not be parsed as a boolean value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 */
			virtual bool expandAsBoolean(const CString& rExpression, bool bFallbackValue = true) const = 0;
			/**
			 * \brief Expands a string to an enumeration entry value based on its use of configuration tokens
			 * \param rExpression [in] : the string that you want to expand
			 * \param rEnumerationTypeIdentifier [in] : the enumeration type to use
			 * \param ui64FallbackValue [in] : a fall back value to return in case the expanded
			 *                                 string can not be parsed as an enumeration entry value
			 * \return the expanded value
			 * \sa IConfigurationManager::expand
			 * \sa ITypeManager
			 */
			virtual uint64_t expandAsEnumerationEntryValue(const CString& rExpression, const CIdentifier& rEnumerationTypeIdentifier,
														   uint64_t ui64FallbackValue = 0) const = 0;
		};
	} // namespace Kernel
} // namespace OpenViBE

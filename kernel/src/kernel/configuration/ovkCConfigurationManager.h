#ifndef __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__
#define __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__

#include "../ovkTKernelObject.h"

#include <map>
#include <string>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class IConfigurationKeywordExpandCallback;

		typedef struct
		{
			CString m_sConfigurationName;
			CString m_sConfigurationValue;
		} SConfigurationToken;

		class CConfigurationManager : public TKernelObject<IConfigurationManager>
		{
		public:

			CConfigurationManager(const IKernelContext& rKernelContext, IConfigurationManager* pParentConfigurationManager = NULL);

			virtual void clear(void);

			virtual bool addConfigurationFromFile(
				const CString& rFileNameWildCard);

			virtual CIdentifier createConfigurationToken(
				const CString& rConfigurationTokenName,
				const CString& rConfigurationTokenValue);
			virtual bool releaseConfigurationToken(
				const CIdentifier& rConfigurationTokenIdentifier);
			virtual CIdentifier getNextConfigurationTokenIdentifier(
				const CIdentifier& rPreviousConfigurationTokenIdentifier) const;

			virtual CString getConfigurationTokenName(
				const CIdentifier& rConfigurationTokenIdentifier) const;
			virtual CString getConfigurationTokenValue(
				const CIdentifier& rConfigurationTokenIdentifier) const;

			virtual bool setConfigurationTokenName(
				const CIdentifier& rConfigurationTokenIdentifier,
				const CString& rConfigurationTokenName);
			virtual bool setConfigurationTokenValue(
				const CIdentifier& rConfigurationTokenIdentifier,
				const CString& rConfigurationTokenValue);
			virtual bool addOrReplaceConfigurationToken(
				const CString& rConfigurationTokenName,
				const CString& rConfigurationTokenValue);

			virtual CIdentifier lookUpConfigurationTokenIdentifier(
				const CString& rConfigurationTokenName,
				const bool bRecursive) const;
			virtual CString lookUpConfigurationTokenValue(
				const CString& rConfigurationTokenName) const;

			virtual bool registerKeywordParser(
				const CString& rKeyword,
				const IConfigurationKeywordExpandCallback& rCallback);

			virtual bool unregisterKeywordParser(
				const CString& rKeyword);

			virtual bool unregisterKeywordParser(
				const IConfigurationKeywordExpandCallback& rCallback);

			virtual CString expand(
				const CString& rExpression) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IConfigurationManager >, OVK_ClassId_Kernel_Configuration_ConfigurationManager);

			virtual CString expandOnlyKeyword(
				const CString& rKeyword,
				const CString& rExpression,
				bool preserveBackslashes) const;
			virtual double expandAsFloat(
				const CString& rExpression,
				const double f64FallbackValue) const;
			virtual int64_t expandAsInteger(
				const CString& rExpression,
				const int64_t i64FallbackValue) const;
			virtual uint64_t expandAsUInteger(
				const CString& rExpression,
				const uint64_t ui64FallbackValue) const;
			virtual bool expandAsBoolean(
				const CString& rExpression,
				const bool bFallbackValue) const;
			virtual uint64_t expandAsEnumerationEntryValue(
				const CString& rExpression,
				const CIdentifier& rEnumerationTypeIdentifier,
				const uint64_t ui64FallbackValue) const;

		protected:

			virtual CIdentifier getUnusedIdentifier(void) const;

			bool internalExpand(const std::string& sValue, std::string& sResult) const;
			bool internalExpandOnlyKeyword(const std::string& sKeyword, const std::string& sValue, std::string& sResult, bool preserveBackslashes) const;
			bool internalGetConfigurationTokenValueFromName(const std::string& sTokenName, std::string& sTokenValue) const;

		protected:

			IConfigurationManager* m_pParentConfigurationManager;

		protected:

			mutable uint32_t m_ui32Index;
			mutable uint32_t m_ui32StartTime;

			uint32_t getRandom(void) const;
			uint32_t getIndex(void) const;
			CString getTime(void) const;
			CString getDate(void) const;
			uint32_t getRealTime(void) const;
			uint32_t getProcessId(void) const;

		protected:

			std::map<CIdentifier, SConfigurationToken> m_vConfigurationToken;
			std::map<CString, const IConfigurationKeywordExpandCallback*> m_vKeywordOverride;

			mutable std::recursive_mutex m_oMutex;
		};
	}
}

#endif // __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__

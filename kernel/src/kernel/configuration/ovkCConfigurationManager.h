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
			OpenViBE::CString m_sConfigurationName;
			OpenViBE::CString m_sConfigurationValue;
		} SConfigurationToken;

		class CConfigurationManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IConfigurationManager>
		{
		public:

			CConfigurationManager(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IConfigurationManager* pParentConfigurationManager = NULL);

			virtual void clear(void);

			virtual bool addConfigurationFromFile(
				const OpenViBE::CString& rFileNameWildCard);

			virtual OpenViBE::CIdentifier createConfigurationToken(
				const OpenViBE::CString& rConfigurationTokenName,
				const OpenViBE::CString& rConfigurationTokenValue);
			virtual bool releaseConfigurationToken(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier);
			virtual OpenViBE::CIdentifier getNextConfigurationTokenIdentifier(
				const OpenViBE::CIdentifier& rPreviousConfigurationTokenIdentifier) const;

			virtual OpenViBE::CString getConfigurationTokenName(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier) const;
			virtual OpenViBE::CString getConfigurationTokenValue(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier) const;

			virtual bool setConfigurationTokenName(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier,
				const OpenViBE::CString& rConfigurationTokenName);
			virtual bool setConfigurationTokenValue(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier,
				const OpenViBE::CString& rConfigurationTokenValue);
			virtual bool addOrReplaceConfigurationToken(
				const OpenViBE::CString& rConfigurationTokenName,
				const OpenViBE::CString& rConfigurationTokenValue);

			virtual OpenViBE::CIdentifier lookUpConfigurationTokenIdentifier(
				const OpenViBE::CString& rConfigurationTokenName,
				const bool bRecursive) const;
			virtual OpenViBE::CString lookUpConfigurationTokenValue(
				const OpenViBE::CString& rConfigurationTokenName) const;

			virtual bool registerKeywordParser(
				const OpenViBE::CString& rKeyword,
				const IConfigurationKeywordExpandCallback& rCallback);

			virtual bool unregisterKeywordParser(
				const OpenViBE::CString& rKeyword);

			virtual bool unregisterKeywordParser(
				const IConfigurationKeywordExpandCallback& rCallback);

			virtual OpenViBE::CString expand(
				const OpenViBE::CString& rExpression) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IConfigurationManager >, OVK_ClassId_Kernel_Configuration_ConfigurationManager);

			virtual OpenViBE::CString expandOnlyKeyword(
				const OpenViBE::CString& rKeyword,
				const OpenViBE::CString& rExpression,
				bool preserveBackslashes) const;
			virtual double expandAsFloat(
				const OpenViBE::CString& rExpression,
				const double f64FallbackValue) const;
			virtual int64_t expandAsInteger(
				const OpenViBE::CString& rExpression,
				const int64_t i64FallbackValue) const;
			virtual uint64_t expandAsUInteger(
				const OpenViBE::CString& rExpression,
				const uint64_t ui64FallbackValue) const;
			virtual bool expandAsBoolean(
				const OpenViBE::CString& rExpression,
				const bool bFallbackValue) const;
			virtual uint64_t expandAsEnumerationEntryValue(
				const OpenViBE::CString& rExpression,
				const OpenViBE::CIdentifier& rEnumerationTypeIdentifier,
				const uint64_t ui64FallbackValue) const;

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

			bool internalExpand(const std::string& sValue, std::string& sResult) const;
			bool internalExpandOnlyKeyword(const std::string& sKeyword, const std::string& sValue, std::string& sResult, bool preserveBackslashes) const;
			bool internalGetConfigurationTokenValueFromName(const std::string& sTokenName, std::string& sTokenValue) const;

		protected:

			OpenViBE::Kernel::IConfigurationManager* m_pParentConfigurationManager;

		protected:

			mutable uint32_t m_ui32Index;
			mutable uint32_t m_ui32StartTime;

			uint32_t getRandom(void) const;
			uint32_t getIndex(void) const;
			OpenViBE::CString getTime(void) const;
			OpenViBE::CString getDate(void) const;
			uint32_t getRealTime(void) const;
			uint32_t getProcessId(void) const;

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::SConfigurationToken> m_vConfigurationToken;
			std::map<OpenViBE::CString, const OpenViBE::Kernel::IConfigurationKeywordExpandCallback*> m_vKeywordOverride;

			mutable std::recursive_mutex m_oMutex;
		};
	}
}

#endif // __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__

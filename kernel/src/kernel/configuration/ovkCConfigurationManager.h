#pragma once

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

			CConfigurationManager(const IKernelContext& ctx, IConfigurationManager* pParentConfigurationManager = nullptr);
			void clear() override;
			bool addConfigurationFromFile(const CString& rFileNameWildCard) override;
			CIdentifier createConfigurationToken(const CString& rConfigurationTokenName, const CString& rConfigurationTokenValue) override;
			bool releaseConfigurationToken(const CIdentifier& rConfigurationTokenIdentifier) override;
			CIdentifier getNextConfigurationTokenIdentifier(const CIdentifier& rPreviousConfigurationTokenIdentifier) const override;
			CString getConfigurationTokenName(const CIdentifier& rConfigurationTokenIdentifier) const override;
			CString getConfigurationTokenValue(const CIdentifier& rConfigurationTokenIdentifier) const override;
			bool setConfigurationTokenName(const CIdentifier& rConfigurationTokenIdentifier, const CString& rConfigurationTokenName) override;
			bool setConfigurationTokenValue(const CIdentifier& rConfigurationTokenIdentifier, const CString& rConfigurationTokenValue) override;
			bool addOrReplaceConfigurationToken(const CString& rConfigurationTokenName, const CString& rConfigurationTokenValue) override;
			CIdentifier lookUpConfigurationTokenIdentifier(const CString& rConfigurationTokenName, bool bRecursive) const override;
			CString lookUpConfigurationTokenValue(const CString& rConfigurationTokenName) const override;
			bool registerKeywordParser(const CString& rKeyword, const IConfigurationKeywordExpandCallback& rCallback) override;
			bool unregisterKeywordParser(const CString& rKeyword) override;
			bool unregisterKeywordParser(const IConfigurationKeywordExpandCallback& rCallback) override;
			CString expand(const CString& rExpression) const override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IConfigurationManager >,
									   OVK_ClassId_Kernel_Configuration_ConfigurationManager)
			CString expandOnlyKeyword(const CString& rKeyword, const CString& rExpression, bool preserveBackslashes) const override;
			double expandAsFloat(const CString& rExpression, double f64FallbackValue) const override;
			int64_t expandAsInteger(const CString& rExpression, int64_t i64FallbackValue) const override;
			uint64_t expandAsUInteger(const CString& rExpression, uint64_t ui64FallbackValue) const override;
			bool expandAsBoolean(const CString& rExpression, bool bFallbackValue) const override;
			uint64_t expandAsEnumerationEntryValue(const CString& rExpression, const CIdentifier& rEnumerationTypeIdentifier,
												   uint64_t ui64FallbackValue) const override;

		protected:

			virtual CIdentifier getUnusedIdentifier() const;

			bool internalExpand(const std::string& sValue, std::string& sResult) const;
			bool internalExpandOnlyKeyword(const std::string& sKeyword, const std::string& sValue, std::string& sResult, bool preserveBackslashes) const;
			bool internalGetConfigurationTokenValueFromName(const std::string& sTokenName, std::string& sTokenValue) const;

			IConfigurationManager* m_pParentConfigurationManager = nullptr;
			mutable uint32_t m_ui32Index;
			mutable uint32_t m_ui32StartTime;

			uint32_t getRandom() const;
			uint32_t getIndex() const;
			CString getTime() const;
			CString getDate() const;
			uint32_t getRealTime() const;
			uint32_t getProcessId() const;

			std::map<CIdentifier, SConfigurationToken> m_vConfigurationToken;
			std::map<CString, const IConfigurationKeywordExpandCallback*> m_vKeywordOverride;

			mutable std::recursive_mutex m_oMutex;
		};
	} // namespace Kernel
} // namespace OpenViBE

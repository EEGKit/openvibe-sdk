#pragma once

#include "ovkTKernelObject.h"

#include <map>
#include <set>
#include <vector>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CTypeManager : public TKernelObject<ITypeManager>
		{
		public:

			explicit CTypeManager(const IKernelContext& rKernelContext);
			CIdentifier getNextTypeIdentifier(const CIdentifier& rPreviousIdentifier) const override;
			std::vector<std::pair<CIdentifier, CString>> getSortedTypes() const override;
			bool registerType(const CIdentifier& rTypeIdentifier, const CString& sTypeName) override;
			bool registerStreamType(const CIdentifier& rTypeIdentifier, const CString& sTypeName, const CIdentifier& rParentTypeIdentifier) override;
			bool registerEnumerationType(const CIdentifier& rTypeIdentifier, const CString& sTypeName) override;
			bool registerEnumerationEntry(const CIdentifier& rTypeIdentifier, const CString& sEntryName, uint64_t ui64EntryValue) override;
			bool registerBitMaskType(const CIdentifier& rTypeIdentifier, const CString& sTypeName) override;
			bool registerBitMaskEntry(const CIdentifier& rTypeIdentifier, const CString& sEntryName, uint64_t ui64EntryValue) override;
			bool isRegistered(const CIdentifier& rTypeIdentifier) const override;
			bool isStream(const CIdentifier& rTypeIdentifier) const override;
			bool isDerivedFromStream(const CIdentifier& rTypeIdentifier, const CIdentifier& rParentTypeIdentifier) const override;
			bool isEnumeration(const CIdentifier& rTypeIdentifier) const override;
			bool isBitMask(const CIdentifier& rTypeIdentifier) const override;
			CString getTypeName(const CIdentifier& rTypeIdentifier) const override;
			CIdentifier getStreamParentType(const CIdentifier& rTypeIdentifier) const override;
			uint64_t getEnumerationEntryCount(const CIdentifier& rTypeIdentifier) const override;
			bool getEnumerationEntry(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const override;
			CString getEnumerationEntryNameFromValue(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryValue) const override;
			uint64_t getEnumerationEntryValueFromName(const CIdentifier& rTypeIdentifier, const CString& rEntryName) const override;
			uint64_t getBitMaskEntryCount(const CIdentifier& rTypeIdentifier) const override;
			bool getBitMaskEntry(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const override;
			CString getBitMaskEntryNameFromValue(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryValue) const override;
			uint64_t getBitMaskEntryValueFromName(const CIdentifier& rTypeIdentifier, const CString& rEntryName) const override;
			CString getBitMaskEntryCompositionNameFromValue(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryCompositionValue) const override;
			uint64_t getBitMaskEntryCompositionValueFromName(const CIdentifier& rTypeIdentifier, const CString& rEntryCompositionName) const override;
			bool evaluateSettingValue(CString settingValue, double& numericResult) const override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ITypeManager>, OVK_ClassId_Kernel_TypeManager)

		protected:

			std::map<CIdentifier, CString> m_vName;
			std::set<CString> m_TakenNames;
			std::map<CIdentifier, std::map<uint64_t, CString>> m_vEnumeration;
			std::map<CIdentifier, std::map<uint64_t, CString>> m_vBitMask;
			std::map<CIdentifier, CIdentifier> m_vStream;

			mutable std::recursive_mutex m_oMutex;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



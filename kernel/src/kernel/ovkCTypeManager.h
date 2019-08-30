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
			bool registerType(const CIdentifier& typeID, const CString& sTypeName) override;
			bool registerStreamType(const CIdentifier& typeID, const CString& sTypeName, const CIdentifier& rParentTypeIdentifier) override;
			bool registerEnumerationType(const CIdentifier& typeID, const CString& sTypeName) override;
			bool registerEnumerationEntry(const CIdentifier& typeID, const CString& sEntryName, uint64_t ui64EntryValue) override;
			bool registerBitMaskType(const CIdentifier& typeID, const CString& sTypeName) override;
			bool registerBitMaskEntry(const CIdentifier& typeID, const CString& sEntryName, uint64_t ui64EntryValue) override;
			bool isRegistered(const CIdentifier& typeID) const override;
			bool isStream(const CIdentifier& typeID) const override;
			bool isDerivedFromStream(const CIdentifier& typeID, const CIdentifier& rParentTypeIdentifier) const override;
			bool isEnumeration(const CIdentifier& typeID) const override;
			bool isBitMask(const CIdentifier& typeID) const override;
			CString getTypeName(const CIdentifier& typeID) const override;
			CIdentifier getStreamParentType(const CIdentifier& typeID) const override;
			uint64_t getEnumerationEntryCount(const CIdentifier& typeID) const override;
			bool getEnumerationEntry(const CIdentifier& typeID, uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const override;
			CString getEnumerationEntryNameFromValue(const CIdentifier& typeID, uint64_t ui64EntryValue) const override;
			uint64_t getEnumerationEntryValueFromName(const CIdentifier& typeID, const CString& rEntryName) const override;
			uint64_t getBitMaskEntryCount(const CIdentifier& typeID) const override;
			bool getBitMaskEntry(const CIdentifier& typeID, uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const override;
			CString getBitMaskEntryNameFromValue(const CIdentifier& typeID, uint64_t ui64EntryValue) const override;
			uint64_t getBitMaskEntryValueFromName(const CIdentifier& typeID, const CString& rEntryName) const override;
			CString getBitMaskEntryCompositionNameFromValue(const CIdentifier& typeID, uint64_t ui64EntryCompositionValue) const override;
			uint64_t getBitMaskEntryCompositionValueFromName(const CIdentifier& typeID, const CString& rEntryCompositionName) const override;
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
	} // namespace Kernel
} // namespace OpenViBE

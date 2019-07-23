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

			virtual CIdentifier getNextTypeIdentifier(const CIdentifier& rPreviousIdentifier) const;

			virtual std::vector<std::pair<CIdentifier, CString>> getSortedTypes() const;

			virtual bool registerType(const CIdentifier& rTypeIdentifier, const CString& sTypeName);

			virtual bool registerStreamType(const CIdentifier& rTypeIdentifier, const CString& sTypeName, const CIdentifier& rParentTypeIdentifier);


			virtual bool registerEnumerationType(const CIdentifier& rTypeIdentifier, const CString& sTypeName);
			virtual bool registerEnumerationEntry(const CIdentifier& rTypeIdentifier, const CString& sEntryName, uint64_t ui64EntryValue);

			virtual bool registerBitMaskType(const CIdentifier& rTypeIdentifier, const CString& sTypeName);
			virtual bool registerBitMaskEntry(const CIdentifier& rTypeIdentifier, const CString& sEntryName, uint64_t ui64EntryValue);

			virtual bool isRegistered(const CIdentifier& rTypeIdentifier) const;
			virtual bool isStream(const CIdentifier& rTypeIdentifier) const;
			virtual bool isDerivedFromStream(const CIdentifier& rTypeIdentifier, const CIdentifier& rParentTypeIdentifier) const;
			virtual bool isEnumeration(const CIdentifier& rTypeIdentifier) const;
			virtual bool isBitMask(const CIdentifier& rTypeIdentifier) const;

			virtual CString getTypeName(const CIdentifier& rTypeIdentifier) const;
			virtual CIdentifier getStreamParentType(const CIdentifier& rTypeIdentifier) const;

			virtual uint64_t getEnumerationEntryCount(const CIdentifier& rTypeIdentifier) const;
			virtual bool getEnumerationEntry(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const;
			virtual CString getEnumerationEntryNameFromValue(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryValue) const;
			virtual uint64_t getEnumerationEntryValueFromName(const CIdentifier& rTypeIdentifier, const CString& rEntryName) const;

			virtual uint64_t getBitMaskEntryCount(const CIdentifier& rTypeIdentifier) const;
			virtual bool getBitMaskEntry(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const;
			virtual CString getBitMaskEntryNameFromValue(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryValue) const;
			virtual uint64_t getBitMaskEntryValueFromName(const CIdentifier& rTypeIdentifier, const CString& rEntryName) const;
			virtual CString getBitMaskEntryCompositionNameFromValue(const CIdentifier& rTypeIdentifier, uint64_t ui64EntryCompositionValue) const;
			virtual uint64_t getBitMaskEntryCompositionValueFromName(const CIdentifier& rTypeIdentifier, const CString& rEntryCompositionName) const;

			virtual bool evaluateSettingValue(CString settingValue, double& numericResult) const;

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



#ifndef __OpenViBEKernel_Kernel_CTypeManager_H__
#define __OpenViBEKernel_Kernel_CTypeManager_H__

#include "ovkTKernelObject.h"

#include <map>
#include <set>
#include <vector>
#include <mutex>

namespace OpenViBE
{
	namespace Kernel
	{
		class CTypeManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ITypeManager>
		{
		public:

			explicit CTypeManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::CIdentifier getNextTypeIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			virtual std::vector<std::pair<CIdentifier, CString>> getSortedTypes() const;

			virtual bool registerType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName);

			virtual bool registerStreamType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName,
				const OpenViBE::CIdentifier& rParentTypeIdentifier);


			virtual bool registerEnumerationType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName);
			virtual bool registerEnumerationEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sEntryName,
				const uint64_t ui64EntryValue);

			virtual bool registerBitMaskType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName);
			virtual bool registerBitMaskEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sEntryName,
				const uint64_t ui64EntryValue);

			virtual bool isRegistered(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual bool isStream(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual bool isDerivedFromStream(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CIdentifier& rParentTypeIdentifier) const;
			virtual bool isEnumeration(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual bool isBitMask(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;

			virtual OpenViBE::CString getTypeName(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::CIdentifier getStreamParentType(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;

			virtual uint64_t getEnumerationEntryCount(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual bool getEnumerationEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const uint64_t ui64EntryIndex,
				OpenViBE::CString& sEntryName,
				uint64_t& rEntryValue) const;
			virtual OpenViBE::CString getEnumerationEntryNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const uint64_t ui64EntryValue) const;
			virtual uint64_t getEnumerationEntryValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryName) const;

			virtual uint64_t getBitMaskEntryCount(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual bool getBitMaskEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const uint64_t ui64EntryIndex,
				OpenViBE::CString& sEntryName,
				uint64_t& rEntryValue) const;
			virtual OpenViBE::CString getBitMaskEntryNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const uint64_t ui64EntryValue) const;
			virtual uint64_t getBitMaskEntryValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryName) const;
			virtual OpenViBE::CString getBitMaskEntryCompositionNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const uint64_t ui64EntryCompositionValue) const;
			virtual uint64_t getBitMaskEntryCompositionValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryCompositionName) const;

			virtual bool evaluateSettingValue(
				const CString settingValue,
				double& numericResult) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ITypeManager>, OVK_ClassId_Kernel_TypeManager);

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_vName;
			std::set<OpenViBE::CString> m_TakenNames;
			std::map<OpenViBE::CIdentifier, std::map<uint64_t, OpenViBE::CString>> m_vEnumeration;
			std::map<OpenViBE::CIdentifier, std::map<uint64_t, OpenViBE::CString>> m_vBitMask;
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_vStream;

			mutable std::recursive_mutex m_oMutex;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CTypeManager_H__

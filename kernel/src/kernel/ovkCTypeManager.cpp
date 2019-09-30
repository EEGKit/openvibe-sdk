#include "ovkCTypeManager.h"

#include "lepton/Lepton.h"

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstdio>
#include <algorithm>

#define OV_TRACE_K(message) this->getLogManager() << OpenViBE::Kernel::LogLevel_Trace << message << "\n";

#define OV_DEBUG_K(message) this->getLogManager() << OpenViBE::Kernel::LogLevel_Debug << message << "\n";

#define OV_DEBUG_UNLESS_K(expression, message) if (!(expression)) { OV_DEBUG_K(message); }

using namespace OpenViBE;
using namespace Kernel;

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class TCharT>
	TCharT ToLower(TCharT c) { return std::tolower(c); }

	struct a_inf_b
	{
		bool operator()(std::pair<CIdentifier, CString> a, std::pair<CIdentifier, CString> b) { return a.second < b.second; }
	};
} // namespace

CTypeManager::CTypeManager(const IKernelContext& ctx)
	: TKernelObject<ITypeManager>(ctx)
{
	m_vName[OV_UndefinedIdentifier] = "undefined";
	this->registerEnumerationType(OV_TypeId_BoxAlgorithmFlag, "BoxFlags");
}

CIdentifier CTypeManager::getNextTypeIdentifier(const CIdentifier& previousID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return getNextIdentifier<CString>(m_vName, previousID);
}

std::vector<std::pair<CIdentifier, CString>> CTypeManager::getSortedTypes() const
{
	std::vector<std::pair<CIdentifier, CString>> sorted;

	for (const auto& element : m_vName) { sorted.push_back(std::pair<CIdentifier, CString>(element.first, element.second)); }
	std::sort(sorted.begin(), sorted.end(), a_inf_b());

	return sorted;
}

bool CTypeManager::registerType(const CIdentifier& typeID, const CString& sTypeName)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	OV_ERROR_UNLESS_KRF(!isRegistered(typeID), "Trying to register type " << typeID.toString() << " that already exists.",
						OpenViBE::Kernel::ErrorType::BadArgument);

	OV_DEBUG_UNLESS_K(m_TakenNames.find(sTypeName) == m_TakenNames.end(),
					  "Trying to register type " << typeID << " with a name that already exists ( " << sTypeName << ")");

	m_vName[typeID] = sTypeName;
	OV_TRACE_K("Registered type id " << typeID << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerStreamType(const CIdentifier& typeID, const CString& sTypeName, const CIdentifier& rParentTypeIdentifier)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	OV_ERROR_UNLESS_KRF(!isRegistered(typeID), "Trying to register stream type " << typeID.toString() << " that already exists.",
						OpenViBE::Kernel::ErrorType::BadArgument);

	OV_DEBUG_UNLESS_K(m_TakenNames.find(sTypeName) == m_TakenNames.end(),
					  "Trying to register stream type " << typeID << " with a name that already exists ( " << sTypeName << ")");

	OV_ERROR_UNLESS_KRF(rParentTypeIdentifier == OV_UndefinedIdentifier || isStream(rParentTypeIdentifier),
						"Trying to register an invalid stream type [" << sTypeName << "] " << typeID.toString() << ", parent : " << rParentTypeIdentifier.
						toString() << ".",
						OpenViBE::Kernel::ErrorType::BadArgument);

	m_vName[typeID] = sTypeName;
	m_TakenNames.insert(sTypeName);
	m_vStream[typeID] = rParentTypeIdentifier;
	OV_TRACE_K("Registered stream type id " << typeID << "::" << rParentTypeIdentifier << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerEnumerationType(const CIdentifier& typeID, const CString& sTypeName)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	if (isRegistered(typeID))
	{
		if (m_vName[typeID] != sTypeName)
		{
			OV_ERROR_KRF(
				"Trying to register enum type " << typeID.toString() << " that already exists with different value (" << m_vName[typeID] << " != " << sTypeName
				<< ")",
				OpenViBE::Kernel::ErrorType::BadArgument);
		}
		OV_DEBUG_K("Trying to register enum type " << typeID.toString() << " that already exists.");
	}

	OV_DEBUG_UNLESS_K(m_TakenNames.find(sTypeName) == m_TakenNames.end(),
					  "Trying to register enum type " << typeID << " with a name that already exists ( " << sTypeName << ")");

	m_vName[typeID] = sTypeName;
	m_TakenNames.insert(sTypeName);
	m_vEnumeration[typeID];
	OV_TRACE_K("Registered enumeration type id " << typeID << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerEnumerationEntry(const CIdentifier& typeID, const CString& sEntryName, const uint64_t ui64EntryValue)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	auto itEnumeration = m_vEnumeration.find(typeID);

	OV_ERROR_UNLESS_KRF(itEnumeration != m_vEnumeration.end(), "Enumeration type [" << typeID.toString() << "] does not exist." << sEntryName,
						OpenViBE::Kernel::ErrorType::BadArgument);

	auto itElem = itEnumeration->second.find(ui64EntryValue);
	if (itElem != itEnumeration->second.end())
	{
		if (std::string(itElem->second) != std::string(sEntryName))
		{
			OV_WARNING_K(
				"Enumeration type [" << typeID.toString() << "] already has element [" << ui64EntryValue << "]. Value will be overriden : " << itElem->second <<
				" => " << sEntryName);
		}
		else { OV_DEBUG_K("Enumeration type [" << typeID.toString() << "] already has element [" << ui64EntryValue << "]."); }
	}

	itEnumeration->second[ui64EntryValue] = sEntryName;
	return true;
}

bool CTypeManager::registerBitMaskType(const CIdentifier& typeID, const CString& sTypeName)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	OV_ERROR_UNLESS_KRF(!isRegistered(typeID), "Trying to register bitmask type " << typeID.toString() << " that already exists.",
						OpenViBE::Kernel::ErrorType::BadArgument);

	OV_DEBUG_UNLESS_K(m_TakenNames.find(sTypeName) == m_TakenNames.end(),
					  "Trying to register bitmask type " << typeID << " with a name that already exists ( " << sTypeName << ")");

	m_vName[typeID] = sTypeName;
	m_vBitMask[typeID];
	OV_TRACE_K("Registered bitmask type id " << typeID << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerBitMaskEntry(const CIdentifier& typeID, const CString& sEntryName, const uint64_t ui64EntryValue)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	auto itBitMask = m_vBitMask.find(typeID);
	OV_ERROR_UNLESS_KRF(itBitMask != m_vBitMask.end(), "Bitmask type [" << typeID.toString() << "] does not exist.", OpenViBE::Kernel::ErrorType::BadArgument);

	auto itElem = itBitMask->second.find(ui64EntryValue);
	if (itElem != itBitMask->second.end())
	{
		if (std::string(itElem->second) != std::string(sEntryName))
		{
			OV_WARNING_K(
				"Bitmask type [" << typeID.toString() << "] already has element [" << ui64EntryValue << "]. Value will be overriden : " << itElem->second <<
				" => " << sEntryName);
		}
		else { OV_DEBUG_K("Bitmask type [" << typeID.toString() << "] already has element [" << ui64EntryValue << "]."); }
	}

	for (uint32_t l_ui32BitCount = 0, i = 0; i < 64; i++)
	{
		if (ui64EntryValue & (1LL << i))
		{
			l_ui32BitCount++;
			OV_ERROR_UNLESS_KRF(l_ui32BitCount <= 1,
								"Discarded bitmask entry (" << m_vName[typeID] << ":" << sEntryName << ") because value " << ui64EntryValue <<
								" contains more than one bit",
								ErrorType::Overflow);
		}
	}
	itBitMask->second[ui64EntryValue] = sEntryName;
	return true;
}

bool CTypeManager::isRegistered(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vName.find(typeID) != m_vName.end();
}

bool CTypeManager::isStream(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);
	return m_vStream.find(typeID) != m_vStream.end();
}

bool CTypeManager::isDerivedFromStream(const CIdentifier& typeID, const CIdentifier& rParentTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	auto it             = m_vStream.find(typeID);
	const auto itParent = m_vStream.find(rParentTypeIdentifier);
	if (it == m_vStream.end()) { return false; }
	if (itParent == m_vStream.end()) { return false; }
	while (it != m_vStream.end())
	{
		if (it->first == rParentTypeIdentifier) { return true; }
		it = m_vStream.find(it->second);
	}
	return false;
}

bool CTypeManager::isEnumeration(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vEnumeration.find(typeID) != m_vEnumeration.end();
}

bool CTypeManager::isBitMask(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vBitMask.find(typeID) != m_vBitMask.end();
}

CString CTypeManager::getTypeName(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	if (!isRegistered(typeID)) { return CString(""); }
	return m_vName.find(typeID)->second;
}

CIdentifier CTypeManager::getStreamParentType(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	if (!isStream(typeID)) { return OV_UndefinedIdentifier; }
	return m_vStream.find(typeID)->second;
}

uint64_t CTypeManager::getEnumerationEntryCount(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(typeID);
	if (itEnumeration == m_vEnumeration.end()) { return 0; }
	return itEnumeration->second.size();
}

bool CTypeManager::getEnumerationEntry(const CIdentifier& typeID, const uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(typeID);
	if (itEnumeration == m_vEnumeration.end()) { return false; }

	if (ui64EntryIndex >= itEnumeration->second.size()) { return false; }

	auto itEnumerationEntry = itEnumeration->second.begin();
	for (uint64_t i = 0; i < ui64EntryIndex && itEnumerationEntry != itEnumeration->second.end(); i++, ++itEnumerationEntry) { }

	rEntryValue = itEnumerationEntry->first;
	sEntryName  = itEnumerationEntry->second;
	return true;
}

CString CTypeManager::getEnumerationEntryNameFromValue(const CIdentifier& typeID, const uint64_t ui64EntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(typeID);
	if (itEnumeration == m_vEnumeration.end()) { return ""; }
	const auto itEnumerationEntry = itEnumeration->second.find(ui64EntryValue);
	if (itEnumerationEntry == itEnumeration->second.end()) { return ""; }
	return itEnumeration->second.find(ui64EntryValue)->second;
}

uint64_t CTypeManager::getEnumerationEntryValueFromName(const CIdentifier& typeID, const CString& rEntryName) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(typeID);
	if (itEnumeration == m_vEnumeration.end()) { return OV_IncorrectStimulation; }

	// first looks at the exact std::string match
	for (const auto& entry : itEnumeration->second) { if (entry.second == rEntryName) { return entry.first; } }

	// then looks at the caseless std::string match
	std::string l_sEntryNameLower = rEntryName.toASCIIString();
	std::transform(l_sEntryNameLower.begin(), l_sEntryNameLower.end(), l_sEntryNameLower.begin(), ::ToLower<std::string::value_type>);
	for (const auto& entry : itEnumeration->second)
	{
		std::string l_sItEntryNameLower = entry.second.toASCIIString();
		std::transform(l_sItEntryNameLower.begin(), l_sItEntryNameLower.end(), l_sItEntryNameLower.begin(), ::ToLower<std::string::value_type>);
		if (l_sItEntryNameLower == l_sEntryNameLower) { return entry.first; }
	}

	// then looks at the std::string being the value itself
	try
	{
		uint64_t l_ui64Value = std::stoull(static_cast<const char*>(rEntryName));

		if ((itEnumeration->second.find(l_ui64Value) != itEnumeration->second.end()) ||
			(typeID == OV_TypeId_Stimulation && this->getConfigurationManager().expandAsBoolean("Kernel_AllowUnregisteredNumericalStimulationIdentifiers")))
		{
			return l_ui64Value;
		}
	}
	catch (const std::exception&) { return OV_IncorrectStimulation; }

	return OV_IncorrectStimulation;
}

uint64_t CTypeManager::getBitMaskEntryCount(const CIdentifier& typeID) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(typeID);
	if (itBitMask == m_vBitMask.end()) { return 0; }
	return itBitMask->second.size();
}

bool CTypeManager::getBitMaskEntry(const CIdentifier& typeID, const uint64_t ui64EntryIndex, CString& sEntryName, uint64_t& rEntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(typeID);
	if (itBitMask == m_vBitMask.end()) { return false; }

	if (ui64EntryIndex >= itBitMask->second.size()) { return false; }

	auto itBitMaskEntry = itBitMask->second.begin();
	for (uint64_t i = 0; i < ui64EntryIndex && itBitMaskEntry != itBitMask->second.end(); i++, ++itBitMaskEntry) { }

	rEntryValue = itBitMaskEntry->first;
	sEntryName  = itBitMaskEntry->second;
	return true;
}

CString CTypeManager::getBitMaskEntryNameFromValue(const CIdentifier& typeID, const uint64_t ui64EntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(typeID);
	if (itBitMask == m_vBitMask.end()) { return ""; }
	const auto itBitMaskEntry = itBitMask->second.find(ui64EntryValue);
	if (itBitMaskEntry == itBitMask->second.end()) { return ""; }
	return itBitMask->second.find(ui64EntryValue)->second;
}

uint64_t CTypeManager::getBitMaskEntryValueFromName(const CIdentifier& typeID, const CString& rEntryName) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(typeID);
	if (itBitMask == m_vBitMask.end()) { return 0xffffffffffffffffLL; }

	// first looks at the exact std::string match
	for (const auto& mask : itBitMask->second) { if (mask.second == rEntryName) { return mask.first; } }

	// then looks at the caseless std::string match
	std::string l_sEntryNameLower = rEntryName.toASCIIString();
	std::transform(l_sEntryNameLower.begin(), l_sEntryNameLower.end(), l_sEntryNameLower.begin(), ::ToLower<std::string::value_type>);
	for (const auto& mask : itBitMask->second)
	{
		std::string l_sItEntryNameLower = mask.second.toASCIIString();
		std::transform(l_sItEntryNameLower.begin(), l_sItEntryNameLower.end(), l_sItEntryNameLower.begin(), ::ToLower<std::string::value_type>);
		if (l_sItEntryNameLower == l_sEntryNameLower) { return mask.first; }
	}

	// then looks at the std::string being the value itself
	try
	{
		uint64_t l_ui64Value = std::stoll(static_cast<const char*>(rEntryName));

		if (itBitMask->second.find(l_ui64Value) != itBitMask->second.end()) { return l_ui64Value; }
	}
	catch (const std::exception&) { return 0xffffffffffffffffLL; }

	return 0xffffffffffffffffLL;
}

CString CTypeManager::getBitMaskEntryCompositionNameFromValue(const CIdentifier& typeID, const uint64_t ui64EntryCompositionValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(typeID);
	if (itBitMask == m_vBitMask.end()) { return ""; }

	std::string res;
	for (uint32_t i = 0; i < 64; i++)
	{
		if (ui64EntryCompositionValue & (1LL << i))
		{
			const auto itBitMaskEntry = itBitMask->second.find(ui64EntryCompositionValue & (1LL << i));
			if (itBitMaskEntry == itBitMask->second.end()) { return ""; }
			if (res.empty()) { res = itBitMaskEntry->second.toASCIIString(); }
			else
			{
				res += std::string(1, OV_Value_EnumeratedStringSeparator);
				res += itBitMaskEntry->second.toASCIIString();
			}
		}
	}
	return CString(res.c_str());
}

uint64_t CTypeManager::getBitMaskEntryCompositionValueFromName(const CIdentifier& typeID, const CString& rEntryCompositionName) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(typeID);
	if (itBitMask == m_vBitMask.end()) { return 0; }

	uint64_t res               = 0;
	std::string l_sEntryCompositionName = rEntryCompositionName.toASCIIString();
	size_t i                            = 0;
	size_t j                            = 0;
	do
	{
		i = l_sEntryCompositionName.find(OV_Value_EnumeratedStringSeparator, i);
		if (i == std::string::npos) { i = l_sEntryCompositionName.length(); }

		if (i != j)
		{
			std::string l_sEntryName;
			l_sEntryName.assign(l_sEntryCompositionName, j, i - j);

			bool l_bFound = false;
			for (const auto& mask : itBitMask->second)
			{
				if (mask.second == CString(l_sEntryName.c_str()))
				{
					res |= mask.first;
					l_bFound = true;
				}
			}

			if (!l_bFound) { return 0; }
		}

		i++;
		j = i;
	} while (i < l_sEntryCompositionName.length());

	return res;
}

bool CTypeManager::evaluateSettingValue(const CString settingValue, double& numericResult) const
{
	// parse and expression with no variables or functions
	try { numericResult = Lepton::Parser::parse(settingValue.toASCIIString()).evaluate(); }
	catch (...) { return false; }
	return true;
}

#include "ovkCTypeManager.h"

#include "lepton/Lepton.h"

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstdio>
#include <algorithm>

#define OV_TRACE_K(message) \
this->getLogManager() << OpenViBE::Kernel::LogLevel_Trace << message << "\n";

#define OV_DEBUG_K(message) \
this->getLogManager() << OpenViBE::Kernel::LogLevel_Debug << message << "\n";

#define OV_DEBUG_UNLESS_K(expression, message) \
if (!(expression)) \
{ \
	 OV_DEBUG_K(message); \
}

using namespace OpenViBE;
using namespace Kernel;

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}

	struct a_inf_b
	{
		inline bool operator()(std::pair<CIdentifier, CString> a, std::pair<CIdentifier, CString> b)
		{
			return a.second < b.second;
		}
	};
};

CTypeManager::CTypeManager(const IKernelContext& rKernelContext)
	: TKernelObject<ITypeManager>(rKernelContext)
{
	m_vName[OV_UndefinedIdentifier] = "undefined";
	this->registerEnumerationType(OV_TypeId_BoxAlgorithmFlag, "BoxFlags");
}

CIdentifier CTypeManager::getNextTypeIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return getNextIdentifier<CString>(m_vName, rPreviousIdentifier);
}

std::vector<std::pair<CIdentifier, CString>> CTypeManager::getSortedTypes() const
{
	std::vector<std::pair<CIdentifier, CString>> l_oSorted;

	for (auto element : m_vName)
	{
		l_oSorted.push_back(std::pair<CIdentifier, CString>(element.first, element.second));
	}
	std::sort(l_oSorted.begin(), l_oSorted.end(), a_inf_b());

	return l_oSorted;
}

bool CTypeManager::registerType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	OV_ERROR_UNLESS_KRF(
		!isRegistered(rTypeIdentifier),
		"Trying to register type " << rTypeIdentifier.toString() << " that already exists.",
		OpenViBE::Kernel::ErrorType::BadArgument);

	OV_DEBUG_UNLESS_K(
		m_TakenNames.find(sTypeName) == m_TakenNames.end(),
		"Trying to register type " << rTypeIdentifier << " with a name that already exists ( " << sTypeName << ")");

	m_vName[rTypeIdentifier] = sTypeName;
	OV_TRACE_K("Registered type id " << rTypeIdentifier << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerStreamType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName,
	const CIdentifier& rParentTypeIdentifier)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	OV_ERROR_UNLESS_KRF(
		!isRegistered(rTypeIdentifier),
		"Trying to register stream type " << rTypeIdentifier.toString() << " that already exists.",
		OpenViBE::Kernel::ErrorType::BadArgument);

	OV_DEBUG_UNLESS_K(
		m_TakenNames.find(sTypeName) == m_TakenNames.end(),
		"Trying to register stream type " << rTypeIdentifier << " with a name that already exists ( " << sTypeName << ")");

	OV_ERROR_UNLESS_KRF(
		rParentTypeIdentifier == OV_UndefinedIdentifier || isStream(rParentTypeIdentifier),
		"Trying to register an invalid stream type [" << sTypeName << "] " << rTypeIdentifier.toString() << ", parent : " << rParentTypeIdentifier.toString() << ".",
		OpenViBE::Kernel::ErrorType::BadArgument);

	m_vName[rTypeIdentifier] = sTypeName;
	m_TakenNames.insert(sTypeName);
	m_vStream[rTypeIdentifier] = rParentTypeIdentifier;
	OV_TRACE_K("Registered stream type id " << rTypeIdentifier << "::" << rParentTypeIdentifier << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerEnumerationType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	if (isRegistered(rTypeIdentifier))
	{
		if (m_vName[rTypeIdentifier] != sTypeName)
		{
			OV_ERROR_KRF(
				"Trying to register enum type " << rTypeIdentifier.toString() << " that already exists with different value (" << m_vName[rTypeIdentifier] << " != " << sTypeName << ")",
				OpenViBE::Kernel::ErrorType::BadArgument);
		}
		else
		{
			OV_DEBUG_K("Trying to register enum type " << rTypeIdentifier.toString() << " that already exists.");
		}
	}

	OV_DEBUG_UNLESS_K(
		m_TakenNames.find(sTypeName) == m_TakenNames.end(),
		"Trying to register enum type " << rTypeIdentifier << " with a name that already exists ( " << sTypeName << ")");

	m_vName[rTypeIdentifier] = sTypeName;
	m_TakenNames.insert(sTypeName);
	m_vEnumeration[rTypeIdentifier];
	OV_TRACE_K("Registered enumeration type id " << rTypeIdentifier << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerEnumerationEntry(
	const CIdentifier& rTypeIdentifier,
	const CString& sEntryName,
	const uint64_t ui64EntryValue)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	auto itEnumeration = m_vEnumeration.find(rTypeIdentifier);

	OV_ERROR_UNLESS_KRF(
		itEnumeration != m_vEnumeration.end(),
		"Enumeration type [" << rTypeIdentifier.toString() << "] does not exist." << sEntryName,
		OpenViBE::Kernel::ErrorType::BadArgument);

	auto itElem = itEnumeration->second.find(ui64EntryValue);
	if (itElem != itEnumeration->second.end())
	{
		if (std::string(itElem->second) != std::string(sEntryName))
		{
			OV_WARNING_K("Enumeration type [" << rTypeIdentifier.toString() << "] already has element [" << ui64EntryValue << "]. Value will be overriden : " << itElem->second << " => " << sEntryName);
		}
		else
		{
			OV_DEBUG_K("Enumeration type [" << rTypeIdentifier.toString() << "] already has element [" << ui64EntryValue << "].");
		}
	}

	itEnumeration->second[ui64EntryValue] = sEntryName;
	return true;
}

bool CTypeManager::registerBitMaskType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	OV_ERROR_UNLESS_KRF(
		!isRegistered(rTypeIdentifier),
		"Trying to register bitmask type " << rTypeIdentifier.toString() << " that already exists.",
		OpenViBE::Kernel::ErrorType::BadArgument);

	OV_DEBUG_UNLESS_K(
		m_TakenNames.find(sTypeName) == m_TakenNames.end(),
		"Trying to register bitmask type " << rTypeIdentifier << " with a name that already exists ( " << sTypeName << ")");

	m_vName[rTypeIdentifier] = sTypeName;
	m_vBitMask[rTypeIdentifier];
	OV_TRACE_K("Registered bitmask type id " << rTypeIdentifier << " - " << sTypeName);
	return true;
}

bool CTypeManager::registerBitMaskEntry(
	const CIdentifier& rTypeIdentifier,
	const CString& sEntryName,
	const uint64_t ui64EntryValue)
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	OV_ERROR_UNLESS_KRF(
		itBitMask != m_vBitMask.end(),
		"Bitmask type [" << rTypeIdentifier.toString() << "] does not exist.",
		OpenViBE::Kernel::ErrorType::BadArgument);

	auto itElem = itBitMask->second.find(ui64EntryValue);
	if (itElem != itBitMask->second.end())
	{
		if (std::string(itElem->second) != std::string(sEntryName))
		{
			OV_WARNING_K("Bitmask type [" << rTypeIdentifier.toString() << "] already has element [" << ui64EntryValue << "]. Value will be overriden : " << itElem->second << " => " << sEntryName);
		}
		else
		{
			OV_DEBUG_K("Bitmask type [" << rTypeIdentifier.toString() << "] already has element [" << ui64EntryValue << "].");
		}
	}

	for (uint32_t l_ui32BitCount = 0, i = 0; i < 64; i++)
	{
		if (ui64EntryValue & (1LL << i))
		{
			l_ui32BitCount++;
			OV_ERROR_UNLESS_KRF(
				l_ui32BitCount <= 1,
				"Discarded bitmask entry (" << m_vName[rTypeIdentifier] << ":" << sEntryName << ") because value " << ui64EntryValue << " contains more than one bit",
				ErrorType::Overflow
			);
		}
	}
	itBitMask->second[ui64EntryValue] = sEntryName;
	return true;
}

bool CTypeManager::isRegistered(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vName.find(rTypeIdentifier) != m_vName.end();
}

bool CTypeManager::isStream(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vStream.find(rTypeIdentifier) != m_vStream.end();
}

bool CTypeManager::isDerivedFromStream(
	const CIdentifier& rTypeIdentifier,
	const CIdentifier& rParentTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	auto it             = m_vStream.find(rTypeIdentifier);
	const auto itParent = m_vStream.find(rParentTypeIdentifier);
	if (it == m_vStream.end()) return false;
	if (itParent == m_vStream.end()) return false;
	while (it != m_vStream.end())
	{
		if (it->first == rParentTypeIdentifier) { return true; }
		it = m_vStream.find(it->second);
	}
	return false;
}

bool CTypeManager::isEnumeration(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vEnumeration.find(rTypeIdentifier) != m_vEnumeration.end();
}

bool CTypeManager::isBitMask(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	return m_vBitMask.find(rTypeIdentifier) != m_vBitMask.end();
}

CString CTypeManager::getTypeName(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	if (!isRegistered(rTypeIdentifier))
	{
		return CString("");
	}
	return m_vName.find(rTypeIdentifier)->second;
}

CIdentifier CTypeManager::getStreamParentType(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	if (!isStream(rTypeIdentifier))
	{
		return OV_UndefinedIdentifier;
	}
	return m_vStream.find(rTypeIdentifier)->second;
}

uint64_t CTypeManager::getEnumerationEntryCount(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(rTypeIdentifier);
	if (itEnumeration == m_vEnumeration.end()) { return 0; }
	return itEnumeration->second.size();
}

bool CTypeManager::getEnumerationEntry(
	const CIdentifier& rTypeIdentifier,
	const uint64_t ui64EntryIndex,
	CString& sEntryName,
	uint64_t& rEntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(rTypeIdentifier);
	if (itEnumeration == m_vEnumeration.end()) { return false; }

	if (ui64EntryIndex >= itEnumeration->second.size()) { return false; }

	auto itEnumerationEntry = itEnumeration->second.begin();
	for (uint64_t i = 0; i < ui64EntryIndex && itEnumerationEntry != itEnumeration->second.end(); i++, ++itEnumerationEntry) { }

	rEntryValue = itEnumerationEntry->first;
	sEntryName  = itEnumerationEntry->second;
	return true;
}

CString CTypeManager::getEnumerationEntryNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64_t ui64EntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(rTypeIdentifier);
	if (itEnumeration == m_vEnumeration.end())
	{
		return "";
	}
	const auto itEnumerationEntry = itEnumeration->second.find(ui64EntryValue);
	if (itEnumerationEntry == itEnumeration->second.end())
	{
		return "";
	}
	return itEnumeration->second.find(ui64EntryValue)->second;
}

uint64_t CTypeManager::getEnumerationEntryValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryName) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itEnumeration = m_vEnumeration.find(rTypeIdentifier);
	if (itEnumeration == m_vEnumeration.end())
	{
		return OV_IncorrectStimulation;
	}

	// first looks at the exact std::string match
	for (const auto& entry : itEnumeration->second)
	{
		if (entry.second == rEntryName)
		{
			return entry.first;
		}
	}

	// then looks at the caseless std::string match
	std::string l_sEntryNameLower = rEntryName.toASCIIString();
	std::transform(l_sEntryNameLower.begin(), l_sEntryNameLower.end(), l_sEntryNameLower.begin(), ::to_lower<std::string::value_type>);
	for (const auto& entry : itEnumeration->second)
	{
		std::string l_sItEntryNameLower = entry.second.toASCIIString();
		std::transform(l_sItEntryNameLower.begin(), l_sItEntryNameLower.end(), l_sItEntryNameLower.begin(), ::to_lower<std::string::value_type>);
		if (l_sItEntryNameLower == l_sEntryNameLower)
		{
			return entry.first;
		}
	}

	// then looks at the std::string being the value itself
	try
	{
		uint64_t l_ui64Value = std::stoull((const char*)rEntryName);

		if ((itEnumeration->second.find(l_ui64Value) != itEnumeration->second.end()) ||
			(rTypeIdentifier == OV_TypeId_Stimulation && this->getConfigurationManager().expandAsBoolean("Kernel_AllowUnregisteredNumericalStimulationIdentifiers")))
		{
			return l_ui64Value;
		}
	}
	catch (const std::exception&)
	{
		return OV_IncorrectStimulation;
	}

	return OV_IncorrectStimulation;
}

uint64_t CTypeManager::getBitMaskEntryCount(
	const CIdentifier& rTypeIdentifier) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	if (itBitMask == m_vBitMask.end()) { return 0; }
	return itBitMask->second.size();
}

bool CTypeManager::getBitMaskEntry(
	const CIdentifier& rTypeIdentifier,
	const uint64_t ui64EntryIndex,
	CString& sEntryName,
	uint64_t& rEntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	if (itBitMask == m_vBitMask.end()) { return false; }

	if (ui64EntryIndex >= itBitMask->second.size()) { return false; }

	auto itBitMaskEntry = itBitMask->second.begin();
	for (uint64_t i = 0; i < ui64EntryIndex && itBitMaskEntry != itBitMask->second.end(); i++, ++itBitMaskEntry) { }

	rEntryValue = itBitMaskEntry->first;
	sEntryName  = itBitMaskEntry->second;
	return true;
}

CString CTypeManager::getBitMaskEntryNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64_t ui64EntryValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	if (itBitMask == m_vBitMask.end())
	{
		return "";
	}
	const auto itBitMaskEntry = itBitMask->second.find(ui64EntryValue);
	if (itBitMaskEntry == itBitMask->second.end())
	{
		return "";
	}
	return itBitMask->second.find(ui64EntryValue)->second;
}

uint64_t CTypeManager::getBitMaskEntryValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryName) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	if (itBitMask == m_vBitMask.end())
	{
		return 0xffffffffffffffffll;
	}

	// first looks at the exact std::string match
	for (const auto& mask : itBitMask->second)
	{
		if (mask.second == rEntryName)
		{
			return mask.first;
		}
	}

	// then looks at the caseless std::string match
	std::string l_sEntryNameLower = rEntryName.toASCIIString();
	std::transform(l_sEntryNameLower.begin(), l_sEntryNameLower.end(), l_sEntryNameLower.begin(), ::to_lower<std::string::value_type>);
	for (const auto& mask : itBitMask->second)
	{
		std::string l_sItEntryNameLower = mask.second.toASCIIString();
		std::transform(l_sItEntryNameLower.begin(), l_sItEntryNameLower.end(), l_sItEntryNameLower.begin(), ::to_lower<std::string::value_type>);
		if (l_sItEntryNameLower == l_sEntryNameLower)
		{
			return mask.first;
		}
	}

	// then looks at the std::string being the value itself
	try
	{
		uint64_t l_ui64Value = std::stoll((const char*)rEntryName);

		if (itBitMask->second.find(l_ui64Value) != itBitMask->second.end())
		{
			return l_ui64Value;
		}
	}
	catch (const std::exception&)
	{
		return 0xffffffffffffffffll;
	}

	return 0xffffffffffffffffll;
}

CString CTypeManager::getBitMaskEntryCompositionNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64_t ui64EntryCompositionValue) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	if (itBitMask == m_vBitMask.end())
	{
		return "";
	}

	std::string l_sResult;
	for (uint32_t i = 0; i < 64; i++)
	{
		if (ui64EntryCompositionValue & (1LL << i))
		{
			const auto itBitMaskEntry = itBitMask->second.find(ui64EntryCompositionValue & (1LL << i));
			if (itBitMaskEntry == itBitMask->second.end())
			{
				return "";
			}
			if (l_sResult == "")
			{
				l_sResult = itBitMaskEntry->second.toASCIIString();
			}
			else
			{
				;
				l_sResult += std::string(1, OV_Value_EnumeratedStringSeparator);
				l_sResult += itBitMaskEntry->second.toASCIIString();
			}
		}
	}
	return CString(l_sResult.c_str());
}

uint64_t CTypeManager::getBitMaskEntryCompositionValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryCompositionName) const
{
	std::unique_lock<std::recursive_mutex> lock(m_oMutex);

	const auto itBitMask = m_vBitMask.find(rTypeIdentifier);
	if (itBitMask == m_vBitMask.end()) { return 0; }

	uint64_t l_ui64Result               = 0;
	std::string l_sEntryCompositionName = rEntryCompositionName.toASCIIString();
	std::string::size_type i            = 0;
	std::string::size_type j            = 0;
	do
	{
		i = l_sEntryCompositionName.find(OV_Value_EnumeratedStringSeparator, i);
		if (i == std::string::npos)
		{
			i = l_sEntryCompositionName.length();
		}

		if (i != j)
		{
			std::string l_sEntryName;
			l_sEntryName.assign(l_sEntryCompositionName, j, i - j);

			bool l_bFound = false;
			for (const auto& mask : itBitMask->second)
			{
				if (mask.second == CString(l_sEntryName.c_str()))
				{
					l_ui64Result |= mask.first;
					l_bFound = true;
				}
			}

			if (!l_bFound) { return 0; }
		}

		i++;
		j = i;
	} while (i < l_sEntryCompositionName.length());

	return l_ui64Result;
}

bool CTypeManager::evaluateSettingValue(const CString settingValue, double& numericResult) const
{
	// parse and expression with no variables or functions
	try
	{
		numericResult = Lepton::Parser::parse(settingValue.toASCIIString()).evaluate();
	}
	catch (...) { return false; }
	return true;
}

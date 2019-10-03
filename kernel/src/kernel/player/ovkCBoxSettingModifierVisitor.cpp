#include "ovkCBoxSettingModifierVisitor.h"

#include "ovkCPlayer.h"

#include <xml/IReader.h>
#include <fs/Files.h>
#include "../../tools/ovk_setting_checker.h"

#include <string>
#include <iostream>
#include <fstream>

#define OVD_AttributeId_SettingOverrideFilename             OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)
#define OV_AttributeId_Box_Disabled                         OpenViBE::CIdentifier(0x341D3912, 0x1478DE86)

//___________________________________________________________________//
//                                                                   //

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Kernel;
using namespace Plugins;

void CBoxSettingModifierVisitor::openChild(const char* name, const char** /*sAttributeName*/, const char** /*sAttributeValue*/, uint64_t /*nAttribute*/)
{
	if (!m_bIsParsingSettingOverride) { if (string(name) == string("OpenViBE-SettingsOverride")) { m_bIsParsingSettingOverride = true; } }
	else if (string(name) == string("SettingValue")) { m_bIsParsingSettingValue = true; }
	else { m_bIsParsingSettingValue = false; }
}

void CBoxSettingModifierVisitor::processChildData(const char* sData)
{
	if (m_bIsParsingSettingValue)
	{
		m_pObjectVisitorContext->getLogManager() << LogLevel_Debug << "Using [" << CString(sData) << "] as setting " << m_settingIdx << "...\n";
		m_pBox->setSettingValue(m_settingIdx, sData);
	}
}

void CBoxSettingModifierVisitor::closeChild()
{
	//We need to count it here because we need to take in account the empty value
	if (m_bIsParsingSettingValue) { m_settingIdx++; }
	m_bIsParsingSettingValue = false;
}

bool CBoxSettingModifierVisitor::processBegin(IObjectVisitorContext& rObjectVisitorContext, IBox& box)
{
	m_pObjectVisitorContext = &rObjectVisitorContext;

	// checks if this box should override
	// settings from external file
	if (box.hasAttribute(OVD_AttributeId_SettingOverrideFilename))
	{
		const CString settingOverrideFilename = box.getAttributeValue(OVD_AttributeId_SettingOverrideFilename);
		CString settingOverrideFilenameFinal;
		if (m_configManager == nullptr) { settingOverrideFilenameFinal = rObjectVisitorContext.getConfigurationManager().expand(settingOverrideFilename); }
		else { settingOverrideFilenameFinal = m_configManager->expand(settingOverrideFilename); }

		// message
		rObjectVisitorContext.getLogManager() << LogLevel_Trace << "Trying to override [" << box.getName() << "] box settings with file [" <<
				settingOverrideFilename << " which expands to " << settingOverrideFilenameFinal << "] !\n";

		// creates XML reader
		XML::IReader* l_pReader = createReader(*this);

		// adds new box settings
		m_pBox                      = &box;
		m_settingIdx                = 0;
		m_bIsParsingSettingValue    = false;
		m_bIsParsingSettingOverride = false;

		auto cleanup = [&]()
		{
			// cleans up internal state
			m_pBox                      = nullptr;
			m_settingIdx                = 0;
			m_bIsParsingSettingValue    = false;
			m_bIsParsingSettingOverride = false;

			// releases XML reader
			l_pReader->release();
			l_pReader = nullptr;
		};

		// 1. Open settings file (binary because read would conflict with tellg for text files)
		// 2. Loop until end of file, reading it
		//    and sending what is read to the XML parser
		// 3. Close the settings file
		ifstream file;
		FS::Files::openIFStream(file, settingOverrideFilenameFinal.toASCIIString(), ios::binary);
		if (file.is_open())
		{
			char buffer[1024];
			bool statusOk = true;
			file.seekg(0, ios::end);
			std::streamoff fileLen = file.tellg();
			file.seekg(0, ios::beg);
			while (fileLen && statusOk)
			{
				// File length is always positive so this is safe
				const std::streamoff bufferLen = (unsigned(fileLen) > sizeof(buffer) ? sizeof(buffer) : fileLen);
				file.read(buffer, bufferLen);
				fileLen -= bufferLen;
				statusOk = l_pReader->processData(buffer, bufferLen);
			}
			file.close();

			// message
			if (m_settingIdx == box.getSettingCount())
			{
				rObjectVisitorContext.getLogManager() << LogLevel_Trace << "Overrode " << m_settingIdx << " setting(s) with this configuration file...\n";

				for (uint32_t i = 0; i < m_settingIdx; i++)
				{
					CString settingName     = "";
					CString rawSettingvalue = "";

					box.getSettingName(i, settingName);
					box.getSettingValue(i, rawSettingvalue);
					CString l_sSettingValue = rawSettingvalue;
					l_sSettingValue         = m_configManager->expand(l_sSettingValue);
					CIdentifier settingType;
					box.getSettingType(i, settingType);
					if (!checkSettingValue(l_sSettingValue, settingType, rObjectVisitorContext.getTypeManager()))
					{
						const auto settingTypeName = rObjectVisitorContext.getTypeManager().getTypeName(settingType);
						cleanup();
						OV_ERROR(
							"<" << box.getName() << "> The following value: [" << rawSettingvalue << "] expanded as [" << l_sSettingValue <<
							"] given as setting is not a valid [" << settingTypeName << "] value.",
							ErrorType::BadArgument, false, m_pObjectVisitorContext->getErrorManager(), m_pObjectVisitorContext->getLogManager());
					}
				}
			}
			else
			{
				cleanup();
				OV_ERROR(
					"Overrode " << m_settingIdx << " setting(s) with configuration file [" << settingOverrideFilenameFinal <<
					"]. That does not match the box setting count " << box.getSettingCount(),
					ErrorType::OutOfBound, false, m_pObjectVisitorContext->getErrorManager(), m_pObjectVisitorContext->getLogManager());
			}
		}
		else
		{
			if (box.hasAttribute(OV_AttributeId_Box_Disabled))
			{
				// if the box is disabled do not stop the scenario execution when configuration fails
			}
			else
			{
				cleanup();
				OV_ERROR(
					"Could not override [" << box.getName() << "] settings because configuration file [" << settingOverrideFilenameFinal <<
					"] could not be opened",
					ErrorType::ResourceNotFound, false, m_pObjectVisitorContext->getErrorManager(), m_pObjectVisitorContext->getLogManager());
			}
		}

		cleanup();
	}

	return true;
}

bool CBoxSettingModifierVisitor::processEnd(IObjectVisitorContext& rObjectVisitorContext, IBox& /*box*/)
{
	m_pObjectVisitorContext = &rObjectVisitorContext;
	return true;
}

#include "mCMetaboxLoader.h"

//#define OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario    OpenViBE::CIdentifier(0x29574C87, 0x7BA77780)
//#define OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer OpenViBE::CIdentifier(0x600463A3, 0x474B7F66)

// This definition should be removed. It has been added to make the build process work after the merge.
#define OVP_GD_ClassId_Algorithm_BinaryScenarioHeaderImporter OpenViBE::CIdentifier(0x512C1BEF, 0x4541752C)

#include <ovp_global_defines.h>
#include <iostream>
#include <string>
#include <algorithm>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#include <cstring>
#include <strings.h>
#define _strcmpi strcasecmp
#endif

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace Mensia;

namespace
{
	static const OpenViBE::boolean g_bCanImportBinary = true;
}


FS::boolean CMetaboxLoaderEntryEnumeratorCallBack::callback(
	FS::IEntryEnumerator::IEntry& rEntry,
	FS::IEntryEnumerator::IAttributes& rAttributes)
{
	// extract the filename from the path, without parent or extension
	char l_sBuffer[1024];
	std::string l_sFullFileName(rEntry.getName());
	FS::Files::getParentPath(l_sFullFileName.c_str(), l_sBuffer);

	std::size_t l_uiFilenameStartPos = strlen(l_sBuffer) + 1; // where the parent path ends
	std::size_t l_uiFilenameDotPos = l_sFullFileName.rfind('.');
	std::size_t l_uiFilenameLength = l_uiFilenameDotPos - l_uiFilenameStartPos;

	std::string l_sMetaboxIdentifier = l_sFullFileName.substr(l_uiFilenameStartPos, l_uiFilenameLength);
	std::transform(l_sMetaboxIdentifier.begin(), l_sMetaboxIdentifier.end(), l_sMetaboxIdentifier.begin(), ::tolower);
	std::replace(l_sMetaboxIdentifier.begin(), l_sMetaboxIdentifier.end(), ' ', '_');

	CIdentifier l_oScenarioIdentifier = m_rMetaboxLoader.openScenario(rEntry.getName());

	if (l_oScenarioIdentifier != OV_UndefinedIdentifier)
	{
		IScenario& l_rScenario = m_rMetaboxLoader.getKernelContext().getScenarioManager().getScenario(l_oScenarioIdentifier);

		CBoxAlgorithmMetaboxDesc l_oMetaboxDesc(
		            l_sMetaboxIdentifier.c_str(),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_Name),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_Author),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_Company),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_ShortDescription),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_DetailedDescription),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_Category),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_Version),
		            "gtk-add",
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_AddedSoftwareVersion),
		            l_rScenario.getAttributeValue(OV_AttributeId_Scenario_UpdatedSoftwareVersion)
		            );

		for (uint32 l_ui32ScenarioInputIndex = 0; l_ui32ScenarioInputIndex < l_rScenario.getInputCount(); l_ui32ScenarioInputIndex++)
		{
			CString l_sInputName;
			CIdentifier l_oInputTypeIdentifier;

			l_rScenario.getInputType(l_ui32ScenarioInputIndex, l_oInputTypeIdentifier);
			l_rScenario.getInputName(l_ui32ScenarioInputIndex, l_sInputName);

			l_oMetaboxDesc.addInput(l_sInputName, l_oInputTypeIdentifier);
		}

		for (uint32 l_ui32ScenarioOutputIndex = 0; l_ui32ScenarioOutputIndex < l_rScenario.getOutputCount(); l_ui32ScenarioOutputIndex++)
		{
			CString l_sOutputName;
			CIdentifier l_oOutputTypeIdentifier;

			l_rScenario.getOutputType(l_ui32ScenarioOutputIndex, l_oOutputTypeIdentifier);
			l_rScenario.getOutputName(l_ui32ScenarioOutputIndex, l_sOutputName);

			l_oMetaboxDesc.addOutput(l_sOutputName, l_oOutputTypeIdentifier);
		}

		for (uint32 l_ui32ScenarioSettingIndex = 0; l_ui32ScenarioSettingIndex < l_rScenario.getSettingCount(); l_ui32ScenarioSettingIndex++)
		{
			CString l_sSettingName;
			CIdentifier l_oSettingTypeIdentifier;
			CString l_sSettingDefaultValue;

			l_rScenario.getSettingName(l_ui32ScenarioSettingIndex, l_sSettingName);
			l_rScenario.getSettingType(l_ui32ScenarioSettingIndex, l_oSettingTypeIdentifier);
			l_rScenario.getSettingDefaultValue(l_ui32ScenarioSettingIndex, l_sSettingDefaultValue);

			l_oMetaboxDesc.addSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sSettingDefaultValue);
		}

		CIdentifier l_oMetaboxPrototypeHash = OV_UndefinedIdentifier;

		if (l_rScenario.hasAttribute(OV_AttributeId_Scenario_MetaboxHash))
		{
			l_oMetaboxPrototypeHash.fromString(l_rScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));
		}
		else
		{
			m_rMetaboxLoader.getKernelContext().getLogManager() << LogLevel_Warning << "The metabox " << l_sMetaboxIdentifier.c_str() << " has no Hash in the scenario " << l_sFullFileName.c_str() << "\n";
		}

		m_rMetaboxLoader.closeScenario(l_oScenarioIdentifier);

		std::string l_sVirtualBoxIdentifier = std::string(l_oMetaboxDesc.getCategory().toASCIIString()) + "/" + std::string(l_oMetaboxDesc.getName().toASCIIString());
		m_rMetaboxLoader.getMetaboxInfo(l_sMetaboxIdentifier).assignMetaboxDesc(l_oMetaboxDesc);
		m_rMetaboxLoader.getMetaboxInfo(l_sMetaboxIdentifier).assignVirtualBoxIdentifier(l_sVirtualBoxIdentifier.c_str());
		m_rMetaboxLoader.getMetaboxInfo(l_sMetaboxIdentifier).assignMetaboxScenarioPath(l_sFullFileName.c_str());
		m_rMetaboxLoader.getMetaboxInfo(l_sMetaboxIdentifier).assignMetaboxHash(l_oMetaboxPrototypeHash);

		m_rMetaboxLoader.getPluginObjectDescMap()[l_sVirtualBoxIdentifier] = &(m_rMetaboxLoader.getMetaboxInfo(l_sMetaboxIdentifier).m_oMetaboxDesc);

		// Insert the path to the metabox scenario inside the configuration manager
		std::string l_sConfigurationTokenName = "Metabox_Scenario_Path_For_" + l_sMetaboxIdentifier;
		m_rMetaboxLoader.getKernelContext().getConfigurationManager().createConfigurationToken(l_sConfigurationTokenName.c_str(), l_sFullFileName.c_str());

		l_sConfigurationTokenName = "Metabox_Scenario_Hash_For_" + l_sMetaboxIdentifier;
		m_rMetaboxLoader.getKernelContext().getConfigurationManager().createConfigurationToken(l_sConfigurationTokenName.c_str(), l_oMetaboxPrototypeHash.toString());
		/*
		l_sConfigurationTokenName = "Metabox_Scenario_ManagerId_For_" + l_sMetaboxIdentifier;
		m_rMetaboxLoader.getKernelContext().getConfigurationManager().createConfigurationToken(l_sConfigurationTokenName.c_str(), l_oScenarioIdentifier.toString());
		*/
	}

	return true;
}


// Note: this seems slow but c++11 move semantics will optimize it
std::map< std::string, std::string > CMetaboxLoader::getMetaboxPathMap()
{
	std::map< std::string, std::string > l_vMetaboxPath;

	//for (auto& l_pMetaboxScenarioInfo : m_mMetaboxScenarioInfo)
	for (auto l_itMetaboxScenarioInfo = m_mMetaboxScenarioInfo.cbegin(); l_itMetaboxScenarioInfo != m_mMetaboxScenarioInfo.cend(); l_itMetaboxScenarioInfo++)
	{
		const std::pair<std::string, SMetaboxScenario>& l_pMetaboxScenarioInfo = *l_itMetaboxScenarioInfo;
		l_vMetaboxPath[l_pMetaboxScenarioInfo.first] = l_pMetaboxScenarioInfo.second.m_sMetaboxScenarioPath;
	}

	return l_vMetaboxPath;
}

boolean CMetaboxLoader::loadPluginDescriptorsFromWildcard(const CString& rFileNameWildCard)
{

	m_rKernelContext.getLogManager() << LogLevel_Trace << "Adding plugins from [" << rFileNameWildCard << "]\n";


	CMetaboxLoaderEntryEnumeratorCallBack l_rCB(m_rKernelContext, *this);
	FS::IEntryEnumerator* l_pEntryEnumerator = FS::createEntryEnumerator(l_rCB);

	boolean l_bResult = l_pEntryEnumerator->enumerate(rFileNameWildCard);

	l_pEntryEnumerator->release();

	return l_bResult;
}

CIdentifier CMetaboxLoader::openScenario(const char* sFileName)
{
	CIdentifier l_oScenarioIdentifier;
	if(m_rScenarioManager.createScenario(l_oScenarioIdentifier))
	{
		IScenario& l_rScenario = m_rScenarioManager.getScenario(l_oScenarioIdentifier);

		CMemoryBuffer l_oMemoryBuffer;
//		boolean l_bSuccess = false;

		FILE* l_pFile = FS::Files::open(sFileName, "rb");
		if(l_pFile)
		{
			::fseek(l_pFile, 0, SEEK_END);
			l_oMemoryBuffer.setSize(::ftell(l_pFile), true);
			::fseek(l_pFile, 0, SEEK_SET);
			if(::fread(reinterpret_cast<char*>(l_oMemoryBuffer.getDirectPointer()), (size_t)l_oMemoryBuffer.getSize(), 1, l_pFile)!=1)
			{
				m_rKernelContext.getLogManager() << LogLevel_Error << "Problem reading '" << sFileName << "'\n";
				::fclose(l_pFile);
				m_rScenarioManager.releaseScenario(l_oScenarioIdentifier);
				return OV_UndefinedIdentifier;
			}
			::fclose(l_pFile);
		}
		else
		{
			m_rKernelContext.getLogManager() << LogLevel_Error << "Unable to open '" << sFileName << "' for reading\n";
			m_rScenarioManager.releaseScenario(l_oScenarioIdentifier);
			return OV_UndefinedIdentifier;
		}

		if(l_oMemoryBuffer.getSize())
		{
			boolean l_bImportAsBinary = false;
			CIdentifier l_oImporterIdentifierToUse = OVP_GD_ClassId_Algorithm_XMLScenarioImporter;

			bool l_bIsFileBinary = false;
			l_bIsFileBinary = (::strlen(sFileName) > 4  && (_strcmpi(sFileName+strlen(sFileName)-4, ".mbs") == 0 ||
			                                                _strcmpi(sFileName+strlen(sFileName)-4, ".mbb") == 0));

			if(g_bCanImportBinary && l_bIsFileBinary)
			{
				l_bImportAsBinary = true;
				l_oImporterIdentifierToUse = OVP_GD_ClassId_Algorithm_BinaryScenarioHeaderImporter;
			}

			CIdentifier l_oImporterIdentifier = m_rKernelContext.getAlgorithmManager().createAlgorithm(l_oImporterIdentifierToUse);
			if(l_oImporterIdentifier != OV_UndefinedIdentifier)
			{
				IAlgorithmProxy* l_pImporter = &m_rKernelContext.getAlgorithmManager().getAlgorithm(l_oImporterIdentifier);
				if(l_pImporter)
				{
					m_rKernelContext.getLogManager() << LogLevel_Trace << "Importing metabox '" << sFileName << "' in " << (l_bImportAsBinary ? "Binary" : "XML") << " Format...\n";

					l_pImporter->initialize();

					TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(l_pImporter->getInputParameter(OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
					TParameterHandler < IScenario* > op_pScenario(l_pImporter->getOutputParameter(OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario));

					ip_pMemoryBuffer = &l_oMemoryBuffer;
					op_pScenario = &l_rScenario;

					/* l_bSuccess = */l_pImporter->process();
					l_pImporter->uninitialize();
					m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*l_pImporter);

				}
			}

			return l_oScenarioIdentifier;
		}
		else
		{
			m_rKernelContext.getLogManager() << LogLevel_Warning << "Importing metabox failed...\n";
			m_rScenarioManager.releaseScenario(l_oScenarioIdentifier);
		}
	}
	return OV_UndefinedIdentifier;
}

void CMetaboxLoader::closeScenario(const CIdentifier& rIdentifier)
{
	m_rScenarioManager.releaseScenario(rIdentifier);
}

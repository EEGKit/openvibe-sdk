#include "ovpiCPluginObjectDescEnumBoxTemplateGenerator.h"

#include <system/ovCTime.h>
#include <fs/Files.h>

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

namespace
{
	std::map<int, char> indentCharacters = { { 0, '=' }, { 1, '-' }, { 2, '~' }, { 3, '+' } };

	std::string generateRstTitle(std::string title, int level)
	{
		return title + "\n" + std::string(title.size(), indentCharacters[level]) + "\n";
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

CPluginObjectDescEnumBoxTemplateGenerator::CPluginObjectDescEnumBoxTemplateGenerator(const IKernelContext& rKernelContext, const CString& sDocTemplateDirectory)
	: CPluginObjectDescEnum(rKernelContext), m_DocTemplateDirectory(sDocTemplateDirectory) {}

bool CPluginObjectDescEnumBoxTemplateGenerator::initialize()
{
	if (!m_KernelContext.getScenarioManager().createScenario(m_ScenarioIdentifier)) { return false; }
	m_Scenario = &m_KernelContext.getScenarioManager().getScenario(m_ScenarioIdentifier);
	return true;
}

bool CPluginObjectDescEnumBoxTemplateGenerator::uninitialize()
{
	if (!m_KernelContext.getScenarioManager().releaseScenario(m_ScenarioIdentifier)) { return false; }

	std::ofstream ofBoxIndex;
	FS::Files::openOFStream(ofBoxIndex, (m_DocTemplateDirectory + "/index-boxes.rst").c_str());

	if (!ofBoxIndex.good())
	{
		m_KernelContext.getLogManager() << LogLevel_Error << "Error while trying to open file [" << (m_DocTemplateDirectory + "/index-boxes.rst").c_str() << "]\n";
		return false;
	}

	ofBoxIndex
			<< ".. _Doc_BoxAlgorithms:\n"
			<< "\n"
			<< generateRstTitle("Boxes list", 0)
			<< "\n"
			<< "Available box algorithms are :\n"
			<< "\n";

	ofBoxIndex << generateRstIndex(m_Categories);
	ofBoxIndex << " \n";

	if (!m_DeprecatedBoxesCategories.empty())
	{
		ofBoxIndex << "\n\n"
				<< generateRstTitle("Deprecated boxes list", 0)
				<< "\n"
				<< "The following boxes are deprecated, they are hidden in Studio and will be removed soon or later, so you should consider not using them:\n";

		ofBoxIndex << generateRstIndex(m_DeprecatedBoxesCategories);
		ofBoxIndex << " \n";
	}

	ofBoxIndex << " \n";
	ofBoxIndex.close();
	return true;
}

bool CPluginObjectDescEnumBoxTemplateGenerator::callback(const IPluginObjectDesc& rPluginObjectDesc)
{
	string fileName = "BoxAlgorithm_" + this->transform(rPluginObjectDesc.getName().toASCIIString());
	CIdentifier boxIdentifier;

	if (rPluginObjectDesc.getCreatedClass() == OVP_ClassId_BoxAlgorithm_Metabox)
	{
		// insert a box into the scenario, initialize it from the proxy-descriptor from the metabox loader
		if (!m_Scenario->addBox(boxIdentifier, static_cast<const IBoxAlgorithmDesc&>(rPluginObjectDesc), OV_UndefinedIdentifier))
		{
			m_KernelContext.getLogManager() << LogLevel_Warning << "Skipped [" << CString(fileName.c_str()) << "] (could not create corresponding box)\n";
			return true;
		}
	}
	else if (!m_Scenario->addBox(boxIdentifier, rPluginObjectDesc.getCreatedClassIdentifier(), OV_UndefinedIdentifier))
	{
		m_KernelContext.getLogManager() << LogLevel_Warning << "Skipped [" << CString(fileName.c_str()) << "] (could not create corresponding box)\n";
		return true;
	}


	IBox& box = *m_Scenario->getBoxDetails(boxIdentifier);

	m_KernelContext.getLogManager() << LogLevel_Trace << "Working on [" << CString(fileName.c_str()) << "]\n";

	// --------------------------------------------------------------------------------------------------------------------
	std::ofstream ofBoxTemplate;
	FS::Files::openOFStream(ofBoxTemplate, (m_DocTemplateDirectory + "/Doc_" + fileName + ".rst-template").c_str());

	if (!ofBoxTemplate.good())
	{
		m_KernelContext.getLogManager() << LogLevel_Error << "Error while trying to open file ["
				<< (m_DocTemplateDirectory + "/Doc_" + fileName + ".rst-template").c_str()
				<< "]\n";
		return false;
	}

	ofBoxTemplate
			<< ".. _Doc_" << fileName << ":\n"
			<< "\n"
			<< generateRstTitle(rPluginObjectDesc.getName().toASCIIString(), 0)
			<< "\n"
			<< ".. container:: attribution\n"
			<< "\n"
			<< "   :Author:\n"
			<< "      " << rPluginObjectDesc.getAuthorName().toASCIIString() << "\n"
			<< "   :Company:\n"
			<< "      " << rPluginObjectDesc.getAuthorCompanyName().toASCIIString() << "\n"
			<< "\n"
			<< "\n"
			<< ".. todo::  Write general box description...\n"
			<< "\n"
			<< "\n";


	if (box.getInputCount())
	{
		ofBoxTemplate
				<< ".. _Doc_" << fileName.c_str() << "_Inputs:\n"
				<< "\n"
				<< generateRstTitle("Inputs", 1).c_str()
				<< ".. todo::  Write general input description...\n"
				<< "\n"
				<< ".. csv-table::\n"
				<< "   :header: \"Input Name\", \"Stream Type\"\n"
				<< "\n";

		std::vector<CString> inputNames(box.getInputCount());
		for (uint32_t i = 0; i < box.getInputCount(); i++)
		{
			CIdentifier typeIdentifier;
			box.getInputName(i, inputNames[i]);
			box.getInputType(i, typeIdentifier);
			CString typeName = m_KernelContext.getTypeManager().getTypeName(typeIdentifier);

			ofBoxTemplate
					<< "   \"" << inputNames[i] << "\", \"" << typeName << "\"\n";
		}
		uint32_t index = 1;
		for (CString inputName : inputNames)
		{
			ofBoxTemplate
					<< "\n"
					<< ".. _Doc_" << fileName << "_Input_" << index << ":\n"
					<< "\n"
					<< generateRstTitle(inputName.toASCIIString(), 2)
					<< "\n"
					<< ".. todo::  Write input description...\n"
					<< "\n"
					<< "\n";
			index++;
		}
	}

	if (box.getOutputCount())
	{
		ofBoxTemplate
				<< ".. _Doc_" << fileName << "_Outputs:\n"
				<< "\n"
				<< generateRstTitle("Outputs", 1)
				<< "\n"
				<< ".. todo::  Write general output description...\n"
				<< "\n"
				<< ".. csv-table::\n"
				<< "   :header: \"Output Name\", \"Stream Type\"\n"
				<< "\n";

		std::vector<CString> outputNames(box.getOutputCount());
		for (uint32_t i = 0; i < box.getOutputCount(); i++)
		{
			CIdentifier typeIdentifier;
			box.getOutputName(i, outputNames[i]);
			box.getOutputType(i, typeIdentifier);
			CString typeName = m_KernelContext.getTypeManager().getTypeName(typeIdentifier);

			ofBoxTemplate
					<< "   \"" << outputNames[i] << "\", \"" << typeName << "\"\n";
		}
		uint32_t index = 1;
		for (CString outputName : outputNames)
		{
			ofBoxTemplate
					<< "\n"
					<< ".. _Doc_" << fileName << "_Output_" << index << ":\n"
					<< "\n"
					<< generateRstTitle(outputName.toASCIIString(), 2)
					<< "\n"
					<< ".. todo::  Write output description...\n"
					<< "\n"
					<< "\n";
			index++;
		}
	}

	if (box.getSettingCount())
	{
		ofBoxTemplate
				<< ".. _Doc_" << fileName.c_str() << "_Settings:\n"
				<< "\n"
				<< generateRstTitle("Settings", 1)
				<< "\n"
				<< ".. todo::  Write settings general description...\n"
				<< "\n"
				<< ".. csv-table::\n"
				<< "   :header: \"Setting Name\", \"Type\", \"Default Value\"\n"
				<< "\n";

		std::vector<CString> settingsNames(box.getSettingCount());
		for (uint32_t i = 0; i < box.getSettingCount(); i++)
		{
			CIdentifier typeIdentifier;
			CString defaultValue;
			box.getSettingName(i, settingsNames[i]);
			box.getSettingType(i, typeIdentifier);
			box.getSettingDefaultValue(i, defaultValue);
			CString typeName = m_KernelContext.getTypeManager().getTypeName(typeIdentifier);

			ofBoxTemplate
					<< "   \"" << settingsNames[i] << "\", \"" << typeName << "\", \"" << defaultValue << "\"\n";
		}
		uint32_t index = 1;
		for (CString settingName : settingsNames)
		{
			ofBoxTemplate
					<< "\n"
					<< ".. _Doc_" << fileName << "_Setting_" << index << ":\n"
					<< "\n"
					<< generateRstTitle(settingName.toASCIIString(), 2)
					<< "\n"
					<< ".. todo:: Write setting description... \n"
					<< "\n"
					<< "\n";
			index++;
		}
	}

	ofBoxTemplate
			<< ".. _Doc_" << fileName << "_Examples:\n"
			<< "\n"
			<< generateRstTitle("Examples", 1)
			<< "\n"
			<< ".. todo::  Write example of use...\n"
			<< "\n"
			<< "\n";

	ofBoxTemplate
			<< ".. _Doc_" << fileName << "_Miscellaneous:\n"
			<< "\n"
			<< generateRstTitle("Miscellaneous", 1)
			<< "\n"
			<< ".. todo::  Write any miscellaneous information...\n"
			<< "\n"
			<< "\n";

	ofBoxTemplate.close();

	// m_Categories is used to generate the list of boxes. Documentation for deprecated boxes
	// should remain available if needed but not be listed
	if (m_KernelContext.getPluginManager().isPluginObjectFlaggedAsDeprecated(box.getAlgorithmClassIdentifier()))
	{
		m_DeprecatedBoxesCategories.push_back(pair<string, string>(rPluginObjectDesc.getCategory().toASCIIString(), rPluginObjectDesc.getName().toASCIIString()));
	}
	else
	{
		m_Categories.push_back(pair<string, string>(rPluginObjectDesc.getCategory().toASCIIString(), rPluginObjectDesc.getName().toASCIIString()));
	}

	return true;
}

string CPluginObjectDescEnumBoxTemplateGenerator::generateRstIndex(std::vector<std::pair<std::string, std::string>> categories)
{
	string res = "";

	string lastCategoryName;
	vector<string> lastSplittedCategories;
	std::sort(categories.begin(), categories.end());

	for (auto& category : categories)
	{
		string categoryName = category.first;
		string name         = category.second;

		if (lastCategoryName != categoryName)
		{
			vector<string>::iterator itLastSplittedCategory;
			vector<string>::iterator itSplittedCategory1;
			vector<string>::iterator itSplittedCategory2;
			vector<string> splittedCategories;
			size_t i        = (size_t)-1;
			bool isFinished = false;
			while (!isFinished)
			{
				size_t j = categoryName.find('/', i + 1);
				if (j == string::npos)
				{
					j          = categoryName.length();
					isFinished = true;
				}
				if (j != i + 1)
				{
					splittedCategories.push_back(categoryName.substr(i + 1, j - i - 1));
					i = j;
				}
			}

			for (itLastSplittedCategory = lastSplittedCategories.begin(), itSplittedCategory1 = splittedCategories.begin();
				 itLastSplittedCategory != lastSplittedCategories.end() && itSplittedCategory1 != splittedCategories.end() && *itLastSplittedCategory == *itSplittedCategory1;
				 ++itLastSplittedCategory, ++itSplittedCategory1) { }

			for (; itSplittedCategory1 != splittedCategories.end(); ++itSplittedCategory1)
			{
				size_t level = 1;
				for (itSplittedCategory2 = splittedCategories.begin(); itSplittedCategory2 != itSplittedCategory1; ++itSplittedCategory2)
				{
					level++;
				}
				res += "\n\n"
						+ generateRstTitle(*itSplittedCategory1, level)
						+ "\n"
						+ ".. toctree::\n"
						+ "   :maxdepth: 1\n"
						+ "\n";
			}

			lastCategoryName       = categoryName;
			lastSplittedCategories = splittedCategories;
		}

		res += "   Doc_BoxAlgorithm_" + this->transform(name) + "\n";
	}
	return res;
}

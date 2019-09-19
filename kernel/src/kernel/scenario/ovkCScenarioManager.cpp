#include "ovkCScenarioManager.h"
#include "ovkCScenario.h"

#include <cstdlib>
#include <fs/Files.h>
#include <cassert>
#include <algorithm>
#include <openvibe/kernel/scenario/ovIAlgorithmScenarioImporter.h>
#include <openvibe/kernel/scenario/ovIAlgorithmScenarioExporter.h>
#include "../../tools/ovkSBoxProto.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

CScenarioManager::CScenarioManager(const IKernelContext& ctx)
	: TKernelObject<IScenarioManager>(ctx) {}

CScenarioManager::~CScenarioManager()
{
	for (auto i = m_vScenario.begin(); i != m_vScenario.end(); ++i) { delete i->second; }
}

void CScenarioManager::cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager)
{
	CIdentifier importContextId = OV_UndefinedIdentifier;
	// Copy the registered importers from the parent Scenario Manager
	while ((importContextId = scenarioManager.getNextScenarioImportContext(importContextId)) != OV_UndefinedIdentifier)
	{
		CString fileNameExtension = "";
		while ((fileNameExtension = scenarioManager.getNextScenarioImporter(importContextId, fileNameExtension)) != CString(""))
		{
			CIdentifier algorithmId = scenarioManager.getScenarioImporterAlgorithmIdentifier(importContextId, fileNameExtension);

			this->registerScenarioImporter(importContextId, fileNameExtension, algorithmId);
		}
	}

	CIdentifier exportContextId = OV_UndefinedIdentifier;
	while ((exportContextId = scenarioManager.getNextScenarioExportContext(exportContextId)) != OV_UndefinedIdentifier)
	{
		CString fileNameExtension = "";
		while ((fileNameExtension = scenarioManager.getNextScenarioExporter(exportContextId, fileNameExtension)) != CString(""))
		{
			CIdentifier algorithmId = scenarioManager.getScenarioExporterAlgorithmIdentifier(exportContextId, fileNameExtension);

			this->registerScenarioExporter(exportContextId, fileNameExtension, algorithmId);
		}
	}
}

CIdentifier CScenarioManager::getNextScenarioIdentifier(const CIdentifier& previousID) const
{
	map<CIdentifier, CScenario*>::const_iterator itScenario;

	if (previousID == OV_UndefinedIdentifier) { itScenario = m_vScenario.begin(); }
	else
	{
		itScenario = m_vScenario.find(previousID);
		if (itScenario == m_vScenario.end()) { return OV_UndefinedIdentifier; }
		++itScenario;
	}

	return itScenario != m_vScenario.end() ? itScenario->first : OV_UndefinedIdentifier;
}

bool CScenarioManager::isScenario(const CIdentifier& scenarioID) const { return m_vScenario.find(scenarioID) != m_vScenario.end(); }

bool CScenarioManager::createScenario(CIdentifier& scenarioID)
{
	//create scenario object
	scenarioID              = getUnusedIdentifier();
	CScenario* l_pScenario  = new CScenario(getKernelContext(), scenarioID);
	m_vScenario[scenarioID] = l_pScenario;

	return true;
}

bool CScenarioManager::importScenario(CIdentifier& newScenarioIdentifier, const IMemoryBuffer& inputMemoryBuffer,
									  const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	newScenarioIdentifier = OV_UndefinedIdentifier;

	OV_ERROR_UNLESS_KRF(this->createScenario(newScenarioIdentifier), "Error creating new scenario", ErrorType::BadResourceCreation);

	auto releaseScenario = [&]()
	{
		// use a fatal here because a release failure while creation succeeded
		// means we are in an unexpected state
		OV_FATAL_UNLESS_K(this->releaseScenario(newScenarioIdentifier), "Releasing just created scenario failed for " << newScenarioIdentifier.toString(),
						  ErrorType::Internal);
		newScenarioIdentifier = OV_UndefinedIdentifier;
	};

	IScenario& newScenarioInstance = this->getScenario(newScenarioIdentifier);

	if (!inputMemoryBuffer.getSize())
	{
		releaseScenario();
		OV_ERROR_KRF("Buffer containing scenario data is empty", ErrorType::BadValue);
	}

	CIdentifier importerInstanceIdentifier = this->getKernelContext().getAlgorithmManager().createAlgorithm(scenarioImporterAlgorithmIdentifier);

	if (importerInstanceIdentifier == OV_UndefinedIdentifier)
	{
		releaseScenario();
		OV_ERROR_KRF("Can not create the requested scenario importer", ErrorType::BadResourceCreation);
	}

	IAlgorithmProxy* importer = &this->getKernelContext().getAlgorithmManager().getAlgorithm(importerInstanceIdentifier);

	OV_FATAL_UNLESS_K(
		importer,
		"Importer with id " << importerInstanceIdentifier.toString() << " not found although it has just been created",
		ErrorType::ResourceNotFound);

	auto releaseAlgorithm = [&]()
	{
		// use a fatal here because a release failure while creation succeeded
		// means we are in an unexpected state
		OV_FATAL_UNLESS_K(
			this->getKernelContext().getAlgorithmManager().releaseAlgorithm(*importer),
			"Releasing just created algorithm failed for " << importerInstanceIdentifier.toString(),
			ErrorType::Internal);
	};

	if (!importer->initialize())
	{
		releaseScenario();
		releaseAlgorithm();
		OV_ERROR_KRF("Can not initialize the requested scenario importer", ErrorType::Internal);
	}

	IParameter* memoryBufferParameter = importer->getInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer);
	IParameter* scenarioParameter     = importer->getOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario);

	if (!(memoryBufferParameter && scenarioParameter))
	{
		releaseScenario();
		releaseAlgorithm();

		OV_ERROR_UNLESS_KRF(
			memoryBufferParameter,
			"The requested importer does not have a MemoryBuffer input parameter with identifier " <<
			OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer.toString(),
			ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(
			scenarioParameter,
			"The requested importer does not have a Scenario output parameter with identifier " << OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario.
			toString(),
			ErrorType::BadOutput);
	}


	TParameterHandler<const IMemoryBuffer*> memoryBufferParameterHandler(memoryBufferParameter);
	TParameterHandler<IScenario*> scenarioParameterHandler(scenarioParameter);

	memoryBufferParameterHandler = &inputMemoryBuffer;
	scenarioParameterHandler     = &newScenarioInstance;

	if (!importer->process())
	{
		releaseScenario();
		releaseAlgorithm();
		OV_ERROR_KRF("Can not process data using the requested scenario importer", ErrorType::Internal);
	}

	if (!importer->uninitialize())
	{
		releaseScenario();
		releaseAlgorithm();
		OV_ERROR_KRF("Can not uninitialize the requested scenario importer", ErrorType::Internal);
	}

	releaseAlgorithm();

	return true;
}

bool CScenarioManager::importScenarioFromFile(CIdentifier& newScenarioIdentifier, const CString& fileName,
											  const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	newScenarioIdentifier = OV_UndefinedIdentifier;

	CMemoryBuffer memoryBuffer;

	FILE* inputFile = FS::Files::open(fileName, "rb");

	OV_ERROR_UNLESS_KRF(
		inputFile,
		"Can not open scenario file '" << fileName << "'",
		ErrorType::BadFileRead);

	fseek(inputFile, 0, SEEK_END);
	memoryBuffer.setSize(size_t(ftell(inputFile)), true);
	fseek(inputFile, 0, SEEK_SET);

	if (fread(reinterpret_cast<char*>(memoryBuffer.getDirectPointer()), size_t(memoryBuffer.getSize()), 1, inputFile) != 1)
	{
		fclose(inputFile);
		OV_ERROR_KRF("Problem reading scenario file '" << fileName << "'", ErrorType::BadFileRead);
	}
	fclose(inputFile);

	return this->importScenario(newScenarioIdentifier, memoryBuffer, scenarioImporterAlgorithmIdentifier);
}

bool CScenarioManager::importScenarioFromFile(CIdentifier& newScenarioIdentifier, const CIdentifier& importContext, const CString& fileName)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters.count(importContext),
						"The import context " << importContext.toString() << " has no associated importers",
						ErrorType::Internal);
	std::vector<char> fileNameExtension;
	fileNameExtension.resize(fileName.length() + 1);
	FS::Files::getFilenameExtension(fileName.toASCIIString(), &fileNameExtension[0]);
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters[importContext].count(&fileNameExtension[0]),
						"The import context " << importContext.toString() << " has no associated importers for extension [" << &fileNameExtension[0] << "]",
						ErrorType::Internal);
	return this->importScenarioFromFile(newScenarioIdentifier, fileName, m_ScenarioImporters[importContext][&fileNameExtension[0]]);
}

bool CScenarioManager::registerScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension,
												const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	if (!m_ScenarioImporters.count(importContext)) { m_ScenarioImporters[importContext] = std::map<std::string, CIdentifier>(); }

	OV_ERROR_UNLESS_KRF(!m_ScenarioImporters[importContext].count(fileNameExtension.toASCIIString()),
						"The file name extension [" << fileNameExtension << "] already has an importer registered for context " << importContext.toString(),
						ErrorType::Internal);

	m_ScenarioImporters[importContext][fileNameExtension.toASCIIString()] = scenarioImporterAlgorithmIdentifier;

	return true;
}

bool CScenarioManager::unregisterScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters.count(importContext),
						"The import context " << importContext.toString() << " has no associated importers",
						ErrorType::Internal);
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters[importContext].count(fileNameExtension.toASCIIString()),
						"The import context " << importContext.toString() << " has no associated importers for extension [" << fileNameExtension << "]",
						ErrorType::Internal);

	auto& contextImporters = m_ScenarioImporters[importContext];

	for (auto it = contextImporters.begin(); it != contextImporters.end();)
	{
		if (it->first == fileNameExtension.toASCIIString()) { it = contextImporters.erase(it); }
		else { ++it; }
	}
	for (auto it = m_ScenarioImporters.begin(); it != m_ScenarioImporters.end();)
	{
		if (it->second.empty()) { it = m_ScenarioImporters.erase(it); }
		else { ++it; }
	}
	return true;
}

CIdentifier CScenarioManager::getNextScenarioImportContext(const CIdentifier& importContext) const
{
	if (m_ScenarioImporters.empty()) { return OV_UndefinedIdentifier; }

	if (importContext == OV_UndefinedIdentifier) { return m_ScenarioImporters.cbegin()->first; }

	auto current = m_ScenarioImporters.find(importContext);
	if (current == m_ScenarioImporters.end() || ++current == m_ScenarioImporters.end()) { return OV_UndefinedIdentifier; }

	return current->first;
}

CString CScenarioManager::getNextScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension) const
{
	if (m_ScenarioImporters.empty() || !m_ScenarioImporters.count(importContext)) { return ""; }

	const auto& scenarioImportContextMap = m_ScenarioImporters.at(importContext);

	if (fileNameExtension == CString("")) { return scenarioImportContextMap.cbegin()->first.c_str(); }

	auto current = scenarioImportContextMap.find(fileNameExtension.toASCIIString());
	if (current == scenarioImportContextMap.end() || ++current == scenarioImportContextMap.end()) { return ""; }

	return current->first.c_str();
}

CIdentifier CScenarioManager::getScenarioImporterAlgorithmIdentifier(const CIdentifier& importContext, const CString& fileNameExtension) const
{
	OV_ERROR_UNLESS_KRU(
		!m_ScenarioImporters.empty() && m_ScenarioImporters.count(importContext) && m_ScenarioImporters.at(importContext).count(fileNameExtension.toASCIIString(
		)),
		"Scenario importer not found",
		ErrorType::OutOfBound);

	return m_ScenarioImporters.at(importContext).at(fileNameExtension.toASCIIString());
}

bool CScenarioManager::exportScenario(IMemoryBuffer& outputMemoryBuffer, const CIdentifier& scenarioID,
									  const CIdentifier& scenarioExporterAlgorithmIdentifier) const
{
	OV_ERROR_UNLESS_KRF(
		m_vScenario.find(scenarioID) != m_vScenario.end(),
		"Scenario with identifier " << scenarioID.toString() << " does not exist.",
		ErrorType::ResourceNotFound);

	// If the scenario is a metabox, we will save its prototype hash into an attribute of the scenario
	// that way the standalone scheduler can check whether metaboxes included inside need updating.
	IScenario& scenario = this->getScenario(scenarioID);


	if (scenario.isMetabox())
	{
		SBoxProto l_oMetaboxProto(getKernelContext().getTypeManager());

		for (uint32_t l_ui32ScenarioInputIndex = 0; l_ui32ScenarioInputIndex < scenario.getInputCount(); l_ui32ScenarioInputIndex++)
		{
			CIdentifier l_oInputputIdentifier;
			CString inputName;
			CIdentifier inputTypeID;

			scenario.getInterfacorIdentifier(Input, l_ui32ScenarioInputIndex, l_oInputputIdentifier);
			scenario.getInputType(l_ui32ScenarioInputIndex, inputTypeID);
			scenario.getInputName(l_ui32ScenarioInputIndex, inputName);

			l_oMetaboxProto.addInput(inputName, inputTypeID, l_oInputputIdentifier, true);
		}

		for (uint32_t l_ui32ScenarioOutputIndex = 0; l_ui32ScenarioOutputIndex < scenario.getOutputCount(); l_ui32ScenarioOutputIndex++)
		{
			CIdentifier OutputID;
			CString outputName;
			CIdentifier OutputTypeID;

			scenario.getInterfacorIdentifier(Output, l_ui32ScenarioOutputIndex, OutputID);
			scenario.getOutputType(l_ui32ScenarioOutputIndex, OutputTypeID);
			scenario.getOutputName(l_ui32ScenarioOutputIndex, outputName);

			l_oMetaboxProto.addOutput(outputName, OutputTypeID, OutputID, true);
		}

		for (uint32_t l_ui32ScenarioSettingIndex = 0; l_ui32ScenarioSettingIndex < scenario.getSettingCount(); l_ui32ScenarioSettingIndex++)
		{
			CString l_sSettingName;
			CIdentifier l_oSettingTypeIdentifier;
			CString l_sSettingDefaultValue;
			CIdentifier l_oSettingIdentifier;

			scenario.getSettingName(l_ui32ScenarioSettingIndex, l_sSettingName);
			scenario.getSettingType(l_ui32ScenarioSettingIndex, l_oSettingTypeIdentifier);
			scenario.getSettingDefaultValue(l_ui32ScenarioSettingIndex, l_sSettingDefaultValue);
			scenario.getInterfacorIdentifier(Setting, l_ui32ScenarioSettingIndex, l_oSettingIdentifier);

			l_oMetaboxProto.addSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sSettingDefaultValue, false, l_oSettingIdentifier, true);
		}

		if (scenario.hasAttribute(OV_AttributeId_Scenario_MetaboxHash))
		{
			scenario.setAttributeValue(OV_AttributeId_Scenario_MetaboxHash, l_oMetaboxProto.m_oHash.toString());
		}
		else { scenario.addAttribute(OV_AttributeId_Scenario_MetaboxHash, l_oMetaboxProto.m_oHash.toString()); }
	}

	CIdentifier exporterInstanceIdentifier = this->getKernelContext().getAlgorithmManager().createAlgorithm(scenarioExporterAlgorithmIdentifier);

	OV_ERROR_UNLESS_KRF(
		exporterInstanceIdentifier != OV_UndefinedIdentifier,
		"Can not create the requested scenario exporter",
		ErrorType::BadResourceCreation);

	IAlgorithmProxy* exporter = &this->getKernelContext().getAlgorithmManager().getAlgorithm(exporterInstanceIdentifier);

	OV_FATAL_UNLESS_K(
		exporter,
		"Exporter with id " << exporterInstanceIdentifier.toString() << " not found although it has just been created",
		ErrorType::ResourceNotFound);

	auto releaseAlgorithm = [&]()
	{
		// use a fatal here because a release failure while creation succeeded
		// means we are in an unexpected state
		OV_FATAL_UNLESS_K(
			this->getKernelContext().getAlgorithmManager().releaseAlgorithm(*exporter),
			"Releasing just created algorithm failed for " << exporterInstanceIdentifier.toString(),
			ErrorType::Internal);
	};

	if (!exporter->initialize())
	{
		releaseAlgorithm();
		OV_ERROR_KRF("Can not initialize the requested scenario exporter", ErrorType::Internal);
	}

	IParameter* scenarioParameter     = exporter->getInputParameter(OV_Algorithm_ScenarioExporter_InputParameterId_Scenario);
	IParameter* memoryBufferParameter = exporter->getOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer);

	if (!(memoryBufferParameter && scenarioParameter))
	{
		releaseAlgorithm();

		OV_ERROR_UNLESS_KRF(
			scenarioParameter,
			"The requested exporter does not have a Scenario input parameter with identifier " << OV_Algorithm_ScenarioExporter_InputParameterId_Scenario.
			toString(),
			ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(
			memoryBufferParameter,
			"The requested exporter does not have a MemoryBuffer output parameter with identifier " <<
			OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer.toString(),
			ErrorType::BadOutput);
	}

	TParameterHandler<IScenario*> scenarioParameterHandler(scenarioParameter);
	TParameterHandler<const IMemoryBuffer*> memoryBufferParameterHandler(memoryBufferParameter);

	scenarioParameterHandler     = &scenario;
	memoryBufferParameterHandler = &outputMemoryBuffer;

	if (!exporter->process())
	{
		releaseAlgorithm();
		OV_ERROR_KRF("Can not process data using the requested scenario exporter", ErrorType::Internal);
	}

	if (!exporter->uninitialize())
	{
		releaseAlgorithm();
		OV_ERROR_KRF("Can not uninitialize the requested scenario exporter", ErrorType::Internal);
	}

	releaseAlgorithm();
	return true;
}

bool CScenarioManager::exportScenarioToFile(const CString& fileName, const CIdentifier& scenarioID,
											const CIdentifier& scenarioExporterAlgorithmIdentifier) const
{
	IScenario& scenario = this->getScenario(scenarioID);
	if (scenario.containsBoxWithDeprecatedInterfacors())
	{
		OV_WARNING_K(
			"Cannot export a scenario with pending deprecated I/O or Settings. Please remove them using the Designer."
		);
		return false;
	}

	CMemoryBuffer memoryBuffer;
	this->exportScenario(memoryBuffer, scenarioID, scenarioExporterAlgorithmIdentifier);

	std::ofstream outputFileStream;
	FS::Files::openOFStream(outputFileStream, fileName, ios::binary);

	OV_ERROR_UNLESS_KRF(
		outputFileStream.good(),
		"Failed to open file " << fileName,
		ErrorType::BadFileRead);

	outputFileStream.write(reinterpret_cast<const char*>(memoryBuffer.getDirectPointer()), long(memoryBuffer.getSize()));
	outputFileStream.close();

	return true;
}

bool CScenarioManager::exportScenarioToFile(const CIdentifier& exportContext, const CString& fileName, const CIdentifier& scenarioID)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters.count(exportContext),
						"The export context " << exportContext.toString() << " has no associated exporters",
						ErrorType::Internal);
	std::vector<char> fileNameExtension;
	fileNameExtension.resize(fileName.length() + 1);
	FS::Files::getFilenameExtension(fileName.toASCIIString(), &fileNameExtension[0]);
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters[exportContext].count(&fileNameExtension[0]),
						"The export context " << exportContext.toString() << " has no associated exporters for extension [" << &fileNameExtension[0] << "]",
						ErrorType::Internal);
	return this->exportScenarioToFile(fileName, scenarioID, m_ScenarioExporters[exportContext][&fileNameExtension[0]]);
}

bool CScenarioManager::registerScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension,
												const CIdentifier& scenarioExporterAlgorithmIdentifier)
{
	if (!m_ScenarioExporters.count(exportContext)) { m_ScenarioExporters[exportContext] = std::map<std::string, CIdentifier>(); }

	OV_ERROR_UNLESS_KRF(!m_ScenarioExporters[exportContext].count(fileNameExtension.toASCIIString()),
						"The file name extension [" << fileNameExtension << "] already has an exporter registered for context " << exportContext.toString(),
						ErrorType::Internal);

	m_ScenarioExporters[exportContext][fileNameExtension.toASCIIString()] = scenarioExporterAlgorithmIdentifier;

	return true;
}

bool CScenarioManager::unregisterScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters.count(exportContext),
						"The export context " << exportContext.toString() << " has no associated exporters",
						ErrorType::Internal);
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters[exportContext].count(fileNameExtension.toASCIIString()),
						"The export context " << exportContext.toString() << " has no associated exporters for extension [" << fileNameExtension << "]",
						ErrorType::Internal);

	auto& contextExporters = m_ScenarioExporters[exportContext];

	for (auto it = contextExporters.begin(); it != contextExporters.end();)
	{
		if (it->first.c_str() == fileNameExtension.toASCIIString()) { it = contextExporters.erase(it); }
		else { ++it; }
	}
	for (auto it = m_ScenarioExporters.begin(); it != m_ScenarioExporters.end();)
	{
		if (it->second.empty()) { it = m_ScenarioExporters.erase(it); }
		else { ++it; }
	}
	return true;
}

CIdentifier CScenarioManager::getNextScenarioExportContext(const CIdentifier& exportContext) const
{
	if (m_ScenarioExporters.empty()) { return OV_UndefinedIdentifier; }

	if (exportContext == OV_UndefinedIdentifier) { return m_ScenarioExporters.cbegin()->first; }

	auto current = m_ScenarioExporters.find(exportContext);
	if (current == m_ScenarioExporters.end() || ++current == m_ScenarioExporters.end()) { return OV_UndefinedIdentifier; }

	return current->first;
}

CString CScenarioManager::getNextScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension) const
{
	if (m_ScenarioExporters.empty() || !m_ScenarioExporters.count(exportContext)) { return ""; }

	const auto& scenarioExportContextMap = m_ScenarioExporters.at(exportContext);

	if (fileNameExtension == CString("")) { return scenarioExportContextMap.cbegin()->first.c_str(); }

	auto current = scenarioExportContextMap.find(fileNameExtension.toASCIIString());
	if (current == scenarioExportContextMap.end() || ++current == scenarioExportContextMap.end()) { return ""; }

	return current->first.c_str();
}

CIdentifier CScenarioManager::getScenarioExporterAlgorithmIdentifier(const CIdentifier& exportContext, const CString& fileNameExtension) const
{
	OV_ERROR_UNLESS_KRU(
		!m_ScenarioExporters.empty() && m_ScenarioExporters.count(exportContext) && m_ScenarioExporters.at(exportContext).count(fileNameExtension.toASCIIString(
		)),
		"Scenario importer not found",
		ErrorType::OutOfBound);

	return m_ScenarioExporters.at(exportContext).at(fileNameExtension.toASCIIString());
}

bool CScenarioManager::releaseScenario(const CIdentifier& scenarioID)
{
	auto itScenario = m_vScenario.find(scenarioID);
	if (itScenario == m_vScenario.end())
	{
		// error is handled on a higher level
		return false;
	}

	CScenario* l_pScenario = itScenario->second;
	delete l_pScenario;
	m_vScenario.erase(itScenario);

	return true;
}

IScenario& CScenarioManager::getScenario(const CIdentifier& scenarioID)
{
	auto itScenario = m_vScenario.find(scenarioID);

	// If the call is wrongly handled, and falls in this condition then next instruction causes a crash...
	// At least, here the abortion is handled!
	OV_FATAL_UNLESS_K(itScenario != m_vScenario.end(), "Scenario " << scenarioID.toString() << " does not exist !", ErrorType::ResourceNotFound);

	return *itScenario->second;
}

IScenario& CScenarioManager::getScenario(const CIdentifier& scenarioID) const { return const_cast<CScenarioManager*>(this)->getScenario(scenarioID); }

CIdentifier CScenarioManager::getUnusedIdentifier() const
{
	uint64_t l_ui64Identifier = (uint64_t(rand()) << 32) + uint64_t(rand());
	CIdentifier l_oResult;
	map<CIdentifier, CScenario*>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult = CIdentifier(l_ui64Identifier);
		i         = m_vScenario.find(l_oResult);
	} while (i != m_vScenario.end() || l_oResult == OV_UndefinedIdentifier);
	return l_oResult;
}

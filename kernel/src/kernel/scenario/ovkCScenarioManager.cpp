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
using namespace OpenViBE::Kernel;
using namespace std;

CScenarioManager::CScenarioManager(const IKernelContext& rKernelContext)
	:TKernelObject<IScenarioManager>(rKernelContext)
{
}

CScenarioManager::~CScenarioManager(void)
{
	for(map<CIdentifier, CScenario*>::iterator i=m_vScenario.begin(); i!=m_vScenario.end(); ++i)
	{
		delete i->second;
	}
}

bool CScenarioManager::cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager)
{
	// Copy the registered importers from the parent Scenario Manager
	for (uint32 contextIndex = 0; contextIndex < scenarioManager.getRegisteredScenarioImportContextsCount(); ++contextIndex)
	{
		CIdentifier importContextId;
		OV_FATAL_UNLESS_K(scenarioManager.getRegisteredScenarioImportContextDetails(contextIndex, importContextId),
		                  "Import context not found", ErrorType::Internal);

		for (uint32 importerIndex = 0; importerIndex < scenarioManager.getRegisteredScenarioImportersCount(importContextId); ++importerIndex)
		{
			const char* fileNameExtension;
			CIdentifier algorithmId;

			OV_FATAL_UNLESS_K(scenarioManager.getRegisteredScenarioImporterDetails(importContextId, importerIndex, &fileNameExtension, algorithmId),
			                  "Importer not found", ErrorType::Internal);

			this->registerScenarioImporter(importContextId, fileNameExtension, algorithmId);

		}
	}

	return true;
}

CIdentifier CScenarioManager::getNextScenarioIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	map<CIdentifier, CScenario*>::const_iterator itScenario;

	if(rPreviousIdentifier==OV_UndefinedIdentifier)
	{
		itScenario=m_vScenario.begin();
	}
	else
	{
		itScenario=m_vScenario.find(rPreviousIdentifier);
		if(itScenario==m_vScenario.end())
		{
			return OV_UndefinedIdentifier;
		}
		++itScenario;
	}

	return itScenario!=m_vScenario.end()?itScenario->first:OV_UndefinedIdentifier;
}

bool CScenarioManager::isScenario(const OpenViBE::CIdentifier& scenarioIdentifier) const
{
	return m_vScenario.find(scenarioIdentifier) != m_vScenario.end();
}

bool CScenarioManager::createScenario(
	CIdentifier& rScenarioIdentifier)
{
	//create scenario object
	rScenarioIdentifier=getUnusedIdentifier();
	CScenario* l_pScenario=new CScenario(getKernelContext(), rScenarioIdentifier);
	m_vScenario[rScenarioIdentifier]=l_pScenario;

	return true;
}

bool CScenarioManager::importScenario(OpenViBE::CIdentifier& newScenarioIdentifier, const IMemoryBuffer& inputMemoryBuffer, const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	newScenarioIdentifier = OV_UndefinedIdentifier;

	OV_ERROR_UNLESS_KRF(
		this->createScenario(newScenarioIdentifier),
		"Error creating new scenario",
		ErrorType::BadResourceCreation
	);

	auto releaseScenario = [&](){
		// use a fatal here because a release failure while creation succeeded
		// means we are in an unexpected state
		OV_FATAL_UNLESS_K(
			this->releaseScenario(newScenarioIdentifier),
			"Releasing just created scenario failed for " << newScenarioIdentifier.toString(),
			ErrorType::Internal
		);
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
		ErrorType::ResourceNotFound
	);

	auto releaseAlgorithm = [&](){
		// use a fatal here because a release failure while creation succeeded
		// means we are in an unexpected state
		OV_FATAL_UNLESS_K(
			this->getKernelContext().getAlgorithmManager().releaseAlgorithm(*importer),
			"Releasing just created algorithm failed for " << importerInstanceIdentifier.toString(),
			ErrorType::Internal
		);
	};

	if (!importer->initialize())
	{
		releaseScenario();
		releaseAlgorithm();
		OV_ERROR_KRF("Can not initialize the requested scenario importer", ErrorType::Internal);
	}

	IParameter* memoryBufferParameter = importer->getInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer);
	IParameter* scenarioParameter = importer->getOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario);

	if (!(memoryBufferParameter && scenarioParameter))
	{
		releaseScenario();
		releaseAlgorithm();

		OV_ERROR_UNLESS_KRF(
			memoryBufferParameter,
			"The requested importer does not have a MemoryBuffer input parameter with identifier " << OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer.toString(),
			ErrorType::BadInput
		);

		OV_ERROR_UNLESS_KRF(
			scenarioParameter,
			"The requested importer does not have a Scenario output parameter with identifier " << OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario.toString(),
			ErrorType::BadOutput
		);
	}


	TParameterHandler<const IMemoryBuffer*> memoryBufferParameterHandler(memoryBufferParameter);
	TParameterHandler<IScenario*> scenarioParameterHandler(scenarioParameter);

	memoryBufferParameterHandler = &inputMemoryBuffer;
	scenarioParameterHandler = &newScenarioInstance;

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

bool CScenarioManager::importScenarioFromFile(OpenViBE::CIdentifier& newScenarioIdentifier, const char* fileName, const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	newScenarioIdentifier = OV_UndefinedIdentifier;

	CMemoryBuffer memoryBuffer;

	FILE* inputFile = FS::Files::open(fileName, "rb");

	OV_ERROR_UNLESS_KRF(
		inputFile,
		"Can not open scenario file '" << fileName << "'",
		ErrorType::BadFileRead
	);

	fseek(inputFile, 0, SEEK_END);
	memoryBuffer.setSize(static_cast<size_t>(::ftell(inputFile)), true);
	fseek(inputFile, 0, SEEK_SET);

	if (fread(reinterpret_cast<char*>(memoryBuffer.getDirectPointer()), (size_t)memoryBuffer.getSize(), 1, inputFile) != 1)
	{
		fclose(inputFile);
		OV_ERROR_KRF("Problem reading scenario file '" << fileName << "'", ErrorType::BadFileRead);
	}
	else
	{
		fclose(inputFile);
	}

	return this->importScenario(newScenarioIdentifier, memoryBuffer, scenarioImporterAlgorithmIdentifier);
}

bool CScenarioManager::importScenarioFromFile(CIdentifier& newScenarioIdentifier, const CIdentifier& importContext, const char* fileName)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters.count(importContext),
	                    "The import context " << importContext.toString() << " has no associated importers",
	                    ErrorType::Internal);
	char extension[255];
	FS::Files::getFilenameExtension(fileName, extension);
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters[importContext].count(extension),
	                    "The import context " << importContext.toString() << " has no associated importers for extension [" << extension << "]",
	                    ErrorType::Internal);
	return this->importScenarioFromFile(newScenarioIdentifier, fileName, m_ScenarioImporters[importContext][extension]);
}

bool CScenarioManager::registerScenarioImporter(const CIdentifier& importContext, const char* fileNameExtension, const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	if (!m_ScenarioImporters.count(importContext))
	{
		m_ScenarioImporters[importContext] = std::map<std::string, CIdentifier>();
	}

	OV_ERROR_UNLESS_KRF(!m_ScenarioImporters[importContext].count(fileNameExtension),
	                    "The file name extension [" << fileNameExtension << "] already has an importer registered for context " << importContext.toString(),
	                    ErrorType::Internal);

	m_ScenarioImporters[importContext][fileNameExtension] = scenarioImporterAlgorithmIdentifier;

	return true;
}

bool CScenarioManager::unregisterScenarioImporter(const OpenViBE::CIdentifier& importContext, const char* fileNameExtension)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters.count(importContext),
	                    "The import context " << importContext.toString() << " has no associated importers",
	                    ErrorType::Internal);
	OV_ERROR_UNLESS_KRF(m_ScenarioImporters[importContext].count(fileNameExtension),
	                    "The import context " << importContext.toString() << " has no associated importers for extension [" << fileNameExtension << "]",
	                    ErrorType::Internal);

	auto& contextImporters = m_ScenarioImporters[importContext];

	for (auto it = contextImporters.begin(); it != contextImporters.end();)
	{
		if (it->first == fileNameExtension)
		{
			it = contextImporters.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto it = m_ScenarioImporters.begin(); it != m_ScenarioImporters.end();)
	{
		if (it->second.empty())
		{
			it = m_ScenarioImporters.erase(it);
		}
		else
		{
			++it;
		}
	}
	return true;
}

OpenViBE::uint32 CScenarioManager::getRegisteredScenarioImportContextsCount() const
{
	return m_ScenarioImporters.size();
}

OpenViBE::uint32 CScenarioManager::getRegisteredScenarioImportersCount(const OpenViBE::CIdentifier& importContext) const
{
	if (!m_ScenarioImporters.count(importContext))
	{
		return 0;
	}
	return m_ScenarioImporters.at(importContext).size();
}

bool CScenarioManager::getRegisteredScenarioImportContextDetails(OpenViBE::uint32 index, OpenViBE::CIdentifier& importContext) const
{
	OV_ERROR_UNLESS_KRF(
	            this->getRegisteredScenarioImportContextsCount() > index,
	            "Scenario import context index out of range",
	            ErrorType::OutOfBound);
	// TODO: optimize this
	auto it = m_ScenarioImporters.cbegin();
	for (size_t i = 0; i < index; ++i)
	{
		++it;
	}
	importContext = it->first;
	return true;
}

bool CScenarioManager::getRegisteredScenarioImporterDetails(const OpenViBE::CIdentifier& importContext, OpenViBE::uint32 index, const char** fileNameExtension, OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier) const
{
	OV_ERROR_UNLESS_KRF(
	            this->getRegisteredScenarioImportersCount(importContext) > index,
	            "Scenario importer index out of range",
	            ErrorType::OutOfBound);

	// TODO: optimize this
	auto it = m_ScenarioImporters.at(importContext).cbegin();
	for (size_t i = 0; i < index; ++i)
	{
		++it;
	}
	*fileNameExtension = it->first.c_str();
	scenarioImporterAlgorithmIdentifier = it->second;
	return true;
}

bool CScenarioManager::exportScenario(OpenViBE::IMemoryBuffer& outputMemoryBuffer, const OpenViBE::CIdentifier& scenarioIdentifier, const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const
{
	OV_ERROR_UNLESS_KRF(
		m_vScenario.find(scenarioIdentifier) != m_vScenario.end(),
		"Scenario with identifier " << scenarioIdentifier.toString() << " does not exist.",
		ErrorType::ResourceNotFound
	);

	// If the scenario is a metabox, we will save its prototype hash into an attribute of the scenario
	// that way the standalone scheduler can check whether metaboxes included inside need updating.
	IScenario& scenario = this->getScenario(scenarioIdentifier);


	if (scenario.isMetabox())
	{
		SBoxProto l_oMetaboxProto;

		for (uint32 l_ui32ScenarioInputIndex = 0; l_ui32ScenarioInputIndex < scenario.getInputCount(); l_ui32ScenarioInputIndex++)
		{
			CString l_sInputName;
			CIdentifier l_oInputTypeIdentifier;

			scenario.getInputType(l_ui32ScenarioInputIndex, l_oInputTypeIdentifier);
			scenario.getInputName(l_ui32ScenarioInputIndex, l_sInputName);

			l_oMetaboxProto.addInput(l_sInputName, l_oInputTypeIdentifier);
		}

		for (uint32 l_ui32ScenarioOutputIndex = 0; l_ui32ScenarioOutputIndex < scenario.getOutputCount(); l_ui32ScenarioOutputIndex++)
		{
			CString l_sOutputName;
			CIdentifier l_oOutputTypeIdentifier;

			scenario.getOutputType(l_ui32ScenarioOutputIndex, l_oOutputTypeIdentifier);
			scenario.getOutputName(l_ui32ScenarioOutputIndex, l_sOutputName);

			l_oMetaboxProto.addOutput(l_sOutputName, l_oOutputTypeIdentifier);
		}

		for (uint32 l_ui32ScenarioSettingIndex = 0; l_ui32ScenarioSettingIndex < scenario.getSettingCount(); l_ui32ScenarioSettingIndex++)
		{
			CString l_sSettingName;
			CIdentifier l_oSettingTypeIdentifier;
			CString l_sSettingDefaultValue;

			scenario.getSettingName(l_ui32ScenarioSettingIndex, l_sSettingName);
			scenario.getSettingType(l_ui32ScenarioSettingIndex, l_oSettingTypeIdentifier);
			scenario.getSettingDefaultValue(l_ui32ScenarioSettingIndex, l_sSettingDefaultValue);

			l_oMetaboxProto.addSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sSettingDefaultValue);
		}

		if (scenario.hasAttribute(OV_AttributeId_Scenario_MetaboxHash))
		{
			scenario.setAttributeValue(OV_AttributeId_Scenario_MetaboxHash, l_oMetaboxProto.m_oHash.toString());
		}
		else
		{
			scenario.addAttribute(OV_AttributeId_Scenario_MetaboxHash, l_oMetaboxProto.m_oHash.toString());
		}
	}

	CIdentifier exporterInstanceIdentifier = this->getKernelContext().getAlgorithmManager().createAlgorithm(scenarioExporterAlgorithmIdentifier);

	OV_ERROR_UNLESS_KRF(
		exporterInstanceIdentifier != OV_UndefinedIdentifier,
		"Can not create the requested scenario exporter",
		ErrorType::BadResourceCreation
	);

	IAlgorithmProxy* exporter = &this->getKernelContext().getAlgorithmManager().getAlgorithm(exporterInstanceIdentifier);

	OV_FATAL_UNLESS_K(
		exporter,
		"Exporter with id " << exporterInstanceIdentifier.toString() << " not found although it has just been created",
		ErrorType::ResourceNotFound
	);

	auto releaseAlgorithm = [&](){
		// use a fatal here because a release failure while creation succeeded
		// means we are in an unexpected state
		OV_FATAL_UNLESS_K(
			this->getKernelContext().getAlgorithmManager().releaseAlgorithm(*exporter),
			"Releasing just created algorithm failed for " << exporterInstanceIdentifier.toString(),
			ErrorType::Internal
		);
	};

	if (!exporter->initialize())
	{
		releaseAlgorithm();
		OV_ERROR_KRF("Can not initialize the requested scenario exporter", ErrorType::Internal);
	}

	IParameter* scenarioParameter = exporter->getInputParameter(OV_Algorithm_ScenarioExporter_InputParameterId_Scenario);
	IParameter* memoryBufferParameter = exporter->getOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer);

	if (!(memoryBufferParameter && scenarioParameter))
	{
		releaseAlgorithm();

		OV_ERROR_UNLESS_KRF(
			scenarioParameter,
			"The requested exporter does not have a Scenario input parameter with identifier " << OV_Algorithm_ScenarioExporter_InputParameterId_Scenario.toString(),
			ErrorType::BadInput
		);

		OV_ERROR_UNLESS_KRF(
			memoryBufferParameter,
			"The requested exporter does not have a MemoryBuffer output parameter with identifier " << OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer.toString(),
			ErrorType::BadOutput
		);
	}

	TParameterHandler<IScenario*> scenarioParameterHandler(scenarioParameter);
	TParameterHandler<const IMemoryBuffer*> memoryBufferParameterHandler(memoryBufferParameter);

	scenarioParameterHandler = &scenario;
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

bool CScenarioManager::exportScenarioToFile(const char* fileName, const OpenViBE::CIdentifier& scenarioIdentifier, const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const
{
	CMemoryBuffer memoryBuffer;
	this->exportScenario(memoryBuffer, scenarioIdentifier, scenarioExporterAlgorithmIdentifier);

	std::ofstream outputFileStream;
	FS::Files::openOFStream(outputFileStream, fileName, ios::binary);

	OV_ERROR_UNLESS_KRF(
		outputFileStream.good(),
		"Failed to open file " << fileName,
		ErrorType::BadFileRead
	);

	outputFileStream.write(reinterpret_cast<const char*>(memoryBuffer.getDirectPointer()), static_cast<long>(memoryBuffer.getSize()));
	outputFileStream.close();

	return true;
}

bool CScenarioManager::exportScenarioToFile(const CIdentifier& exportContext, const char* fileName, const OpenViBE::CIdentifier& scenarioIdentifier)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters.count(exportContext),
	                    "The export context " << exportContext.toString() << " has no associated exporters",
	                    ErrorType::Internal);
	char extension[255];
	FS::Files::getFilenameExtension(fileName, extension);
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters[exportContext].count(extension),
	                    "The export context " << exportContext.toString() << " has no associated exporters for extension [" << extension << "]",
	                    ErrorType::Internal);
	return this->exportScenarioToFile(fileName, scenarioIdentifier, m_ScenarioExporters[exportContext][extension]);
}

bool CScenarioManager::registerScenarioExporter(const CIdentifier& exportContext, const char* fileNameExtension, const CIdentifier& scenarioExporterAlgorithmIdentifier)
{
	if (!m_ScenarioExporters.count(exportContext))
	{
		m_ScenarioExporters[exportContext] = std::map<std::string, CIdentifier>();
	}

	OV_ERROR_UNLESS_KRF(!m_ScenarioExporters[exportContext].count(fileNameExtension),
	                    "The file name extension [" << fileNameExtension << "] already has an exporter registered for context " << exportContext.toString(),
	                    ErrorType::Internal);

	m_ScenarioExporters[exportContext][fileNameExtension] = scenarioExporterAlgorithmIdentifier;

	return true;
}

bool CScenarioManager::unregisterScenarioExporter(const OpenViBE::CIdentifier& exportContext, const char* fileNameExtension)
{
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters.count(exportContext),
	                    "The export context " << exportContext.toString() << " has no associated exporters",
	                    ErrorType::Internal);
	OV_ERROR_UNLESS_KRF(m_ScenarioExporters[exportContext].count(fileNameExtension),
	                    "The export context " << exportContext.toString() << " has no associated exporters for extension [" << fileNameExtension << "]",
	                    ErrorType::Internal);

	auto& contextExporters = m_ScenarioImporters[exportContext];

	for (auto it = contextExporters.begin(); it != contextExporters.end();)
	{
		if (it->first == fileNameExtension)
		{
			it = contextExporters.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto it = m_ScenarioExporters.begin(); it != m_ScenarioImporters.end();)
	{
		if (it->second.empty())
		{
			it = m_ScenarioExporters.erase(it);
		}
		else
		{
			++it;
		}
	}
	return true;
}

OpenViBE::uint32 CScenarioManager::getRegisteredScenarioExportContextsCount() const
{
	return m_ScenarioExporters.size();
}

OpenViBE::uint32 CScenarioManager::getRegisteredScenarioExportersCount(const OpenViBE::CIdentifier& exportContext) const
{
	if (!m_ScenarioExporters.count(exportContext))
	{
		return 0;
	}
	return m_ScenarioExporters.at(exportContext).size();
}

bool CScenarioManager::getRegisteredScenarioExportContextDetails(OpenViBE::uint32 index, OpenViBE::CIdentifier& exportContext) const
{
	OV_ERROR_UNLESS_KRF(
	            this->getRegisteredScenarioExportContextsCount() > index,
	            "Scenario export context index out of range",
	            ErrorType::OutOfBound);
	// TODO: optimize this
	auto it = m_ScenarioExporters.cbegin();
	for (size_t i = 0; i < index; ++i)
	{
		++it;
	}
	exportContext = it->first;
	return true;
}

bool CScenarioManager::getRegisteredScenarioExporterDetails(const OpenViBE::CIdentifier& exportContext, OpenViBE::uint32 index, const char** fileNameExtension, OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const
{
	OV_ERROR_UNLESS_KRF(
	            this->getRegisteredScenarioExportersCount(exportContext) > index,
	            "Scenario exporter index out of range",
	            ErrorType::OutOfBound);

	// TODO: optimize this
	auto it = m_ScenarioExporters.at(exportContext).cbegin();
	for (size_t i = 0; i < index; ++i)
	{
		++it;
	}
	*fileNameExtension = it->first.c_str();
	scenarioExporterAlgorithmIdentifier = it->second;
	return true;
}

bool CScenarioManager::releaseScenario(
	const CIdentifier& rScenarioIdentifier)
{
	//retrieve iterator to scenario
	map<CIdentifier, CScenario*>::iterator itScenario;
	itScenario=m_vScenario.find(rScenarioIdentifier);
	if(itScenario==m_vScenario.end())
	{
		// error is handled on a higher level
		return false;
	}

	CScenario* l_pScenario = itScenario->second;
	delete l_pScenario;
	m_vScenario.erase(itScenario);

	return true;
}

IScenario& CScenarioManager::getScenario(
	const CIdentifier& rScenarioIdentifier)
{
	map<CIdentifier, CScenario*>::const_iterator itScenario;
	itScenario=m_vScenario.find(rScenarioIdentifier);

	// If the call is wrongly handled, and falls in this condition then next instruction causes a crash...
	// At least, here the abortion is handled!
	OV_FATAL_UNLESS_K(
		itScenario != m_vScenario.end(),
		"Scenario " << rScenarioIdentifier.toString() << " does not exist !",
		ErrorType::ResourceNotFound
	);

	return *itScenario->second;
}

IScenario& CScenarioManager::getScenario(const CIdentifier& rScenarioIdentifier) const
{
	return const_cast<CScenarioManager*>(this)->getScenario(rScenarioIdentifier);
}

CIdentifier CScenarioManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, CScenario*>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vScenario.find(l_oResult);
	}
	while(i!=m_vScenario.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}

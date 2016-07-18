#include "ovkCScenarioManager.h"
#include "ovkCScenario.h"

#include <cstdlib>
#include <fs/Files.h>
#include <cassert>
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
	for(map<CIdentifier, CScenario*>::iterator i=m_vScenario.begin(); i!=m_vScenario.end(); i++)
	{
		delete i->second;
	}
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
		itScenario++;
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

	bool hasCreateScenarioSucceeded = this->createScenario(newScenarioIdentifier);
	(void)hasCreateScenarioSucceeded;
	assert(hasCreateScenarioSucceeded);

	auto releaseScenario = [&](){
		bool hasReleaseScenarioSucceeded = this->releaseScenario(newScenarioIdentifier);
		(void)hasReleaseScenarioSucceeded;
		assert(hasReleaseScenarioSucceeded);
	};

	IScenario& newScenarioInstance = this->getScenario(newScenarioIdentifier);

	if (!inputMemoryBuffer.getSize())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Buffer containing scenario data is empty" << "\n";
		releaseScenario();
		return false;
	}

	CIdentifier importerInstanceIdentifier = this->getKernelContext().getAlgorithmManager().createAlgorithm(scenarioImporterAlgorithmIdentifier);

	if (importerInstanceIdentifier == OV_UndefinedIdentifier)
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not create the requested scenario importer" << "\n";
		releaseScenario();
		return false;
	}

	IAlgorithmProxy* importer = &this->getKernelContext().getAlgorithmManager().getAlgorithm(importerInstanceIdentifier);
	assert(importer);

	auto releaseAlgorithm = [&](){
		bool hasReleaseSucceeded = this->getKernelContext().getAlgorithmManager().releaseAlgorithm(*importer);
		(void)hasReleaseSucceeded;
		assert(hasReleaseSucceeded);
	};

	if (!importer->initialize())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not initialize the requested scenario importer" << "\n";
		releaseScenario();
		releaseAlgorithm();
		return false;
	}

	IParameter* memoryBufferParameter = importer->getInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer);
	IParameter* scenarioParameter = importer->getOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario);

	if (!(memoryBufferParameter && scenarioParameter))
	{
		if (!memoryBufferParameter)
		{
			this->getKernelContext().getLogManager() << LogLevel_Error << "The requested importer does not have a MemoryBuffer input parameter with identifier " << OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer << "\n";
		}
		if (!scenarioParameter)
		{
			this->getKernelContext().getLogManager() << LogLevel_Error << "The requested importer does not have a Scenario output parameter with identifier " << OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario << "\n";
		}
		releaseScenario();
		releaseAlgorithm();
		return false;
	}


	TParameterHandler<const IMemoryBuffer*> memoryBufferParameterHandler(memoryBufferParameter);
	TParameterHandler<IScenario*> scenarioParameterHandler(scenarioParameter);

	memoryBufferParameterHandler = &inputMemoryBuffer;
	scenarioParameterHandler = &newScenarioInstance;

	if (!importer->process())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not process data using the requested scenario importer" << "\n";
		releaseScenario();
		releaseAlgorithm();
		return false;
	}

	if (!importer->uninitialize())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not uninitialize the requested scenario importer" << "\n";
		releaseScenario();
		releaseAlgorithm();
		return false;
	}

	releaseAlgorithm();

	return true;
}

bool CScenarioManager::importScenarioFromFile(OpenViBE::CIdentifier& newScenarioIdentifier, const char* fileName, const CIdentifier& scenarioImporterAlgorithmIdentifier)
{
	newScenarioIdentifier = OV_UndefinedIdentifier;

	CMemoryBuffer memoryBuffer;

	FILE* inputFile = FS::Files::open(fileName, "rb");
	if (!inputFile)
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not open scenario file '" << fileName << "'" << "\n";
		return false;
	}

	fseek(inputFile, 0, SEEK_END);
	memoryBuffer.setSize(static_cast<size_t>(::ftell(inputFile)), true);
	fseek(inputFile, 0, SEEK_SET);

	if (fread(reinterpret_cast<char*>(memoryBuffer.getDirectPointer()), (size_t)memoryBuffer.getSize(), 1, inputFile) != 1)
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Problem reading scenario file '" << fileName << "'" << "\n";
		fclose(inputFile);
		return false;
	}
	fclose(inputFile);

	return this->importScenario(newScenarioIdentifier, memoryBuffer, scenarioImporterAlgorithmIdentifier);
}

bool CScenarioManager::exportScenario(OpenViBE::IMemoryBuffer& outputMemoryBuffer, const OpenViBE::CIdentifier& scenarioIdentifier, const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier)
{
	if (m_vScenario.find(scenarioIdentifier) == m_vScenario.end())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Scenario with identifier " << scenarioIdentifier << " does not exist." << "\n";
		return false;
	}

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

	if (exporterInstanceIdentifier == OV_UndefinedIdentifier)
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not create the requested scenario exporter" << "\n";
		return false;
	}

	IAlgorithmProxy* exporter = &this->getKernelContext().getAlgorithmManager().getAlgorithm(exporterInstanceIdentifier);
	assert(exporter);

	auto releaseAlgorithm = [&](){
		bool hasReleaseSucceeded = this->getKernelContext().getAlgorithmManager().releaseAlgorithm(*exporter);
		(void)hasReleaseSucceeded;
		assert(hasReleaseSucceeded);
	};

	if (!exporter->initialize())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not initialize the requested scenario exporter" << "\n";
		releaseAlgorithm();
		return false;
	}

	IParameter* scenarioParameter = exporter->getInputParameter(OV_Algorithm_ScenarioExporter_InputParameterId_Scenario);
	IParameter* memoryBufferParameter = exporter->getOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer);

	if (!(memoryBufferParameter && scenarioParameter))
	{
		if (!scenarioParameter)
		{
			this->getKernelContext().getLogManager() << LogLevel_Error << "The requested exporter does not have a Scenario input parameter with identifier " << OV_Algorithm_ScenarioExporter_InputParameterId_Scenario << "\n";
		}
		if (!memoryBufferParameter)
		{
			this->getKernelContext().getLogManager() << LogLevel_Error << "The requested exporter does not have a MemoryBuffer output parameter with identifier " << OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer << "\n";
		}
		releaseAlgorithm();
		return false;
	}

	TParameterHandler<IScenario*> scenarioParameterHandler(scenarioParameter);
	TParameterHandler<const IMemoryBuffer*> memoryBufferParameterHandler(memoryBufferParameter);

	scenarioParameterHandler = &scenario;
	memoryBufferParameterHandler = &outputMemoryBuffer;

	if (!exporter->process())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not process data using the requested scenario exporter" << "\n";
		releaseAlgorithm();
		return false;
	}

	if (!exporter->uninitialize())
	{
		this->getKernelContext().getLogManager() << LogLevel_Error << "Can not uninitialize the requested scenario exporter" << "\n";
		releaseAlgorithm();
		return false;
	}

	releaseAlgorithm();
	return true;
}

bool CScenarioManager::exportScenarioToFile(const char* fileName, const OpenViBE::CIdentifier& scenarioIdentifier, const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier)
{
	CMemoryBuffer memoryBuffer;
	this->exportScenario(memoryBuffer, scenarioIdentifier, scenarioExporterAlgorithmIdentifier);

	std::ofstream outputFileStream;
	FS::Files::openOFStream(outputFileStream, fileName, ios::binary);
	if (!outputFileStream.good())
	{
		return false;
	}

	outputFileStream.write(reinterpret_cast<const char*>(memoryBuffer.getDirectPointer()), static_cast<long>(memoryBuffer.getSize()));
	outputFileStream.close();

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
	if(itScenario==m_vScenario.end())
	{
		this->getLogManager() << LogLevel_Fatal << "Scenario " << rScenarioIdentifier << " does not exist !\n";
		// If the call is wrongly handled, and falls in this condition then next instruction causes a crash...
		// TODO: return something here (but what ?)
	}
	return *itScenario->second;
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

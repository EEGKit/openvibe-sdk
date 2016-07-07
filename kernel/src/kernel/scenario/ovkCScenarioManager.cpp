#include "ovkCScenarioManager.h"
#include "ovkCScenario.h"

#include <cstdlib>
#include <fs/Files.h>
#include <cassert>
#include <openvibe/kernel/scenario/ovIAlgorithmScenarioImporter.h>

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

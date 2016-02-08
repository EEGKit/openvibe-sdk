/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <map>
#include <limits>
#include <cassert>

#include <system/Time.h>
#include <openvibe/ovITimeArithmetics.h>

#include "ovspCCommand.h"
#include "ovsp_base.h"
#include "ovspCKernelFacade.h"

namespace OpenViBE
{
	using namespace OpenViBE::Kernel;
	using namespace OpenViBE::Plugins;

	struct KernelFacade::KernelFacadeImpl
	{
		CKernelLoader m_KernelLoader;
		IKernelContext* m_KernelContext{ nullptr };
		std::map<std::string, CIdentifier> m_ScenarioMap;
	};

	KernelFacade::KernelFacade() : m_Pimpl(new KernelFacadeImpl())
	{
	}

	// The destructor is needed in the .cpp file to implement pimpl idiom
	// with unique_ptr. This is due to the fact that Kernel facade dtor
	// has to call unique_ptr<KernelFacadeImpl> deleter function that calls the detete function
	// on KernelFacadeImpl. Therefore it needs to know KernelFacadeImpl implementation.
	KernelFacade::~KernelFacade()
	{
		this->unloadKernel();
		this->uninitialize();
	}

	PlayerReturnCode KernelFacade::initialize(const InitCommand& command)
	{
		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::uninitialize()
	{
		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::loadKernel(const LoadKernelCommand& command)
	{
		if (m_Pimpl->m_KernelContext)
		{
			std::cout << "WARNING: The kernel is already loaded" << std::endl;
			return PlayerReturnCode::Success;
		}

		CString kernelFile;

#if defined TARGET_OS_Windows
		kernelFile = OpenViBE::Directories::getLibDir() + "/openvibe-kernel.dll";
#else
		kernelFile = OpenViBE::Directories::getLibDir() + "/libopenvibe-kernel.so";
#endif

		CKernelLoader& kernelLoader = m_Pimpl->m_KernelLoader;
		CString error;

		if (!kernelLoader.load(kernelFile, &error))
		{
			std::cerr << "ERROR: impossible to load kernel from file located at: " << kernelFile << std::endl;
			return PlayerReturnCode::KernelLoadingFailure;
		}

		kernelLoader.initialize();

		IKernelDesc* kernelDesc{ nullptr };
		kernelLoader.getKernelDesc(kernelDesc);
		
		if (!kernelDesc)
		{
			std::cerr << "ERROR: impossible to retrieve kernel descriptor " << std::endl;
			return PlayerReturnCode::KernelInvalidDesc;
		}

		CString configurationFile;

		if (command.configurationFile && !command.configurationFile.get().empty())
		{
			configurationFile = command.configurationFile.get().c_str();
		}
		else
		{
			configurationFile = CString(OpenViBE::Directories::getDataDir() + "/kernel/openvibe.conf");
		}


		IKernelContext* kernelContext = kernelDesc->createKernel("scenario-player", configurationFile);

		if (!kernelContext)
		{
			std::cerr << "ERROR: impossible to create kernel context " << std::endl;
			return PlayerReturnCode::KernelInvalidDesc;
		}

		// kernelContext->initialize();
		m_Pimpl->m_KernelContext = kernelContext;
		OpenViBEToolkit::initialize(*kernelContext);

		IConfigurationManager& configurationManager = kernelContext->getConfigurationManager();
		kernelContext->getPluginManager().addPluginsFromFiles(configurationManager.expand("${Kernel_Plugins}"));

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::unloadKernel()
	{
		if (m_Pimpl->m_KernelContext)
		{
			// not releasing the scenario before releasing the kernel
			// causes a segfault on linux
			auto& scenarioManager = m_Pimpl->m_KernelContext->getScenarioManager();
			for (auto scenarioPair : m_Pimpl->m_ScenarioMap)
			{
				scenarioManager.releaseScenario(scenarioPair.second);
			}


			OpenViBEToolkit::uninitialize(*m_Pimpl->m_KernelContext);
			// m_Pimpl->m_KernelContext->uninitialize();
			IKernelDesc* kernelDesc{ nullptr };
			m_Pimpl->m_KernelLoader.getKernelDesc(kernelDesc);
			kernelDesc->releaseKernel(m_Pimpl->m_KernelContext);
			m_Pimpl->m_KernelContext = nullptr;
		}

		m_Pimpl->m_KernelLoader.uninitialize();
		m_Pimpl->m_KernelLoader.unload();

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::loadScenario(const LoadScenarioCommand& command)
	{
		assert(command.m_ScenarioFile && command.m_ScenarioName);

		if (!m_Pimpl->m_KernelContext)
		{
			std::cerr << "ERROR: Kernel is not loaded" << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		std::string scenarioFile = command.scenarioFile.get();
		std::string scenarioName = command.scenarioName.get();

		CIdentifier scenarioIdentifier;
		auto& scenarioManager = m_Pimpl->m_KernelContext->getScenarioManager();

		if (!scenarioManager.createScenario(scenarioIdentifier))
		{
			std::cerr << "ERROR: failed to create scenario " << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		IScenario& scenarioToImport = scenarioManager.getScenario(scenarioIdentifier);

		// use of returnCode to store error and achive an RAII-like
		// behavior by releasing scenario at the end
		PlayerReturnCode returnCode = PlayerReturnCode::Success;

		CMemoryBuffer fileBuffer;
		std::FILE* fileHandle = std::fopen(scenarioFile.c_str(), "rb");
		
		if (fileHandle)
		{
			std::string contents;
			std::fseek(fileHandle, 0, SEEK_END);
			fileBuffer.setSize(std::ftell(fileHandle), true);
			std::rewind(fileHandle);
			std::fread(fileBuffer.getDirectPointer(), 1, static_cast<std::size_t>(fileBuffer.getSize()), fileHandle);
			std::fclose(fileHandle);

			auto& algorithmManager = m_Pimpl->m_KernelContext->getAlgorithmManager();
			auto algorithmIdentifier = algorithmManager.createAlgorithm(OVP_GD_ClassId_Algorithm_XMLScenarioImporter);

			IAlgorithmProxy* scenarioImporter{ nullptr };
			if (algorithmIdentifier != OV_UndefinedIdentifier &&
				(scenarioImporter = &algorithmManager.getAlgorithm(algorithmIdentifier))) // assignment in 'if' is generally an anti-pattern because of
																						  // non-maintainability but here we can get rid of an if/else section...
			{
				scenarioImporter->initialize();

				TParameterHandler<const IMemoryBuffer*> inputBuffer(scenarioImporter->getInputParameter(OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
				TParameterHandler<IScenario*> outputScenario(scenarioImporter->getOutputParameter(OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario));

				inputBuffer = &fileBuffer;
				outputScenario = &scenarioToImport;
				
				bool importSuccess = scenarioImporter->process();
				scenarioImporter->uninitialize();

				algorithmManager.releaseAlgorithm(*scenarioImporter);

				if (!importSuccess)
				{
					std::cerr << "ERROR: failed to import scenario  " << std::endl;
					returnCode = PlayerReturnCode::KernelInternalFailure;
				}

			}
			else
			{
				std::cerr << "ERROR: failed to generate an importer to import scenario file " << std::endl;
				returnCode = PlayerReturnCode::KernelInternalFailure;
			}

		}
		else
		{
			std::cerr << "ERROR: failed to open scenario file at location " << scenarioFile << std::endl;
			returnCode =  PlayerReturnCode::OpeningFileFailure;
		}

		if (returnCode != PlayerReturnCode::Success)
		{
			scenarioManager.releaseScenario(scenarioIdentifier);
		}
		else
		{
			auto scenarioToReleaseIt = m_Pimpl->m_ScenarioMap.find(scenarioName);
			if (scenarioToReleaseIt != m_Pimpl->m_ScenarioMap.end())
			{
				scenarioManager.releaseScenario(scenarioToReleaseIt->second);
			}

			m_Pimpl->m_ScenarioMap[scenarioName] = scenarioIdentifier;
		}

		return returnCode;
	}


	PlayerReturnCode KernelFacade::runScenarioList(const RunScenarioCommand& command)
	{
		assert(command.m_ScenarioList);

		if (!m_Pimpl->m_KernelContext)
		{
			std::cerr << "ERROR: Kernel is not loaded" << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		auto scenarioList = command.scenarioList.get();

		// use of returnCode to store error and achive an RAII-like
		// behavior by releasing all players at the end
		PlayerReturnCode returnCode = PlayerReturnCode::Success;

		auto& playerManager = m_Pimpl->m_KernelContext->getPlayerManager();

		// Keep 2 different containers because identifier information is
		// not relevant during the performance sensitive loop task.
		// This might be premature optimization...
		std::vector<IPlayer*> playerList;
		std::vector<CIdentifier> playerIdentifiersList;

		// attach players to scenario
		for (auto& pair : m_Pimpl->m_ScenarioMap)
		{
			if (std::find(scenarioList.begin(), scenarioList.end(), pair.first) ==
				scenarioList.end()) // not in the list of scenario to run
			{
				continue;
			}

			CIdentifier playerIdentifier;
			if (!playerManager.createPlayer(playerIdentifier) || playerIdentifier == OV_UndefinedIdentifier)
			{
				std::cerr << "ERROR: impossible to create player" << std::endl;
				returnCode = PlayerReturnCode::KernelInternalFailure;
				break;
			}

			IPlayer* player = &playerManager.getPlayer(playerIdentifier);

			// Scenario attachment with setup of local token
			if (player->setScenario(pair.second) && player->initialize())
			{
				if (command.playMode && command.playMode.get() == PlayerPlayMode::Fastfoward)
				{
					player->forward();
				}
				else
				{
					player->play();
				}

				playerList.push_back(player);	
				playerIdentifiersList.push_back(playerIdentifier);
			}
			else
			{
				std::cerr << "ERROR: impossible to initialize player for scenario " << pair.first.c_str() << std::endl;
				returnCode = PlayerReturnCode::KernelInternalFailure;
				break;
			}
		}

		if (returnCode == PlayerReturnCode::Success)
		{
			// loop until timeout
			uint64 startTime = System::Time::zgetTime();
			uint64 lastLoopTime = startTime;

			// cannot directly feed secondsToTime with parameters.m_MaximumExecutionTime
			// because it could overflow
			float64 boundedMaxExecutionTimeInS = ITimeArithmetics::timeToSeconds(std::numeric_limits<uint64>::max());

			uint64 maxExecutionTimeInFixedPoint;
			if (command.maximumExecutionTime &&
				command.maximumExecutionTime.get() > 0 &&
				command.maximumExecutionTime.get() < boundedMaxExecutionTimeInS)
			{
				maxExecutionTimeInFixedPoint = ITimeArithmetics::secondsToTime(command.maximumExecutionTime.get());
			}
			else
			{
				maxExecutionTimeInFixedPoint = std::numeric_limits<uint64>::max();
			}

			bool allStopped{ false };
			while (!allStopped) // negative condition here because it is easier to reason about it
			{
				uint64 currentTime = System::Time::zgetTime();
				allStopped = true;
				for (auto p : playerList)
				{
					p->loop(currentTime - lastLoopTime, maxExecutionTimeInFixedPoint);

					if (p->getCurrentSimulatedTime() >= maxExecutionTimeInFixedPoint)
					{
						p->stop();
					}

					allStopped &= (p->getStatus() == EPlayerStatus::PlayerStatus_Stop);
				}

				lastLoopTime = currentTime;
			}

		}

		// release players
		for (auto& id : playerIdentifiersList)
		{
			playerManager.releasePlayer(id);
		}
		
		return returnCode;
	}
}
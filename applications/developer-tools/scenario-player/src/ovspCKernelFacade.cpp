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

#include <system/ovCTime.h>
#include <openvibe/ovITimeArithmetics.h>

#include "ovspCCommand.h"
#include "ovsp_base.h"
#include "ovspCKernelFacade.h"

namespace OpenViBE
{
	using namespace OpenViBE::Kernel;
	using namespace OpenViBE::Plugins;
	using TokenList = std::vector<std::pair<std::string, std::string>>;

	namespace
	{
		void setConfigurationTokenList(IConfigurationManager& configurationManager, const TokenList& tokenList)
		{
			for (auto& token : tokenList)
			{
				configurationManager.addOrReplaceConfigurationToken(token.first.c_str(), token.second.c_str());
			}
		}
	}

	struct KernelFacade::KernelFacadeImpl
	{
		CKernelLoader kernelLoader;
		IKernelContext* kernelContext = nullptr;
		std::map<std::string, CIdentifier> scenarioMap;
		std::map<std::string, TokenList> scenarioTokenMap;
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
		if (m_Pimpl->kernelContext)
		{
			std::cout << "WARNING: The kernel is already loaded" << std::endl;
			return PlayerReturnCode::Success;
		}

		CString kernelFile;

#if defined TARGET_OS_Windows
		kernelFile = OpenViBE::Directories::getLibDir() + "/openvibe-kernel.dll";
#elif defined TARGET_OS_Linux
		kernelFile = OpenViBE::Directories::getLibDir() + "/libopenvibe-kernel.so";
#elif defined TARGET_OS_MacOS
		kernelFile = OpenViBE::Directories::getLibDir() + "/libopenvibe-kernel.dylib";
#endif

		CKernelLoader& kernelLoader = m_Pimpl->kernelLoader;
		CString error;

		if (!kernelLoader.load(kernelFile, &error))
		{
			std::cerr << "ERROR: impossible to load kernel from file located at: " << kernelFile << std::endl;
			return PlayerReturnCode::KernelLoadingFailure;
		}

		kernelLoader.initialize();

		IKernelDesc* kernelDesc = nullptr;
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

		kernelContext->initialize();
		m_Pimpl->kernelContext = kernelContext;
		OpenViBEToolkit::initialize(*kernelContext);

		IConfigurationManager& configurationManager = kernelContext->getConfigurationManager();
		kernelContext->getPluginManager().addPluginsFromFiles(configurationManager.expand("${Kernel_Plugins}"));
		kernelContext->getMetaboxManager().addMetaboxesFromFiles(configurationManager.expand("${Kernel_Metabox}"));

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::unloadKernel()
	{
		if (m_Pimpl->kernelContext)
		{
			// not releasing the scenario before releasing the kernel
			// causes a segfault on linux
			auto& scenarioManager = m_Pimpl->kernelContext->getScenarioManager();
			for (auto scenarioPair : m_Pimpl->scenarioMap)
			{
				scenarioManager.releaseScenario(scenarioPair.second);
			}


			OpenViBEToolkit::uninitialize(*m_Pimpl->kernelContext);
			// m_Pimpl->kernelContext->uninitialize();
			IKernelDesc* kernelDesc = nullptr;
			m_Pimpl->kernelLoader.getKernelDesc(kernelDesc);
			kernelDesc->releaseKernel(m_Pimpl->kernelContext);
			m_Pimpl->kernelContext = nullptr;
		}

		m_Pimpl->kernelLoader.uninitialize();
		m_Pimpl->kernelLoader.unload();

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::loadScenario(const LoadScenarioCommand& command)
	{
		assert(command.scenarioFile && command.scenarioName);

		if (!m_Pimpl->kernelContext)
		{
			std::cerr << "ERROR: Kernel is not loaded" << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		std::string scenarioFile = command.scenarioFile.get();
		std::string scenarioName = command.scenarioName.get();

		CIdentifier scenarioIdentifier;
		auto& scenarioManager = m_Pimpl->kernelContext->getScenarioManager();

		if (!scenarioManager.importScenarioFromFile(scenarioIdentifier, scenarioFile.c_str(), OVP_GD_ClassId_Algorithm_XMLScenarioImporter))
		{
			std::cerr << "ERROR: failed to create scenario " << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		scenarioManager.getScenario(scenarioIdentifier).addAttribute(OV_AttributeId_ScenarioFilename, scenarioFile.c_str());

		auto scenarioToReleaseIt = m_Pimpl->scenarioMap.find(scenarioName);
		if (scenarioToReleaseIt != m_Pimpl->scenarioMap.end())
		{
			scenarioManager.releaseScenario(scenarioToReleaseIt->second);
		}

		m_Pimpl->scenarioMap[scenarioName] = scenarioIdentifier;

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::setupScenario(const SetupScenarioCommand& command)
	{
		if (!m_Pimpl->kernelContext)
		{
			std::cerr << "ERROR: Kernel is not loaded" << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		if (!command.scenarioName)
		{
			std::cerr << "ERROR: Missing scenario name for setup" << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		auto scenarioName = command.scenarioName.get();

		if (m_Pimpl->scenarioMap.find(scenarioName) == m_Pimpl->scenarioMap.end())
		{
			std::cerr << "ERROR: Trying to configure not loaded scenario " << scenarioName << std::endl;
			return PlayerReturnCode::ScenarioNotLoaded;
		}

		// token list is just stored at this step for further use at runtime
		// current token list overwrites the previous one
		if (command.tokenList)
		{
			m_Pimpl->scenarioTokenMap[scenarioName] = command.tokenList.get();
		}

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode KernelFacade::runScenarioList(const RunScenarioCommand& command)
	{
		assert(command.scenarioList);

		if (!m_Pimpl->kernelContext)
		{
			std::cerr << "ERROR: Kernel is not loaded" << std::endl;
			return PlayerReturnCode::KernelInternalFailure;
		}

		auto scenarioList = command.scenarioList.get();

		// use of returnCode to store error and achive an RAII-like
		// behavior by releasing all players at the end
		PlayerReturnCode returnCode = PlayerReturnCode::Success;

		// set up global token
		if (command.tokenList)
		{
			setConfigurationTokenList(m_Pimpl->kernelContext->getConfigurationManager(), command.tokenList.get());
		}

		auto& playerManager = m_Pimpl->kernelContext->getPlayerManager();

		// Keep 2 different containers because identifier information is
		// not relevant during the performance sensitive loop task.
		// This might be premature optimization...
		std::vector<IPlayer*> playerList;
		std::vector<CIdentifier> playerIdentifiersList;

		// attach players to scenario
		for (auto& scenarioPair : m_Pimpl->scenarioMap)
		{
			auto scenarioName = scenarioPair.first;
			if (std::find(scenarioList.begin(), scenarioList.end(), scenarioName) ==
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

			// player identifier is pushed here to ensure a correct cleanup event if player initialization fails
			playerIdentifiersList.push_back(playerIdentifier);

			// Scenario attachment with setup of local token
			if (player->setScenario(scenarioPair.second))
			{
				// set scenario specific token
				setConfigurationTokenList(player->getRuntimeConfigurationManager(), m_Pimpl->scenarioTokenMap[scenarioName]);
			}
			else
			{
				std::cerr << "ERROR: impossible to set player scenario " << scenarioName << std::endl;
				returnCode = PlayerReturnCode::KernelInternalFailure;
				break;
			}

			if (player->initialize() == PlayerReturnCode_Sucess)
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
			}
			else
			{
				std::cerr << "ERROR: impossible to initialize player for scenario " << scenarioName << std::endl;
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
					if(p->getStatus() != EPlayerStatus::PlayerStatus_Stop)
					{
						p->loop(currentTime - lastLoopTime, maxExecutionTimeInFixedPoint);
					}

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
			playerManager.getPlayer(id).uninitialize();
			playerManager.releasePlayer(id);
		}

		return returnCode;
	}
}

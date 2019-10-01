/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
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
#include <openvibe/ovTimeArithmetics.h>

#include "ovspCCommand.h"
#include "ovsp_base.h"
#include "ovspCKernelFacade.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using TokenList = std::vector<std::pair<std::string, std::string>>;

namespace
{
	void setConfigurationTokenList(IConfigurationManager& configurationManager, const TokenList& tokenList) { for (auto& token : tokenList) { configurationManager.addOrReplaceConfigurationToken(token.first.c_str(), token.second.c_str()); } }
} // namespace

struct KernelFacade::KernelFacadeImpl
{
	CKernelLoader kernelLoader;
	IKernelContext* ctx = nullptr;
	std::map<std::string, CIdentifier> scenarioMap;
	std::map<std::string, TokenList> scenarioTokenMap;
};

KernelFacade::KernelFacade() : m_Pimpl(new KernelFacadeImpl()) { }

// The destructor is needed in the .cpp file to implement pimpl idiom
// with unique_ptr. This is due to the fact that Kernel facade dtor
// has to call unique_ptr<KernelFacadeImpl> deleter function that calls the detete function
// on KernelFacadeImpl. Therefore it needs to know KernelFacadeImpl implementation.
KernelFacade::~KernelFacade()
{
	this->unloadKernel();
	this->uninitialize();
}

OpenViBE::EPlayerReturnCode KernelFacade::initialize(const InitCommand& /*command*/) { return OpenViBE::EPlayerReturnCode::Success; }

OpenViBE::EPlayerReturnCode KernelFacade::uninitialize() { return OpenViBE::EPlayerReturnCode::Success; }

OpenViBE::EPlayerReturnCode KernelFacade::loadKernel(const LoadKernelCommand& command)
{
	if (m_Pimpl->ctx)
	{
		std::cout << "WARNING: The kernel is already loaded" << std::endl;
		return OpenViBE::EPlayerReturnCode::Success;
	}

	CString kernelFile;

#if defined TARGET_OS_Windows
	kernelFile = Directories::getLibDir() + "/openvibe-kernel.dll";
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
		return OpenViBE::EPlayerReturnCode::KernelLoadingFailure;
	}

	kernelLoader.initialize();

	IKernelDesc* kernelDesc = nullptr;
	kernelLoader.getKernelDesc(kernelDesc);

	if (!kernelDesc)
	{
		std::cerr << "ERROR: impossible to retrieve kernel descriptor " << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInvalidDesc;
	}

	CString configurationFile;

	if (command.configurationFile && !command.configurationFile.get().empty()) { configurationFile = command.configurationFile.get().c_str(); }
	else { configurationFile = CString(Directories::getDataDir() + "/kernel/openvibe.conf"); }


	IKernelContext* ctx = kernelDesc->createKernel("scenario-player", configurationFile);

	if (!ctx)
	{
		std::cerr << "ERROR: impossible to create kernel context " << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInvalidDesc;
	}

	ctx->initialize();
	m_Pimpl->ctx = ctx;
	OpenViBEToolkit::initialize(*ctx);

	IConfigurationManager& configurationManager = ctx->getConfigurationManager();
	ctx->getPluginManager().addPluginsFromFiles(configurationManager.expand("${Kernel_Plugins}"));
	ctx->getMetaboxManager().addMetaboxesFromFiles(configurationManager.expand("${Kernel_Metabox}"));

	return OpenViBE::EPlayerReturnCode::Success;
}

OpenViBE::EPlayerReturnCode KernelFacade::unloadKernel()
{
	if (m_Pimpl->ctx)
	{
		// not releasing the scenario before releasing the kernel
		// causes a segfault on linux
		auto& scenarioManager = m_Pimpl->ctx->getScenarioManager();
		for (auto& scenarioPair : m_Pimpl->scenarioMap) { scenarioManager.releaseScenario(scenarioPair.second); }


		OpenViBEToolkit::uninitialize(*m_Pimpl->ctx);
		// m_Pimpl->ctx->uninitialize();
		IKernelDesc* kernelDesc = nullptr;
		m_Pimpl->kernelLoader.getKernelDesc(kernelDesc);
		kernelDesc->releaseKernel(m_Pimpl->ctx);
		m_Pimpl->ctx = nullptr;
	}

	m_Pimpl->kernelLoader.uninitialize();
	m_Pimpl->kernelLoader.unload();

	return OpenViBE::EPlayerReturnCode::Success;
}

OpenViBE::EPlayerReturnCode KernelFacade::loadScenario(const LoadScenarioCommand& command)
{
	assert(command.scenarioFile && command.scenarioName);

	if (!m_Pimpl->ctx)
	{
		std::cerr << "ERROR: Kernel is not loaded" << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInternalFailure;
	}

	const std::string scenarioFile = command.scenarioFile.get();
	const std::string scenarioName = command.scenarioName.get();

	CIdentifier scenarioID;
	auto& scenarioManager = m_Pimpl->ctx->getScenarioManager();

	if (!scenarioManager.importScenarioFromFile(scenarioID, scenarioFile.c_str(), OVP_GD_ClassId_Algorithm_XMLScenarioImporter))
	{
		std::cerr << "ERROR: failed to create scenario " << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInternalFailure;
	}

	scenarioManager.getScenario(scenarioID).addAttribute(OV_AttributeId_ScenarioFilename, scenarioFile.c_str());

	const auto scenarioToReleaseIt = m_Pimpl->scenarioMap.find(scenarioName);
	if (scenarioToReleaseIt != m_Pimpl->scenarioMap.end()) { scenarioManager.releaseScenario(scenarioToReleaseIt->second); }

	m_Pimpl->scenarioMap[scenarioName] = scenarioID;

	return OpenViBE::EPlayerReturnCode::Success;
}

OpenViBE::EPlayerReturnCode KernelFacade::updateScenario(const UpdateScenarioCommand& command)
{
	assert(command.scenarioFile && command.scenarioName);

	auto& scenarioManager = m_Pimpl->ctx->getScenarioManager();

	const auto scenarioName = command.scenarioName.get();
	const auto scenarioFile = command.scenarioFile.get();

	if (m_Pimpl->scenarioMap.find(scenarioName) == m_Pimpl->scenarioMap.end())
	{
		std::cerr << "ERROR: Trying to update a not loaded scenario " << scenarioName << std::endl;
		return OpenViBE::EPlayerReturnCode::ScenarioNotLoaded;
	}

	auto& scenario = scenarioManager.getScenario(m_Pimpl->scenarioMap[scenarioName]);

	// check for boxes to be updated
	//		scenario.checkBoxesRequiringUpdate();

	// update boxes to be updated
	CIdentifier* listID = nullptr;
	size_t elemCount    = 0;
	scenario.getOutdatedBoxIdentifierList(&listID, &elemCount);
	for (size_t i = 0; i < elemCount; ++i) { scenario.updateBox(listID[i]); }

	// export scenario to the destination file
	if (!scenarioManager.exportScenarioToFile(scenarioFile.c_str(), m_Pimpl->scenarioMap[scenarioName], OVP_GD_ClassId_Algorithm_XMLScenarioExporter))
	{
		std::cerr << "ERROR: failed to create scenario " << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInternalFailure;
	}

	return OpenViBE::EPlayerReturnCode::Success;
}

OpenViBE::EPlayerReturnCode KernelFacade::setupScenario(const SetupScenarioCommand& command)
{
	if (!m_Pimpl->ctx)
	{
		std::cerr << "ERROR: Kernel is not loaded" << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInternalFailure;
	}

	if (!command.scenarioName)
	{
		std::cerr << "ERROR: Missing scenario name for setup" << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInternalFailure;
	}

	const auto scenarioName = command.scenarioName.get();

	if (m_Pimpl->scenarioMap.find(scenarioName) == m_Pimpl->scenarioMap.end())
	{
		std::cerr << "ERROR: Trying to configure not loaded scenario " << scenarioName << std::endl;
		return OpenViBE::EPlayerReturnCode::ScenarioNotLoaded;
	}

	// token list is just stored at this step for further use at runtime
	// current token list overwrites the previous one
	if (command.tokenList) { m_Pimpl->scenarioTokenMap[scenarioName] = command.tokenList.get(); }

	return OpenViBE::EPlayerReturnCode::Success;
}

OpenViBE::EPlayerReturnCode KernelFacade::runScenarioList(const RunScenarioCommand& command)
{
	assert(command.scenarioList);

	if (!m_Pimpl->ctx)
	{
		std::cerr << "ERROR: Kernel is not loaded" << std::endl;
		return OpenViBE::EPlayerReturnCode::KernelInternalFailure;
	}

	auto scenarioList = command.scenarioList.get();

	// use of returnCode to store error and achive an RAII-like
	// behavior by releasing all players at the end
	OpenViBE::EPlayerReturnCode returnCode = OpenViBE::EPlayerReturnCode::Success;

	// set up global token
	if (command.tokenList) { setConfigurationTokenList(m_Pimpl->ctx->getConfigurationManager(), command.tokenList.get()); }

	auto& playerManager = m_Pimpl->ctx->getPlayerManager();

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
			scenarioList.end()) { continue; } // not in the list of scenario to run 

		CIdentifier playerIdentifier;
		if (!playerManager.createPlayer(playerIdentifier) || playerIdentifier == OV_UndefinedIdentifier)
		{
			std::cerr << "ERROR: impossible to create player" << std::endl;
			returnCode = OpenViBE::EPlayerReturnCode::KernelInternalFailure;
			break;
		}

		IPlayer* player = &playerManager.getPlayer(playerIdentifier);

		// player identifier is pushed here to ensure a correct cleanup event if player initialization fails
		playerIdentifiersList.push_back(playerIdentifier);

		CNameValuePairList configurationTokensMap;
		for (auto& token : m_Pimpl->scenarioTokenMap[scenarioName]) { configurationTokensMap.setValue(token.first.c_str(), token.second.c_str()); }

		// Scenario attachment with setup of local token
		if (!player->setScenario(scenarioPair.second, &configurationTokensMap))
		{
			std::cerr << "ERROR: impossible to set player scenario " << scenarioName << std::endl;
			returnCode = OpenViBE::EPlayerReturnCode::KernelInternalFailure;
			break;
		}

		if (player->initialize() == PlayerReturnCode_Sucess)
		{
			if (command.playMode && command.playMode.get() == PlayerPlayMode::Fastfoward) { player->forward(); }
			else { player->play(); }

			playerList.push_back(player);
		}
		else
		{
			std::cerr << "ERROR: impossible to initialize player for scenario " << scenarioName << std::endl;
			returnCode = OpenViBE::EPlayerReturnCode::KernelInternalFailure;
			break;
		}
	}

	if (returnCode == OpenViBE::EPlayerReturnCode::Success)
	{
		// loop until timeout
		const uint64_t startTime    = System::Time::zgetTime();
		uint64_t lastLoopTime = startTime;

		// cannot directly feed secondsToTime with parameters.m_MaximumExecutionTime
		// because it could overflow
		const double boundedMaxExecutionTimeInS = TimeArithmetics::timeToSeconds(std::numeric_limits<uint64_t>::max());

		uint64_t maxExecutionTimeInFixedPoint;
		if (command.maximumExecutionTime &&
			command.maximumExecutionTime.get() > 0 &&
			command.maximumExecutionTime.get() < boundedMaxExecutionTimeInS) { maxExecutionTimeInFixedPoint = TimeArithmetics::secondsToTime(command.maximumExecutionTime.get()); }
		else { maxExecutionTimeInFixedPoint = std::numeric_limits<uint64_t>::max(); }

		bool allStopped{ false };
		while (!allStopped) // negative condition here because it is easier to reason about it
		{
			const uint64_t currentTime = System::Time::zgetTime();
			allStopped           = true;
			for (auto p : playerList)
			{
				if (p->getStatus() != PlayerStatus_Stop) { if (!p->loop(currentTime - lastLoopTime, maxExecutionTimeInFixedPoint)) { returnCode = OpenViBE::EPlayerReturnCode::KernelInternalFailure; } }

				if (p->getCurrentSimulatedTime() >= maxExecutionTimeInFixedPoint) { p->stop(); }

				allStopped &= (p->getStatus() == PlayerStatus_Stop);
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

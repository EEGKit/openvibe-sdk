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

#include "ovspCCommand.h"
#include "ovspCCommandLineOptionParser.h"

namespace OpenViBE
{
	CommandLineOptionParser::CommandLineOptionParser(ProgramOptionParser& parser)
		: m_OptionParser(parser) { }

	void CommandLineOptionParser::initialize()
	{
		// nothing to do
	}

	void CommandLineOptionParser::uninitialize() { m_CommandList.clear(); }

	std::vector<std::shared_ptr<SCommand>> CommandLineOptionParser::getCommandList() const { return m_CommandList; }

	EPlayerReturnCode CommandLineOptionParser::parse()
	{
		// parsing consists of building a straightfoward command workflow according to command-line
		// options.

		// commands have to be pushed in the right order

		// choose a dumb name for the scenario
		std::string scenarioName = "express-scenario";

		// an init command is always needed
		m_CommandList.push_back(std::make_shared<InitCommand>());


		// workflow must at least load the kernel
		std::shared_ptr<LoadKernelCommand> kernelCmd = std::make_shared<LoadKernelCommand>();

		if (m_OptionParser.hasOption("config-file")) // optional
		{
			kernelCmd->configurationFile = m_OptionParser.getOptionValue<std::string>("config-file");
		}

		m_CommandList.push_back(kernelCmd);

		// scenario loading is a mandatory step
		std::shared_ptr<LoadScenarioCommand> scenarioCmd = std::make_shared<LoadScenarioCommand>();

		if (m_OptionParser.hasOption("scenario-file")) // mandatory option
		{
			scenarioCmd->scenarioFile = m_OptionParser.getOptionValue<std::string>("scenario-file");

			// set dumb name as it used to recognize scenario in the application
			scenarioCmd->scenarioName = scenarioName;
		}
		else
		{
			std::cerr << "ERROR: mandatory option 'scenario-file' not set" << std::endl;
			return EPlayerReturnCode::MissingMandatoryArgument;
		}

		m_CommandList.push_back(scenarioCmd);

		
		// scenario update option
		std::shared_ptr<UpdateScenarioCommand> updateScenarioCmd = std::make_shared<UpdateScenarioCommand>();

		if (m_OptionParser.hasOption("updated-scenario-file"))
		{
			// do not play scenario, just update it.
			updateScenarioCmd->scenarioFile = m_OptionParser.getOptionValue<std::string>("updated-scenario-file");			
			
			// set dumb name as it used to recognize scenario in the application
			updateScenarioCmd->scenarioName = scenarioName;

			m_CommandList.push_back(updateScenarioCmd);
		}
		else
		{
			// check if some scenario setup information has been set
			if (m_OptionParser.hasOption("ds"))
			{
				std::shared_ptr<SetupScenarioCommand> setupCmd = std::make_shared<SetupScenarioCommand>();
				setupCmd->scenarioName                         = scenarioName;
				setupCmd->tokenList                            = m_OptionParser.getOptionValue<std::vector<SetupScenarioCommand::Token>>("ds");

				m_CommandList.push_back(setupCmd);
			}
			
			// last command in the workflow is the run command
			std::shared_ptr<RunScenarioCommand> runCmd = std::make_shared<RunScenarioCommand>();
			runCmd->scenarioList                       = std::vector<std::string>{ scenarioName };

			if (m_OptionParser.hasOption("play-mode"))
			{
				const auto playMode = m_OptionParser.getOptionValue<std::string>("play-mode");

				if (playMode != "ff" && playMode != "std")
				{
					std::cerr << "ERROR: option 'play-mode' must be ff or std" << std::endl;
					return EPlayerReturnCode::BadArg;
				}
				
				// permissive code here
				// any other entry than ff leads to standard mode...
				runCmd->playMode = ((playMode == "ff") ? PlayerPlayMode::Fastfoward : PlayerPlayMode::Standard);
			}

			if (m_OptionParser.hasOption("max-time")) { runCmd->maximumExecutionTime = m_OptionParser.getOptionValue<double>("max-time"); }

			if (m_OptionParser.hasOption("dg")) { runCmd->tokenList = m_OptionParser.getOptionValue<std::vector<SetupScenarioCommand::Token>>("dg"); }

			m_CommandList.push_back(runCmd);
		}

		return EPlayerReturnCode::Success;
	}
}	// namespace OpenViBE

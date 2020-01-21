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
	CommandLineOptionParser::CommandLineOptionParser(ProgramOptionParser& parser) : m_parser(parser) { }

	void CommandLineOptionParser::initialize() { }	// nothing to do
	void CommandLineOptionParser::uninitialize() { m_cmdList.clear(); }

	std::vector<std::shared_ptr<SCommand>> CommandLineOptionParser::getCommandList() const { return m_cmdList; }

	EPlayerReturnCode CommandLineOptionParser::parse()
	{
		// parsing consists of building a straightfoward command workflow according to command-line
		// options.

		// commands have to be pushed in the right order

		// choose a dumb name for the scenario
		std::string scenarioName = "express-scenario";

		// an init command is always needed
		m_cmdList.push_back(std::make_shared<SInitCmd>());


		// workflow must at least load the kernel
		std::shared_ptr<SLoadKernelCmd> kernelCmd = std::make_shared<SLoadKernelCmd>();

		if (m_parser.hasOption("config-file")) // optional
		{
			kernelCmd->configFile = m_parser.getOptionValue<std::string>("config-file");
		}

		m_cmdList.push_back(kernelCmd);

		// scenario loading is a mandatory step
		std::shared_ptr<SLoadScenarioCmd> scenarioCmd = std::make_shared<SLoadScenarioCmd>();

		if (m_parser.hasOption("scenario-file")) // mandatory option
		{
			scenarioCmd->scenarioFile = m_parser.getOptionValue<std::string>("scenario-file");

			// set dumb name as it used to recognize scenario in the application
			scenarioCmd->scenarioName = scenarioName;
		}
		else
		{
			std::cerr << "ERROR: mandatory option 'scenario-file' not set" << std::endl;
			return EPlayerReturnCode::MissingMandatoryArgument;
		}

		m_cmdList.push_back(scenarioCmd);

		
		// scenario update option
		std::shared_ptr<SUpdateScenarioCmd> updateScenarioCmd = std::make_shared<SUpdateScenarioCmd>();

		if (m_parser.hasOption("updated-scenario-file"))
		{
			// do not play scenario, just update it.
			updateScenarioCmd->scenarioFile = m_parser.getOptionValue<std::string>("updated-scenario-file");			
			
			// set dumb name as it used to recognize scenario in the application
			updateScenarioCmd->scenarioName = scenarioName;

			m_cmdList.push_back(updateScenarioCmd);
		}
		else
		{
			// check if some scenario setup information has been set
			if (m_parser.hasOption("ds"))
			{
				std::shared_ptr<SSetupScenarioCmd> setupCmd = std::make_shared<SSetupScenarioCmd>();
				setupCmd->scenarioName                      = scenarioName;
				setupCmd->tokenList                         = m_parser.getOptionValue<std::vector<SSetupScenarioCmd::Token>>("ds");

				m_cmdList.push_back(setupCmd);
			}
			
			// last command in the workflow is the run command
			std::shared_ptr<SRunScenarioCmd> runCmd = std::make_shared<SRunScenarioCmd>();
			runCmd->scenarioList                    = std::vector<std::string>{ scenarioName };

			if (m_parser.hasOption("play-mode"))
			{
				const auto playMode = m_parser.getOptionValue<std::string>("play-mode");

				if (playMode != "ff" && playMode != "std")
				{
					std::cerr << "ERROR: option 'play-mode' must be ff or std" << std::endl;
					return EPlayerReturnCode::BadArg;
				}
				
				// permissive code here
				// any other entry than ff leads to standard mode...
				runCmd->playMode = ((playMode == "ff") ? EPlayerPlayMode::Fastfoward : EPlayerPlayMode::Standard);
			}

			if (m_parser.hasOption("max-time")) { runCmd->maximumExecutionTime = m_parser.getOptionValue<double>("max-time"); }
			if (m_parser.hasOption("dg")) { runCmd->tokenList = m_parser.getOptionValue<std::vector<SSetupScenarioCmd::Token>>("dg"); }

			m_cmdList.push_back(runCmd);
		}

		return EPlayerReturnCode::Success;
	}
}	// namespace OpenViBE

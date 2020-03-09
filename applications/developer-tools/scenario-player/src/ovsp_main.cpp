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

#include <string>
#include <vector>
#include <memory>

#include "ovsp_defines.h"
#include "ovspCKernelFacade.h"
#include "ovspCCommand.h"
#include "ovspCCommandLineOptionParser.h"
#include "ovspCCommandFileParser.h"

using namespace OpenViBE;

void initializeParser(ProgramOptionParser& parser)
{
	const std::string desc =
			R"d(Usage: program options

Program can be run in express mode to directly execute a scenario
Program can be run in command mode to execute list of commands from a file

)d";
	parser.setGlobalDesc(desc);

	parser.addSimpleOption("help", { "h", "Help" });
	parser.addSimpleOption("version", { "v", "Program version" });

	parser.addValueOption<SProgramOptionsTraits::String>("mode", { "m", "Execution mode: 'x' for express, 'c' for command [mandatory]" });

	// express mode options
	parser.addValueOption<SProgramOptionsTraits::String>("config-file", { "", "Path to configuration file (express mode only)" });
	parser.addValueOption<SProgramOptionsTraits::String>("scenario-file", { "", "Path to scenario file (express mode only) [mandatory]" });
	parser.addValueOption<SProgramOptionsTraits::String>("updated-scenario-file", {
															 "",
															 "Enable update process instead of playing scenario. Path to the updated scenario file (express mode only)."
														 });
	parser.addValueOption<SProgramOptionsTraits::String>("play-mode", {
															 "", "Play mode: std for standard and ff for fast-foward (express mode only) [default=std]"
														 });
	parser.addValueOption<SProgramOptionsTraits::Float>("max-time", { "", "Scenarios playing execution time limit (express mode only)" });
	parser.addValueOption<SProgramOptionsTraits::TokenPairList>("dg", { "", "Global user-defined token: -dg=\"(token:value)\" (express mode only)" });
	parser.addValueOption<SProgramOptionsTraits::TokenPairList>("ds", { "", "Scenario user-defined token: -ds=\"(token:value)\" (express mode only)" });

	// command mode options
	parser.addValueOption<SProgramOptionsTraits::String>("command-file", { "", "Path to command file (command mode only) [mandatory]" });
}

int main(int argc, char** argv)
{
	ProgramOptionParser optionParser;
	initializeParser(optionParser);

	if (!optionParser.parse(argc, argv))
	{
		std::cerr << "ERROR: Failed to parse arguments" << std::endl;
		return int(EPlayerReturnCodes::InvalidArg);
	}
	if (optionParser.hasOption("help"))
	{
		optionParser.printOptionsDesc();
		return int(EPlayerReturnCodes::Success);
	}
	if (optionParser.hasOption("version"))
	{
		// PROJECT_VERSION is added to definition from cmake
		std::cout << "version: " << PROJECT_VERSION << std::endl;
		return int(EPlayerReturnCodes::Success);
	}
	if (optionParser.hasOption("mode") || optionParser.hasOption("updated-scenario-file"))
	{
		// command parser type is selected from mode
		std::unique_ptr<ICommandParser> commandParser{ nullptr };
		const auto mode = optionParser.getOptionValue<SProgramOptionsTraits::String>("mode");

		if (mode == "c")
		{
			// check for the mandatory commad file
			if (optionParser.hasOption("command-file"))
			{
				commandParser.reset(new CommandFileParser(optionParser.getOptionValue<std::string>("command-file")));
			}
			else
			{
				std::cerr << "ERROR: mandatory option 'command-file' not set" << std::endl;
				return int(EPlayerReturnCodes::MissingMandatoryArgument);
			}
		}
		else if ((mode == "x") || optionParser.hasOption("updated-scenario-file")) { commandParser.reset(new CommandLineOptionParser(optionParser)); }
		else
		{
			std::cerr << "ERROR: unknown mode set" << std::endl;
			std::cerr << "Mode must be 'x' or 'c'" << std::endl;
			return int(EPlayerReturnCodes::InvalidArg);
		}

		commandParser->initialize();

		try
		{
			auto returnCode = commandParser->parse();

			if (returnCode == EPlayerReturnCodes::Success)
			{
				CKernelFacade kernel;

				for (auto& cmd : commandParser->getCommandList())
				{
					returnCode = cmd->execute(kernel);
					if (returnCode != EPlayerReturnCodes::Success) { return int(returnCode); }
				}
			}
			else { return int(returnCode); }
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: received unexpected exception: " << e.what() << std::endl;
			return int(EPlayerReturnCodes::UnkownFailure);
		}
	}
	else
	{
		std::cerr << "ERROR: mandatory option 'mode' not set" << std::endl;
		return int(EPlayerReturnCodes::MissingMandatoryArgument);
	}

	return int(EPlayerReturnCodes::Success);
}

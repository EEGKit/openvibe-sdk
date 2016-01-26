#include <iostream>
#include <string>
#include <vector>

#include "cvsp_defines.h"
#include "cvProgramOptions.hpp"

using namespace CertiViBE;
using OptionsParser = ProgramOptions<ProgramOptionsTraits::String, ProgramOptionsTraits::TokenPairList>;


void initializeParser(OptionsParser& parser)
{
	std::string desc = R"d(Usage: program options

Program can be run in express mode to directly execute a scenario
Program can be run in command mode to execute list of commands from a file
Program can be run in interactive mode to set the list of commands to execute interactively

)d";
	parser.setGlobalDesc(desc);

	parser.addSimpleOption("help", { "h", "Help" });
	parser.addSimpleOption("version", { "v", "Program version" });

	parser.addValueOption<ProgramOptionsTraits::String>("mode", { "m", "Execution mode: 'x' for express, c for command, i for interactive" });

	// express mode options
	parser.addValueOption<ProgramOptionsTraits::String>("config-file", { "", "Path to configuration file (express mode only)" });
	parser.addValueOption<ProgramOptionsTraits::String>("play-file", { "", "Path to scenario file (express mode only)" });
	parser.addValueOption<ProgramOptionsTraits::String>("play-mode", { "", "std for standard and ff for fast-foward (express mode only)" });

	parser.addValueOption<ProgramOptionsTraits::TokenPairList>("dg", { "", "global user-defined token" });
	parser.addValueOption<ProgramOptionsTraits::TokenPairList>("ds", { "", "scenario user-defined token" });

	// command mode options
	parser.addValueOption<ProgramOptionsTraits::String>("command-file", { "", "Path to command file (command mode only)" });
}

int main(int argc, char** argv)
{
	PlayerReturnCode returnCode = PlayerReturnCode::Sucess;

	OptionsParser parser;
	initializeParser(parser);

	if (!parser.parse(argc, argv))
	{
		std::cout << "ERROR: Failed to parse arguments" << std::endl;
		returnCode = PlayerReturnCode::InvalidArg;
	}
	else
	{
		if (parser.hasOption("help"))
		{
			parser.printOptionsDesc();
			returnCode = PlayerReturnCode::Sucess;
		}
		else if (parser.hasOption("version"))
		{
			// PROJECT_VERSION is added to definition from cmake
			std::cout << "version: " << PROJECT_VERSION << std::endl;
			returnCode = PlayerReturnCode::Sucess;
		}
		else
		{
			if (parser.hasOption("mode"))
			{
				std::cout << "mode = " << parser.getOptionValue<ProgramOptionsTraits::String>("mode") << std::endl;
			}

			if (parser.hasOption("config-file"))
			{
				std::cout << "config-file = " << parser.getOptionValue<ProgramOptionsTraits::String>("config-file") << std::endl;
			}

			if (parser.hasOption("play-file"))
			{
				std::cout << "play-file = " << parser.getOptionValue<ProgramOptionsTraits::String>("play-file") << std::endl;
			}

			if (parser.hasOption("play-mode"))
			{
				std::cout << "play-mode = " << parser.getOptionValue<ProgramOptionsTraits::String>("play-mode") << std::endl;
			}

			if (parser.hasOption("dg"))
			{
				for (auto& token : parser.getOptionValue<ProgramOptionsTraits::TokenPairList>("dg"))
				{
					std::cout << "token = (" << token.first << "," << token.second << ")" << std::endl;
				}
			}

			if (parser.hasOption("ds"))
			{
				for (auto& token : parser.getOptionValue<ProgramOptionsTraits::TokenPairList>("ds"))
				{
					std::cout << "token = (" << token.first << "," << token.second << ")" << std::endl;
				}
			}

			if (parser.hasOption("command-file"))
			{
				std::cout << "command-file = " << parser.getOptionValue<ProgramOptionsTraits::String>("command-file") << std::endl;
			}
		}
	}

	return static_cast<int>(returnCode);
}


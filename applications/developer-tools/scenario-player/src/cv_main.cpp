#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "cvsp_defines.h"
#include "cvKernelFacade.h"
#include "cvCommandInterface.h"
#include "cvCommandLineOptionParser.h"

using namespace CertiViBE;

void initializeParser(ProgramOptionParser& optionParser)
{
	std::string desc = R"d(Usage: program options

Program can be run in express mode to directly execute a scenario
Program can be run in command mode to execute list of commands from a file
Program can be run in interactive mode to set the list of commands to execute interactively

)d";
	optionParser.setGlobalDesc(desc);

	optionParser.addSimpleOption("help", { "h", "Help" });
	optionParser.addSimpleOption("version", { "v", "Program version" });

	optionParser.addValueOption<ProgramOptionsTraits::String>("mode", { "m", "Execution mode: 'x' for express, c for command, i for interactive [mandatory]" });

	// express mode options
	optionParser.addValueOption<ProgramOptionsTraits::String>("config-file", { "", "Path to configuration file (express mode only)" });
	optionParser.addValueOption<ProgramOptionsTraits::String>("play-file", { "", "Path to scenario file (express mode only) [mandatory]" });
	optionParser.addValueOption<ProgramOptionsTraits::String>("play-mode", { "", "std for standard and ff for fast-foward (express mode only)" });

	optionParser.addValueOption<ProgramOptionsTraits::TokenPairList>("dg", { "", "global user-defined token" });
	optionParser.addValueOption<ProgramOptionsTraits::TokenPairList>("ds", { "", "scenario user-defined token" });

	// command mode options
	optionParser.addValueOption<ProgramOptionsTraits::String>("command-file", { "", "Path to command file (command mode only) [mandatory]" });
}

int main(int argc, char** argv)
{
	PlayerReturnCode returnCode = PlayerReturnCode::Sucess;

	ProgramOptionParser optionParser;
	initializeParser(optionParser);

	if (!optionParser.parse(argc, argv))
	{
		std::cout << "ERROR: Failed to parse arguments" << std::endl;
		returnCode = PlayerReturnCode::InvalidArg;
	}
	else
	{
		if (optionParser.hasOption("help"))
		{
			optionParser.printOptionsDesc();
			returnCode = PlayerReturnCode::Sucess;
		}
		else if (optionParser.hasOption("version"))
		{
			// PROJECT_VERSION is added to definition from cmake
			std::cout << "version: " << PROJECT_VERSION << std::endl;
			returnCode = PlayerReturnCode::Sucess;
		}
		else
		{
		
			if (optionParser.hasOption("mode"))
			{
				// command parser type is selected from mode
				std::unique_ptr<CommandParserInterface> commandParser{ nullptr };
				auto mode = optionParser.getOptionValue<ProgramOptionsTraits::String>("mode");

				if (mode == "i")
				{
					std::cout << "Crash expected: interactive mode not yet implemented" << std::endl;
				}
				else if (mode == "c")
				{
					std::cout << "Crash expected: command mode not yet implemented" << std::endl;
				}
				else if (mode == "x")
				{
					commandParser.reset(new CommandLineOptionParser(optionParser));
				}
				else
				{
					std::cout << "ERROR: unknown mode set" << std::endl;
					std::cout << "Mode must be 'x','i' or 'c'" << std::endl;
					returnCode = PlayerReturnCode::InvalidArg;
				}

				commandParser->initialize();

				returnCode = commandParser->parse();

				if (returnCode == PlayerReturnCode::Sucess)
				{
					KernelFacade kernel;

					for (auto& cmd : commandParser->getCommandList())
					{
						returnCode = cmd->execute(kernel);

						if (returnCode != PlayerReturnCode::Sucess)
						{
							break;
						}
					}
				}
			}
			else
			{
				std::cout << "ERROR: mandatory option 'mode' not set" << std::endl;
				returnCode = PlayerReturnCode::MissingMandatoryArg;
			}
		}
	}

	return static_cast<int>(returnCode);
}


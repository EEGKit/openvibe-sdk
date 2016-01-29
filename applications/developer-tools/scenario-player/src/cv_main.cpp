#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "cvsp_defines.h"
#include "cvKernelFacade.h"
#include "cvCommandInterface.h"
#include "cvCommandLineOptionParser.h"
#include "cvCommandFileParser.h"

using namespace CertiViBE;

void initializeParser(ProgramOptionParser& optionParser)
{
	std::string desc = R"d(Usage: program options

Program can be run in express mode to directly execute a scenario
Program can be run in command mode to execute list of commands from a file

)d";
	optionParser.setGlobalDesc(desc);

	optionParser.addSimpleOption("help", { "h", "Help" });
	optionParser.addSimpleOption("version", { "v", "Program version" });

	optionParser.addValueOption<ProgramOptionsTraits::String>("mode", { "m", "Execution mode: 'x' for express, 'c' for command [mandatory]" });

	// express mode options
	optionParser.addValueOption<ProgramOptionsTraits::String>("config-file", { "", "Path to configuration file (express mode only)" });
	optionParser.addValueOption<ProgramOptionsTraits::String>("play-file", { "", "Path to scenario file (express mode only) [mandatory]" });
	optionParser.addValueOption<ProgramOptionsTraits::String>("play-mode", { "", "Play mode: std for standard and ff for fast-foward (express mode only)" });

	optionParser.addValueOption<ProgramOptionsTraits::TokenPairList>("dg", { "", "Global user-defined token: -dg=\"(token:value)\" (express mode only)"});
	optionParser.addValueOption<ProgramOptionsTraits::TokenPairList>("ds", { "", "Scenario user-defined token: -ds=\"(token:value)\" (express mode only)" });

	// command mode options
	optionParser.addValueOption<ProgramOptionsTraits::String>("command-file", { "", "Path to command file (command mode only) [mandatory]" });
}

int main(int argc, char** argv)
{
	ProgramOptionParser optionParser;
	initializeParser(optionParser);

	if (!optionParser.parse(argc, argv))
	{
		std::cerr << "ERROR: Failed to parse arguments" << std::endl;
		return static_cast<int>(PlayerReturnCode::InvalidArg);
	}
	else
	{
		if (optionParser.hasOption("help"))
		{
			optionParser.printOptionsDesc();
			return static_cast<int>(PlayerReturnCode::Success);
		}
		else if (optionParser.hasOption("version"))
		{
			// PROJECT_VERSION is added to definition from cmake
			std::cout << "version: " << PROJECT_VERSION << std::endl;
			return static_cast<int>(PlayerReturnCode::Success);
		}
		else
		{
		
			if (optionParser.hasOption("mode"))
			{
				// command parser type is selected from mode
				std::unique_ptr<CommandParserInterface> commandParser{ nullptr };
				auto mode = optionParser.getOptionValue<ProgramOptionsTraits::String>("mode");

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
						return static_cast<int>(PlayerReturnCode::MissingMandatoryArg);
					}
				}
				else if (mode == "x")
				{
					commandParser.reset(new CommandLineOptionParser(optionParser));
				}
				else
				{
					std::cerr << "ERROR: unknown mode set" << std::endl;
					std::cerr << "Mode must be 'x' or 'c'" << std::endl;
					return static_cast<int>(PlayerReturnCode::InvalidArg);
				}

				commandParser->initialize();

				try {

					auto returnCode = commandParser->parse();

					if (returnCode == PlayerReturnCode::Success)
					{
						KernelFacade kernel;

						for (auto& cmd : commandParser->getCommandList())
						{
							returnCode = cmd->execute(kernel);

							if (returnCode != PlayerReturnCode::Success)
							{
								return static_cast<int>(returnCode);
							}
						}
					}
				}
				catch (const std::exception& e)
				{
					std::cerr << "ERROR: received unexpected exception: " << e.what() << std::endl;
					return static_cast<int>(PlayerReturnCode::UnkownFailure);
				}
			}
			else
			{
				std::cerr << "ERROR: mandatory option 'mode' not set" << std::endl;
				return static_cast<int>(PlayerReturnCode::MissingMandatoryArg);
			}
		}
	}

	return static_cast<int>(PlayerReturnCode::Success);
}


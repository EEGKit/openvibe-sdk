
#include "cvInitCommand.h"
#include "cvLoadKernelCommand.h"
#include "cvLoadScenarioCommand.h"
#include "cvSetupScenarioCommand.h"
#include "cvRunScenarioCommand.h"
#include "cvCommandLineOptionParser.h"

namespace CertiViBE
{
	CommandLineOptionParser::CommandLineOptionParser(ProgramOptionParser& parser) 
		: m_OptionParser(parser)
	{
	}

	void CommandLineOptionParser::initialize()
	{
		// nothing to do
	}

	void CommandLineOptionParser::uninitialize()
	{
		m_CommandList.clear();
	}

	std::vector<std::shared_ptr<CommandInterface>> CommandLineOptionParser::getCommandList() const
	{
		return m_CommandList;
	}

	PlayerReturnCode CommandLineOptionParser::parse()
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
			kernelCmd->setConfigurationFile(m_OptionParser.getOptionValue<std::string>("config-file"));
		}

		m_CommandList.push_back(kernelCmd);

		// scenario loading is a mandatory step
		std::shared_ptr<LoadScenarioCommand> scenarioCmd = std::make_shared<LoadScenarioCommand>();

		if (m_OptionParser.hasOption("scenario-file")) // mandatory option
		{
			scenarioCmd->setScenarioFile(m_OptionParser.getOptionValue<std::string>("scenario-file"));

			// set dumb name as it used to recognize scenario in the application
			scenarioCmd->setScenarioName(scenarioName);
		}
		else
		{
			std::cerr << "ERROR: mandatory option 'scenario-file' not set" << std::endl;
			return PlayerReturnCode::MissingMandatoryArg;
		}

		m_CommandList.push_back(scenarioCmd);

		// check if some scenario setup information has been set
		if (m_OptionParser.hasOption("ds"))
		{
			std::shared_ptr<SetupScenarioCommand> setupCmd = std::make_shared<SetupScenarioCommand>();
			setupCmd->setScenarioName(scenarioName);
			setupCmd->setTokenList(m_OptionParser.getOptionValue<std::vector<SetupScenarioCommand::Token>>("ds"));

			m_CommandList.push_back(setupCmd);
		}

		// last command in the workflow is the run command
		std::shared_ptr<RunScenarioCommand> runCmd = std::make_shared<RunScenarioCommand>();
		runCmd->setScenarioList({ scenarioName });
		
		if (m_OptionParser.hasOption("play-mode"))
		{
			auto playMode = m_OptionParser.getOptionValue<std::string>("play-mode");

			if (playMode != "ff" && playMode != "std")
			{
				std::cerr << "ERROR: option 'play-mode' must be ff or std" << std::endl;
				return PlayerReturnCode::BadArg;
			}

			// permissive code here
			// any other entry than ff leads to standard mode...
			runCmd->setPlayMode((playMode == "ff") ? PlayMode::Fastfoward : PlayMode::Standard);
		}

		if (m_OptionParser.hasOption("max-time"))
		{
			runCmd->setMaximumExecutionTime(m_OptionParser.getOptionValue<double>("max-time"));
		}

		if (m_OptionParser.hasOption("dg"))
		{
			runCmd->setTokenList(m_OptionParser.getOptionValue<std::vector<SetupScenarioCommand::Token>>("dg"));
		}

		m_CommandList.push_back(runCmd);

		return PlayerReturnCode::Success;
	}
}

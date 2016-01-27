#pragma once

#include "cvCommandParserInterface.h"
#include "cvsp_defines.h"

namespace CertiViBE
{
	/**
	* \class CommandLineOptionParser
	* \author cgarraud (INRIA)
	* \date 2016-01-27
	* \brief Parser implementation that parses command from command-line arguments
	* \ingroup ScenarioPlayer
	*
	* The current implementation retrieves the options from a ProgramOptions parser and
	* simply builds the commands from the parsed options.
	*
	*/
	class CommandLineOptionParser : public CommandParserInterface
	{

	public:

		explicit CommandLineOptionParser(ProgramOptionParser& parser);

		virtual void initialize() override;

		virtual void uninitialize() override;

		virtual std::vector<std::shared_ptr<CommandInterface>> getCommandList() const override;

		virtual PlayerReturnCode parse() override;

	private:

		ProgramOptionParser& m_OptionParser;
		std::vector<std::shared_ptr<CommandInterface>> m_CommandList;
	};
}
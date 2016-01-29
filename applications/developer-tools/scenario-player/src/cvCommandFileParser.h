#pragma once

#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <map>

#include "cvCommandParserInterface.h"
#include "cvsp_defines.h"

namespace CertiViBE
{
	/**
	* \class CommandFileParser
	* \author cgarraud (INRIA)
	* \date 2016-01-27
	* \brief Parser implementation that parses command a file
	* \ingroup ScenarioPlayer
	*
	* The current implementation retrieves the list of commands from a file.
	* 
	* Input file format requirements:
	* - Command names are defined within brackets [InitCommand] on a single line
	* - Each command parameter is defined on a single line below the command name:
	*  + Single value parameters are defined as: parameterName: value
	*  + Simple list parameters are defined as: parameterName: {val1,val2,...}
	*  + Token list parameters are defined as: parameterName: { (token1:val1),(token2:val2) } (no ',' authorized in (token, value) pair)
	*  .
	* .
	*
	* \todo The implementation uses a lot of string utility functions that need more
	* testing
	*
	* \note Use of regex would simplify the implementation but boost::regex is not header-only and std::regex not implemented in gcc 4.8
	*
	*/
	class CommandFileParser : public CommandParserInterface
	{

	public:

		/**
		*
		* \brief Constructor
		* \param[in] commandFile path to the command file
		*
		*/
		explicit CommandFileParser(const std::string& commandFile);

		virtual void initialize() override;

		virtual void uninitialize() override;

		virtual std::vector<std::shared_ptr<CommandInterface>> getCommandList() const override;

		virtual PlayerReturnCode parse() override;

	private:

		using CallbackType = std::function<PlayerReturnCode(const std::vector<std::string>)>;
		using Token = std::pair<std::string, std::string>;

		static std::string trim(const std::string& str);
		static std::pair<std::string, std::string> tokenize(const std::string& str);
		static std::vector<std::string> split(const std::string& str, char delimiter);
		static bool toBool(const std::string& str);
		static std::vector<std::string> toList(const std::string& str);
		static std::vector<Token> toTokenList(const std::string& str);

		PlayerReturnCode flush(const std::string& sectionTag, const std::vector<std::string>& sectionContent);

		PlayerReturnCode initCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode resetCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode loadKernelCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode loadScenarioCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode setupScenarioCommandCb(const std::vector<std::string>& sectionContent);
		PlayerReturnCode runScenarioCommandCb(const std::vector<std::string>& sectionContent);

		std::string m_CommandFile;
		std::vector<std::shared_ptr<CommandInterface>> m_CommandList;
		std::map<std::string, CallbackType> m_CallbackList;
	};
}
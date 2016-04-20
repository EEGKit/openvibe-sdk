/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
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
#include "ovspCCommandFileParser.h"

namespace OpenViBE
{
	using namespace std::placeholders;

	// should be moved in a utility class/file
	std::string CommandFileParser::trim(const std::string& str)
	{
		// remove leading and trailing space
		// no use of regex here cause it might be too slow
		auto begin = str.find_first_not_of(" \t");
		auto end = str.find_last_not_of(" \t");

		if (begin == std::string::npos)
		{
			begin = 0;
		}

		if (end == std::string::npos)
		{
			end = str.size() - 1;
		}

		return str.substr(begin, end - begin + 1);
	}

	// should be moved in a utility class/file
	std::pair<std::string, std::string> CommandFileParser::tokenize(const std::string& str)
	{
		// given a string of type "token : value", return a pair of (token,value)
		// split method is not used because only the first delimiter is of interest
		auto split = str.find_first_of(":");

		if (split == std::string::npos)
		{
			throw std::runtime_error("Impossible to convert " + str + " to token/value pair");
		}

		auto token = trim(str.substr(0, split));
		auto val = trim(str.substr(split + 1, str.size() - split - 1));

		return std::make_pair(token, val);
	}

	// should be moved in a utility class/file
	std::vector<std::string> CommandFileParser::split(const std::string& str, char delimiter)
	{
		std::vector<std::string> vec;
		std::size_t currentIndex = 0;
		std::size_t delimiterIndex = str.find(delimiter);

		auto trimmed = trim(str.substr(currentIndex, delimiterIndex));

		if (!trimmed.empty())
		{
			vec.push_back(trimmed);
		}

		while (delimiterIndex != std::string::npos)
		{
			// abc,cde,fgh
			//     ^       -> current index points to the first element after a match
			//        ^    -> delimiter index points to next match
			currentIndex = delimiterIndex + 1;
			delimiterIndex = str.find(delimiter, currentIndex);

			trimmed = trim(str.substr(currentIndex, delimiterIndex - currentIndex));

			if (!trimmed.empty())
			{
				vec.push_back(trimmed);
			}
		}

		return vec;
	}

	// should be moved in a utility class/file
	bool CommandFileParser::toBool(const std::string& str)
	{
		// try to keep same behavior as stoi or stol

		std::string lowerStr;
		lowerStr.reserve(str.size());
		std::transform(str.begin(), str.end(), lowerStr.begin(), ::tolower);

		bool result;
		if (str == "false" || str == "0")
		{
			result = false;
		}
		else if (str == "true" || str == "1")
		{
			result = true;
		}
		else
		{
			throw std::runtime_error("Impossible to convert " + str + " to bool");
		}

		return result;
	}

	// should be moved in a utility class/file
	// input is expected to be trimmed (by a call to tolenize for example)
	std::vector<std::string> CommandFileParser::toList(const std::string& str)
	{
		std::vector<std::string> vec;
		// {token1, token2, token3 ...} pattern expected
		if (str.size() >= 3 && str[0] == '{' && str[str.size() - 1] == '}') 
		{
			vec = split(str.substr(1, str.length() - 2), ',');
		}
		else
		{
			throw std::runtime_error("Impossible to convert " + str + " to list");
		}

		return vec;
	}

	// should be moved in a utility class/file
	// part of this function duplicates code from ProgramOptions.hpp
	std::vector<CommandFileParser::Token> CommandFileParser::toTokenList(const std::string& str)
	{
		std::vector<Token> vec;

		for (auto& rawToken : toList(str))
		{
			// rawToken is expected to be trimmed

			auto split = rawToken.find_first_of(":");
			auto size = rawToken.size();

			// (a:b) pattern expected
			// minimal regex std::regex("\\(.+:.+\\)")
			if (!(size >= 5 && rawToken[0] == '(' && rawToken[size - 1] == ')') ||
				split == std::string::npos) 
			{
				throw std::runtime_error("Failed to parse token pair from value: " + rawToken);
			}

			Token token;
			token.first = trim(rawToken.substr(1, split - 1));

			// magic 2 numbers is because substr takes a length as second parameter
			// 2 = remove the last ) + account for the first one
			token.second = trim(rawToken.substr(split + 1, size - split - 2));

			vec.push_back(token);
		}

		return vec;
	}

	CommandFileParser::CommandFileParser(const std::string& file)
		: m_CommandFile(file)
	{
	}

	void CommandFileParser::initialize()
	{
		// using a callback mechanism allows us to implement the core parse() method
		// very easily (no need to put some if/else blocks everywhere depending on which command is encountered)
		m_CallbackList["Init"] = std::bind(&CommandFileParser::initCommandCb, this, _1);
		m_CallbackList["Reset"] = std::bind(&CommandFileParser::resetCommandCb, this, _1);
		m_CallbackList["LoadKernel"] = std::bind(&CommandFileParser::loadKernelCommandCb, this, _1);
		m_CallbackList["LoadScenario"] = std::bind(&CommandFileParser::loadScenarioCommandCb, this, _1);
		m_CallbackList["SetupScenario"] = std::bind(&CommandFileParser::setupScenarioCommandCb, this, _1);
		m_CallbackList["RunScenario"] = std::bind(&CommandFileParser::runScenarioCommandCb, this, _1);
	}

	void CommandFileParser::uninitialize()
	{
		m_CallbackList.clear();
		m_CommandList.clear();
	}

	std::vector<std::shared_ptr<ICommand>> CommandFileParser::getCommandList() const
	{
		return m_CommandList;
	}

	PlayerReturnCode CommandFileParser::parse()
	{
		std::ifstream fileStream(m_CommandFile);

		if (!fileStream.is_open())
		{
			std::cerr << "ERROR: impossible to open file at location: " << m_CommandFile << std::endl;
			return PlayerReturnCode::OpeningFileFailure;
		}

		std::string line;
		bool isFillingSection{ false };
		std::vector<std::string> sectionContent;
		std::string sectionTag;

		while (std::getline(fileStream, line))
		{
			auto trimmedLine = trim(line);
			auto size = trimmedLine.size();

			// [a] pattern expected
			// minimal regex std::regex("^(?!\\#)\\[.+\\])")
			if (size >= 3 && trimmedLine[0] == '['
				&& trimmedLine[size - 1] == ']') 
			{
				if (isFillingSection) // flush the section that was beeing filled
				{
					auto errorCode = this->flush(sectionTag, sectionContent);

					if (errorCode != PlayerReturnCode::Success)
					{
						return errorCode;
					}
				}

				// use of regex to be confident on tag structure
				// magic 2 numbers is because substr takes length as second parameter
				// 2 = remove the last ] + account for the first one
				sectionTag = trimmedLine.substr(1, size - 2);

				if (m_CallbackList.find(sectionTag) == m_CallbackList.end())
				{
					std::cerr << "ERROR: Unknown command = " << sectionTag << std::endl;
					return PlayerReturnCode::ParsingCommandFailure;
				}

				isFillingSection = true;
				sectionContent.clear();
			}
			else
			{
				sectionContent.push_back(trimmedLine);
			}
		}

		if (isFillingSection)
		{
			auto errorCode = this->flush(sectionTag, sectionContent);

			if (errorCode != PlayerReturnCode::Success)
			{
				return errorCode;
			}
		}

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::flush(const std::string& sectionTag, const std::vector<std::string>& sectionContent)
	{

		try // try block here as some conversions are made with the stl in the callback and might throw
		{
			auto returnCode = m_CallbackList[sectionTag](sectionContent);

			if (returnCode != PlayerReturnCode::Success)
			{
				return  returnCode;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "ERROR: Caught exception while parsing command = " << sectionTag << std::endl;
			std::cerr << "ERROR: Exception: " << e.what() << std::endl;

			return PlayerReturnCode::ParsingCommandFailure;
		}

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::initCommandCb(const std::vector<std::string>& sectionContent)
	{
		std::shared_ptr<InitCommand> command = std::make_shared<InitCommand>();

		for (auto& line : sectionContent)
		{
			// lines are expected to be trimmed
			// a:b pattern expected
			// minimal regex std::regex("^(?!\\#).+:.+")
			if (!line.empty() && line[0] != '#')
			{
				auto param = tokenize(line);

				if (param.first == "Benchmark")
				{
					command->benchmark = toBool(param.second);
				}
				else
				{
					std::cout << "WARNING: Unknown parameter for Init command: " << param.first << std::endl;
				}
			}
		}

		m_CommandList.push_back(command);

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::resetCommandCb(const std::vector<std::string>& sectionContent)
	{
		std::shared_ptr<ResetCommand> command = std::make_shared<ResetCommand>();

		m_CommandList.push_back(command);

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::loadKernelCommandCb(const std::vector<std::string>& sectionContent)
	{
		std::shared_ptr<LoadKernelCommand> command = std::make_shared<LoadKernelCommand>();

		// cf. initCommandCb
		for (auto& line : sectionContent)
		{
			if (!line.empty() && line[0] != '#')
			{
				auto param = tokenize(line);

				if (param.first == "ConfigurationFile")
				{
					command->configurationFile = param.second;
				}
				else
				{
					std::cout << "WARNING: Unknown parameter for LoadKernel command: " << param.first << std::endl;
				}
			}
		}

		m_CommandList.push_back(command);

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::loadScenarioCommandCb(const std::vector<std::string>& sectionContent)
	{
		std::shared_ptr<LoadScenarioCommand> command = std::make_shared<LoadScenarioCommand>();

		// cf. initCommandCb
		for (auto& line : sectionContent)
		{
			if (!line.empty() && line[0] != '#')
			{
				auto param = tokenize(line);

				if (param.first == "ScenarioName")
				{
					command->scenarioName = param.second;
				}
				else if (param.first == "ScenarioFile")
				{
					command->scenarioFile = param.second;
				}
				else
				{
					std::cout << "WARNING: Unknown parameter for LoadScenario command: " << param.first << std::endl;
				}
			}
		}

		m_CommandList.push_back(command);

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::setupScenarioCommandCb(const std::vector<std::string>& sectionContent)
	{
		std::shared_ptr<SetupScenarioCommand> command = std::make_shared<SetupScenarioCommand>();

		// cf. initCommandCb
		for (auto& line : sectionContent)
		{
			if (!line.empty() && line[0] != '#')
			{
				auto param = tokenize(line);

				if (param.first == "ScenarioName")
				{
					command->scenarioName = param.second;
				}
				else if (param.first == "TokenList")
				{
					command->tokenList = toTokenList(param.second);
				}
				else
				{
					std::cout << "WARNING: Unknown parameter for SetupScenario command: " << param.first << std::endl;
				}
			}
		}

		m_CommandList.push_back(command);

		return PlayerReturnCode::Success;
	}

	PlayerReturnCode CommandFileParser::runScenarioCommandCb(const std::vector<std::string>& sectionContent)
	{
		std::shared_ptr<RunScenarioCommand> command = std::make_shared<RunScenarioCommand>();

		// cf. initCommandCb
		for (auto& line : sectionContent)
		{
			if (!line.empty() && line[0] != '#')
			{
				auto param = tokenize(line);

				if (param.first == "ScenarioList")
				{
					command->scenarioList = toList(param.second);
				}
				else if (param.first == "PlayMode")
				{
					command->playMode = static_cast<PlayerPlayMode>(std::stoi(param.second));
				}
				else if (param.first == "MaximumExecutionTime")
				{
					command->maximumExecutionTime = std::stod(param.second);
				}
				else if (param.first == "TokenList")
				{
					command->tokenList = (toTokenList(param.second));
				}
				else
				{
					std::cout << "WARNING: Unknown parameter for RunScenario command: " << param.first << std::endl;
				}
			}
		}

		m_CommandList.push_back(command);

		return PlayerReturnCode::Success;
	}
}

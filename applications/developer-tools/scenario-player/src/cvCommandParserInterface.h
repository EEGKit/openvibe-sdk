#pragma once

#include <vector>
#include <memory>

#include "cvsp_defines.h"

namespace CertiViBE
{
	class CommandInterface;

	/**
	* \class CommandParserInterface
	* \author cgarraud (INRIA)
	* \date 2016-01-27
	* \brief Base abstract class for command parser
	* \ingroup ScenarioPlayer
	*
	* Command parsers aim at parsing a list of commands from a specific input.
	*
	*/
	class CommandParserInterface
	{

	public:

		virtual ~CommandParserInterface() = default;
		CommandParserInterface() = default;


		/**
		* \brief Initialize parser
		*
		*/
		virtual void initialize() = 0;


		/**
		* \brief Unitialize parser
		*
		*/
		virtual void uninitialize() = 0;

		/**
		* \brief Retrieve the list of commands
		* \pre This method should be called after the parse() method
		*
		*/
		virtual std::vector<std::shared_ptr<CommandInterface>> getCommandList() const = 0;


		/**
		* \brief Retrieve the list of commands
		* \pre This method should be called after the initialize() method
		*
		*/
		virtual PlayerReturnCode parse() = 0;

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class
		CommandParserInterface(const CommandParserInterface&) = delete;
		CommandParserInterface &operator=(const CommandParserInterface&) = delete;
	};
}
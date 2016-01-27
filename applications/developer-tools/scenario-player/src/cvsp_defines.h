#pragma once

#include "cvProgramOptions.hpp"

namespace CertiViBE
{
	/**
	* \defgroup ScenarioPlayer Scenario Player
	*/

	/**
	* \class ReturnCode
	* \author cgarraud
	* \date 2016-01-25
	* \brief Scenario player list of potential return code
	* \ingroup ScenarioPlayer
	*/
	enum class PlayerReturnCode
	{
		Sucess = 0, /*!< No error during execution */
		InvalidArg, /*!< Invalid command-line options */
		MissingMandatoryArg, /*!< A mandatory argument is missing */
	};

	/* Define the common parser to be used in the application */
	//using ProgramOptionParser = ProgramOptions<ProgramOptionsTraits::String, ProgramOptionsTraits::TokenPairList>;
	typedef ProgramOptions<ProgramOptionsTraits::String, ProgramOptionsTraits::TokenPairList> ProgramOptionParser;
}
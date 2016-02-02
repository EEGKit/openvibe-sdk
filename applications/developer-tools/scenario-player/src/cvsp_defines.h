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
		Success = 0, /*!< No error during execution */
		InvalidArg, /*!< Invalid command-line options */
		MissingMandatoryArg, /*!< A mandatory argument is missing */
		BadArg, /*!< An argument is given with a wrong value */
		OpeningFileFailure, /*!< A file could not be opened */
		ParsingCommandFailure, /*!< General parsing command error*/
		UnkownFailure, /*!< Error of unknown type*/
		KernelLoadingFailure, /*!< Kernel loading failed*/
		KernelInvalidDesc, /*!< Invalid kernel descriptor*/
		KernelInvalidContext, /*!< Invalid kernel context*/
		KernelInternalFailure /*!< Generic error type for kernel internal error*/
	};

	/**
	* \class ReturnCode
	* \author cgarraud
	* \date 2016-01-25
	* \brief Way of playing a scenario
	* \ingroup ScenarioPlayer
	*/
	enum class PlayMode
	{
		Standard = 0,
		Fastfoward
	};

	/* Define the common parser to be used in the application */
	using ProgramOptionParser = ProgramOptions<ProgramOptionsTraits::String, ProgramOptionsTraits::Float, ProgramOptionsTraits::TokenPairList>;
}
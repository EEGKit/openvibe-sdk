#pragma once

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
		InvalidArg /*!< Invalid command-line options */
	};
}
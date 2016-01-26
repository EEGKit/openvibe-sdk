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
		/** No error occurred during execution */
		Sucess = 0,
		/** Invalid command line options giden to the application */
		InvalidArg
	};
}
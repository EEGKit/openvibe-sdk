#pragma once

namespace CertiViBE
{
	/**
	* \defgroup ScenarioPlayer Scenario Player
	*/

	// here we use an additional namespace 
	// to avoid collision with OpenViBE error management no yet implemented
	namespace ScenarioPlayer 
	{
		/**
		* \class ReturnCode
		* \author cgarraud
		* \date 2016-01-25
		* \brief Scenario player list of potential return code
		* \ingroup ScenarioPlayer
		*/
		enum class ReturnCode
		{
			/** No error occurred during execution */
			Sucess = 0,
			/** Invalid command line options giden to the application */
			InvalidArg
		};
	}

}
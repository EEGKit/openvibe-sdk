/*********************************************************************
* Software License Agreement (AGPL-3 License)                        *
*                                                                    *
* CertiViBE                                                          *
* Based on OpenViBE V1.1.0, Copyright (C) INRIA, 2006-2015           *
* Copyright (C) INRIA, 2015-2017,V1.0                                *
*                                                                    *
* \author Charles Garraud (INRIA)                                    *
*                                                                    *
* This program is free software: you can redistribute it and/or      *
* modify it under the terms of the GNU Affero General Public License *
* as published by the Free Software Foundation, either version 3 of  *
* the License.                                                       *
*                                                                    *
* This program is distributed in the hope that it will be useful,    *
* but WITHOUT ANY WARRANTY; without even the implied warranty of     *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
* Affero General Public License for more details.                    *
*                                                                    *
* You should have received a copy of the GNU Affero General Public   *
* License along with this program.                                   *
* If not, see <http://www.gnu.org/licenses/>.                        *
*********************************************************************/

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
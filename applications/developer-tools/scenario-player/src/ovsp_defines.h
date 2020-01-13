/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
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

#pragma once

#include "ovspTProgramOptions.hpp"

namespace OpenViBE
{
	/**
	* \defgroup ScenarioPlayer Scenario Player
	*/

	/**
	* \class EPlayerReturnCode
	* \author cgarraud
	* \date 2016-01-25
	* \brief Scenario player list of potential return code
	* \ingroup ScenarioPlayer
	*/
	enum class EPlayerReturnCode
	{
		Success = 0, /*!< No error during execution */
		InvalidArg, /*!< Invalid command-line options */
		MissingMandatoryArgument, /*!< A mandatory argument is missing */
		BadArg, /*!< An argument is given with a wrong value */
		OpeningFileFailure, /*!< A file could not be opened */
		ParsingCommandFailure, /*!< General parsing command error*/
		UnkownFailure, /*!< Error of unknown type*/
		KernelLoadingFailure, /*!< Kernel loading failed*/
		KernelInvalidDesc, /*!< Invalid kernel descriptor*/
		KernelInvalidContext, /*!< Invalid kernel context*/
		KernelInternalFailure, /*!< Generic error type for kernel internal error*/
		ScenarioNotLoaded /*!< Error triggered when an action is requested on an unknown scenario */
	};

	/**
	* \class EPlayerPlayMode
	* \author cgarraud
	* \date 2016-01-25
	* \brief Way of playing a scenario
	* \ingroup ScenarioPlayer
	*/
	enum class EPlayerPlayMode
	{
		Standard = 0,
		Fastfoward
	};

	/* Define the common parser to be used in the application */
	using ProgramOptionParser = ProgramOptions<SProgramOptionsTraits::String, SProgramOptionsTraits::Float, SProgramOptionsTraits::TokenPairList>;
}	// namespace OpenViBE

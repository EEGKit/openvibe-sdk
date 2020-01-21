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

#include "ovspICommand.h"
#include <boost/optional.hpp>
#include <string>

namespace OpenViBE
{
	/**
	* \struct SInitCmd
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the initialization of the tool
	* \ingroup ScenarioPlayer
	*
	* InitCommand class contains the following properties:
	* - Benchmark: Flag to enable benchmark on execute command (optional).
	*
	*/
	struct SInitCmd final : SCommand
	{
		// List of properties
		boost::optional<bool> benchmark;

		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:
		void doPrint(std::ostream& os) const override;
	};


	/**
	* \struct SLoadKernelCmd
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives kernel loading
	* \ingroup ScenarioPlayer
	*
	* LoadKernelCommand contains the following properties:
	* - ConfigurationFile: Path to kernel configuration file (optional)
	* .
	*
	*/
	struct SLoadKernelCmd final : SCommand
	{
		// List of properties
		boost::optional<std::string> configFile;

		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:

		void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct SLoadScenarioCmd
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives scenario loading
	* \ingroup ScenarioPlayer
	*
	* LoadScenarioCommand contains the following properties:
	* - ScenarioFile: Path to xml scenario file (mandatory)
	* - ScenarioName: Name of the scenario (mandatory)
	* .
	*
	*/
	struct SLoadScenarioCmd final : SCommand
	{
		// List of properties
		boost::optional<std::string> scenarioFile;
		boost::optional<std::string> scenarioName;

		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:

		void doPrint(std::ostream& os) const override;
	};
	
	/**
	* \struct SUpdateScenarioCmd
	* \author criou (INRIA)
	* \date 2018-04-16
	* \brief Command that drives scenario update and export
	* \ingroup ScenarioPlayer
	*
	* UpdateScenarioCommand contains the following properties:
	* - ScenarioFile: Path to xml scenario file (mandatory)
	* .
	*
	*/
	struct SUpdateScenarioCmd final : SCommand
	{
		// List of properties
		boost::optional<std::string> scenarioFile;
		boost::optional<std::string> scenarioName;

		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:

		void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct SResetCmd
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives tool reset to its initial state
	* \ingroup ScenarioPlayer
	*/
	struct SResetCmd final : SCommand
	{
		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:

		void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct SRunScenarioCmd
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the execution of a list of scenarios
	* \ingroup ScenarioPlayer
	*
	* SRunScenarioCmd contains the following properties:
	* - ScenarioList: Names of scenario that must be executed (mandatory)
	* - PlayMode: 0 for standard, 1 for fastforward (optional)
	* - MaximumExecutionTime: Scenarios playing execution time limit (optional)
	* - TokenList: List of global (token,value) pairs (optional)
	* .
	*
	*/
	struct SRunScenarioCmd final : SCommand
	{
		using Token = std::pair<std::string, std::string>;

		// List of properties
		boost::optional<std::vector<std::string>> scenarioList;
		boost::optional<EPlayerPlayMode> playMode;
		boost::optional<double> maximumExecutionTime;
		boost::optional<std::vector<Token>> tokenList;

		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:

		void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct SSetupScenarioCmd
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the setup of a scenario
	* \ingroup ScenarioPlayer
	*
	* SSetupScenarioCmd contains the following properties:
	* - ScenarioName: name of the scenario to setup (mandatory)
	* - TokenList: List of scenario specific tokens (optional)
	* .
	*
	* The token list overwrites the previous token list if the command was already
	* called on the same scenario (note that an empty token list is allowed).
	*
	*/
	struct SSetupScenarioCmd final : SCommand
	{
		using Token = std::pair<std::string, std::string>;

		// List of properties
		boost::optional<std::string> scenarioName;
		boost::optional<std::vector<Token>> tokenList;


		EPlayerReturnCode execute(CKernelFacade& kernelFacade) const override;

	protected:

		void doPrint(std::ostream& os) const override;
	};
}	// namespace OpenViBE

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

#include <string>

#include <boost/optional.hpp>

#include "cvsp_defines.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \struct CommandInterface
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Base abstract struct for commands
	* \ingroup ScenarioPlayer
	*
	* A command is an object that encapsulates all necessary information to perform an action later one.
	* Typically, a command implementation should contain an implementation of CommandInterface interface,
	* and a list of properties.
	*
	*/
	struct CommandInterface
	{

		CommandInterface() = default;
		virtual ~CommandInterface() = default;

		friend std::ostream& operator<< (std::ostream& os, const CommandInterface& cmd);

		/**
		* \brief Execute the command
		* \param[in] kernelFacade the kernel facade that gives access to kernel features 
		*/
		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const = 0;

	protected:

		// use of the non-virtual interface pattern to implement printing in the class hierarchy
		virtual void doPrint(std::ostream& os) const = 0;

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class
		CommandInterface(const CommandInterface&) = delete;
		CommandInterface &operator=(const CommandInterface&) = delete;
	};

	inline std::ostream& operator<<(std::ostream& os, const CommandInterface& cmd)
	{
		cmd.doPrint(os);
		return os;
	}

	/**
	* \struct InitCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the initialization of the tool
	* \ingroup ScenarioPlayer
	*
	* InitCommand class contains the following properties:
	* - Benchmark: Flag to enable benchmark on execute command (optional)
	* .
	*
	*/
	struct InitCommand : CommandInterface
	{
		// List of properties
		boost::optional<bool> m_Benchmark;


		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;
	};


	/**
	* \struct LoadKernelCommand
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
	struct LoadKernelCommand : CommandInterface
	{
		// List of properties
		boost::optional<std::string> m_ConfigurationFile;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct LoadScenarioCommand
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
	struct LoadScenarioCommand : CommandInterface
	{
		// List of properties
		boost::optional<std::string> m_ScenarioFile;
		boost::optional<std::string> m_ScenarioName;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct ResetCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives tool reset to its initial state
	* \ingroup ScenarioPlayer
	*/
	struct ResetCommand : CommandInterface
	{

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct RunScenarioCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the execution of a list of scenarios
	* \ingroup ScenarioPlayer
	*
	* RunScenarioCommand contains the following properties:
	* - ScenarioList: Names of scenario that must be executed (mandatory)
	* - PlayMode: 0 for standard, 1 for fastforward (optional)
	* - MaximumExecutionTime: Scenarios playing execution time limit (optional)
	* - ResetList: Names of scenario whose setup must be reset (optional)
	* - TokenList: List of global (token,value) pairs (optional)
	* .
	*
	*/
	struct RunScenarioCommand : CommandInterface
	{

	public:

		using Token = std::pair<std::string, std::string>;

		// List of properties
		boost::optional<std::vector<std::string>> m_ScenarioList;
		boost::optional<PlayMode> m_PlayMode;
		boost::optional<double> m_MaximumExecutionTime;
		boost::optional<std::vector<std::string>> m_ResetList;
		boost::optional<std::vector<Token>> m_TokenList;

		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;
	};

	/**
	* \struct SetupScenarioCommand
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Command that drives the setup of a scenario
	* \ingroup ScenarioPlayer
	*
	* SetupScenarioCommand contains the following properties:
	* - ScenarioName: name of the scenario to setup (mandatory)
	* - ResetList: List of token to reset (optional)
	* - TokenList: List of scenario specific (optional)
	* .
	*
	*/
	struct SetupScenarioCommand : CommandInterface
	{

		using Token = std::pair<std::string, std::string>;

		// List of properties
		boost::optional<std::string> m_ScenarioName;
		boost::optional<std::vector<std::string>> m_ResetList;
		boost::optional<std::vector<Token>> m_TokenList;


		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const override;

	protected:

		virtual void doPrint(std::ostream& os) const override;
	};
}
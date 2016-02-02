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

#include <memory>

#include "cvsp_defines.h"

namespace CertiViBE
{
	/**
	* \class KernelFacade
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Wrapper class used to access Kernel features
	* \ingroup ScenarioPlayer
	*
	* This class is one-to-many interface used as a central point
	* to access a subset of Kernel features.
	*
	*/
	class KernelFacade final
	{

	public:

		/**
		* Scenario running parameters wrapper
		*/
		struct RunParameters
		{
			double m_MaximumExecutionTime{ -1.0 }; /*!< Scenarios playing execution time limit */
			PlayMode m_PlayMode{ PlayMode::Standard }; /*!< Play mode for scenario execution */
			std::vector<std::string> m_ScenarioList; /*!< Names of scenarios to be executed */
			std::vector<std::string> m_ResetList; /*!< Names of token to be reset */
			std::vector<std::pair<std::string, std::string>> m_TokenList; /*!< List of condiguration token,value pairs */
		};

		KernelFacade();
		~KernelFacade();

		/**
		* \brief Initialize session parameters
		* \param[in] benchmark true if benchmarking on scenario playing is needed
		*/
		PlayerReturnCode initialize(bool benchmark = false);


		/**
		* \brief Reset session parameters
		*/
		PlayerReturnCode uninitialize();

		/**
		* \brief Load kernel
		* \param[in] configFile openvibe configuration file
		*/
		PlayerReturnCode loadKernel(const std::string& configFile = "");

		/**
		* \brief Unload kernel
		*/
		PlayerReturnCode unloadKernel();

		/**
		* \brief Load scenario
		* \param[in] scenarioName alias used to refer to this scenario
		* \param[in] scenarioFilename scenario xml file
		*/
		PlayerReturnCode loadScenario(const std::string& scenarioName, const std::string& scenarioFilename);

		/**
		* \brief Run one or multiple scenarios
		* \param[in] parameters run session setup
		*/
		PlayerReturnCode runScenarioList(const RunParameters& parameters);

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class event if it is not inheritable
		KernelFacade(const KernelFacade&) = delete;
		KernelFacade &operator=(const KernelFacade&) = delete;

		struct KernelFacadeImpl;
		std::unique_ptr<KernelFacadeImpl> m_Pimpl;
	};
}
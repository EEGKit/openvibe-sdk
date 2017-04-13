/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
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

#include <memory>

#include "ovsp_defines.h"

#define OVP_ScenarioImportContext_OnLoadMetaboxImport OpenViBE::CIdentifier(0xED48480A, 0x9F6DBAA7)

namespace OpenViBE
{
	struct InitCommand;
	struct ResetCommand;
	struct LoadKernelCommand;
	struct LoadScenarioCommand;
	struct SetupScenarioCommand;
	struct RunScenarioCommand;

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

		KernelFacade();
		~KernelFacade();

		/**
		* \brief Initialize session parameters
		* \param[in] command command containing all mandatory properties
		*/
		PlayerReturnCode initialize(const InitCommand& command);


		/**
		* \brief Reset session parameters
		*/
		PlayerReturnCode uninitialize();

		/**
		* \brief Load kernel
		* \param[in] command command containing all mandatory properties
		*/
		PlayerReturnCode loadKernel(const LoadKernelCommand& command);

		/**
		* \brief Unload kernel
		*/
		PlayerReturnCode unloadKernel();

		/**
		* \brief Load scenario
		* \param[in] command command containing all mandatory properties
		*/
		PlayerReturnCode loadScenario(const LoadScenarioCommand& command);

		/**
		* \brief Configure scenario
		* \param[in] command command containing all mandatory properties
		*/
		PlayerReturnCode setupScenario(const SetupScenarioCommand& command);

		/**
		* \brief Run one or multiple scenarios
		* \param[in] command command containing all mandatory properties
		*/
		PlayerReturnCode runScenarioList(const RunScenarioCommand& command);

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class event if it is not inheritable
		KernelFacade(const KernelFacade&) = delete;
		KernelFacade &operator=(const KernelFacade&) = delete;

		struct KernelFacadeImpl;
		std::unique_ptr<KernelFacadeImpl> m_Pimpl;
	};
}

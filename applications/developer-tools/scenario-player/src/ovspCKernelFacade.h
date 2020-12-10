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

#include "ovsp_defines.h"
#include <memory>

namespace OpenViBE {
struct SInitCmd;
struct SResetCmd;
struct SLoadKernelCmd;
struct SLoadScenarioCmd;
struct SUpdateScenarioCmd;
struct SSetupScenarioCmd;
struct SRunScenarioCmd;

/**
* \class CKernelFacade
* \author cgarraud (INRIA)
* \date 2016-01-26
* \brief Wrapper class used to access Kernel features
* \ingroup ScenarioPlayer
*
* This class is one-to-many interface used as a central point
* to access a subset of Kernel features.
*
*/
class CKernelFacade final
{
public:

	CKernelFacade();
	~CKernelFacade();

	/**
	* \brief Initialize session parameters
	*/
	static EPlayerReturnCodes initialize() { return EPlayerReturnCodes::Success; }

	/**
	* \brief Reset session parameters
	*/
	static EPlayerReturnCodes uninitialize() { return EPlayerReturnCodes::Success; }

	/**
	* \brief Load kernel
	* \param[in] command command containing all mandatory properties
	*/
	EPlayerReturnCodes loadKernel(const SLoadKernelCmd& command) const;

	/**
	* \brief Unload kernel
	*/
	EPlayerReturnCodes unloadKernel() const;

	/**
	* \brief Load scenario
	* \param[in] command command containing all mandatory properties
	*/
	EPlayerReturnCodes loadScenario(const SLoadScenarioCmd& command) const;

	/**
	* \brief Update scenario
	* \param[in] command command containing all mandatory properties
	*/
	EPlayerReturnCodes updateScenario(const SUpdateScenarioCmd& command) const;

	/**
	* \brief Configure scenario
	* \param[in] command command containing all mandatory properties
	*/
	EPlayerReturnCodes setupScenario(const SSetupScenarioCmd& command) const;

	/**
	* \brief Run one or multiple scenarios
	* \param[in] command command containing all mandatory properties
	*/
	EPlayerReturnCodes runScenarioList(const SRunScenarioCmd& command) const;

private:

	// disable copy and assignment because it is not meant to used
	// as a value class event if it is not inheritable
	CKernelFacade(const CKernelFacade&)            = delete;
	CKernelFacade& operator=(const CKernelFacade&) = delete;

	struct SKernelFacadeImpl;
	std::unique_ptr<SKernelFacadeImpl> m_impl;
};
}	// namespace OpenViBE

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
#include <vector>
#include <memory>

namespace OpenViBE {
struct SCommand;

/**
* \class ICommandParser
* \author cgarraud (INRIA)
* \date 2016-01-27
* \brief Base abstract class for command parser
* \ingroup ScenarioPlayer
*
* Command parsers aim at parsing a list of commands from a specific input.
*
*/
class ICommandParser
{
public:

	virtual ~ICommandParser() = default;
	ICommandParser()          = default;

	/**
	* \brief Initialize parser
	*
	*/
	virtual void initialize() = 0;

	/**
	* \brief Unitialize parser
	*
	*/
	virtual void uninitialize() = 0;

	/**
	* \brief Retrieve the list of commands
	* \pre This method should be called after the parse() method
	*
	*/
	virtual std::vector<std::shared_ptr<SCommand>> getCommandList() const = 0;


	/**
	* \brief Retrieve the list of commands
	* \pre This method should be called after the initialize() method
	*
	*/
	virtual EPlayerReturnCodes parse() = 0;

private:

	// disable copy and assignment because it is not meant to used
	// as a value class
	ICommandParser(const ICommandParser&)            = delete;
	ICommandParser& operator=(const ICommandParser&) = delete;
};
}	// namespace OpenViBE

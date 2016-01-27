#pragma once

#include "cvsp_defines.h"

namespace CertiViBE
{
	class KernelFacade;

	/**
	* \class CommandInterface
	* \author cgarraud (INRIA)
	* \date 2016-01-26
	* \brief Base abstract class for commands
	* \ingroup ScenarioPlayer
	*
	* A command is an object that encapsulates all necessary information to perform an action later one.
	* Typically, a command implementation should contain an implementation of CommandInterface interface,
	* a list of setX/getX/hasX for each property (hasX is here to check a property has been set or not).
	*
	*/
	class CommandInterface
	{

	public:

		virtual ~CommandInterface()
		{
		}

		/**
		* \brief Clear command internal state
		*/
		virtual void clear() = 0;

		/**
		* \brief Execute the command
		* \param[in] kernelFacade the kernel facade that gives access to kernel features 
		*/
		virtual PlayerReturnCode execute(KernelFacade& kernelFacade) const = 0;

	private:

		// disable copy and assignment because it is not meant to used
		// as a value class
		CommandInterface(const CommandInterface&) = delete;
		CommandInterface &operator=(const CommandInterface&) = delete;
	};
}
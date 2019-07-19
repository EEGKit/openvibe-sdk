#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IBoxListenerContext
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-06
		 * \brief Exectution context for box algorithm descriptors
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 * \sa OpenViBE::Plugins::IBoxAlgorithmDesc
		 * \sa OpenViBE::Plugins::IBoxAlgorithm
		 */
		class OV_API IBoxListenerContext : public IKernelObject
		{
		public:

			/** \name Manager accessors */
			//@{

			/**
			 * \brief Gets a reference on the current algorithm manager
			 * \return a reference on the current algorithm manager
			 */
			virtual IAlgorithmManager& getAlgorithmManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current player manager
			 * \return a reference on the current player manager
			 */
			virtual IPlayerManager& getPlayerManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current plugin manager
			 * \return a reference on the current plugin manager
			 */
			virtual IPluginManager& getPluginManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current metabox manager
			 * \return a reference on the current metabox manager
			 */
			virtual IMetaboxManager& getMetaboxManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current scenario manager
			 * \return a reference on the current scenario manager
			 */
			virtual IScenarioManager& getScenarioManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current type manager
			 * \return a reference on the current type manager
			 */
			virtual ITypeManager& getTypeManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current log manager
			 * \return a reference on the current log manager
			 */
			virtual ILogManager& getLogManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current error manager
			 * \return a reference on the current error manager
			 */
			virtual IErrorManager& getErrorManager(void) const = 0;

			/**
			 * \brief Gets a reference on the current configuration manager
			 * \return a reference on the current configuration manager
			 */
			virtual IConfigurationManager& getConfigurationManager(void) const = 0;

			//@}
			/** \name Box specific stuffs */

			/**
			 * \brief Gets a reference to the considered box
			 * \return a reference to the considered box
			 */
			virtual IBox& getBox(void) const = 0;
			/**
			 * \brief Gets a reference to the scenario owning the considered box
			 * \return a reference to the scenario owning the considered box
			 */
			virtual IScenario& getScenario(void) const = 0;
			/**
			 * \brief Gets the index of the concerned callback message
			 * \return the index of the concerned callback message in case it makes sense
			 * \return \c 0xffffffff in case it does not make any sense
			 */
			virtual uint32_t getIndex(void) const = 0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_BoxListenerContext)
		};
	};
};



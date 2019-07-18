#ifndef __OpenViBE_Kernel_Plugins_IPluginModuleContext_H__
#define __OpenViBE_Kernel_Plugins_IPluginModuleContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class ILogManager;
		class ITypeManager;
		class IScenarioManager;
		class IErrorManager;
		class IConfigurationManager;

		/**
		 * \class IPluginModuleContext
		 * \brief Plugin context
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-06-20
		 * \ingroup Group_Plugins
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 */
		class OV_API IPluginModuleContext : public IKernelObject
		{
		public:

			/**
			 * \brief Gets the current scenario manager
			 * \return a reference on the current scenario manager
			 */
			virtual IScenarioManager& getScenarioManager(void) const = 0;
			/**
			 * \brief Gets the current type manager
			 * \return a reference on the current type manager
			 */
			virtual ITypeManager& getTypeManager(void) const = 0;
			/**
			 * \brief Gets the current log manager
			 * \return a reference on the current log manager
			 */
			virtual ILogManager& getLogManager(void) const = 0;
			/**
			 * \brief Gets the current error manager
			 * \return a reference on the current error manager
			 */
			virtual IErrorManager& getErrorManager(void) const = 0;
			/**
			 * \brief Gets the current configuration manager
			 * \return a reference on the current configuration manager
			 */
			virtual IConfigurationManager& getConfigurationManager(void) const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Plugins_PluginModuleContext)
		};
	};
};

#endif // __OpenViBE_Kernel_Plugins_IPluginModuleContext_H__

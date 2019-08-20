#pragma once

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IAlgorithmManager;
		class IConfigurationManager;
		class IKernelObjectFactory;
		class IPlayerManager;
		class IPluginManager;
		class IMetaboxManager;
		class IScenarioManager;
		class ITypeManager;
		class ILogManager;
		class IErrorManager;

		/**
		 * \class IKernelContext
		 * \brief Kernel context interface, gives access to each manager the kernel owns
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-10-24
		 *
		 * This class simply provides access to each manager the kernel owns. This is the top
		 * level object that can be used by a custom OpenViBE application and this is the common
		 * object all kernel object have in order to access all the functionnalities.
		 *
		 * See each manager's own documentation for more detail on a specific manager goal and usage.
		 */
		class OV_API IKernelContext : public IKernelObject
		{
		public:

			/**
			 * \brief Initializes the kernel context
			 * \return \e true in case of success
			 * \return \e false in case of error
			 */
			virtual bool initialize(const char* const* tokenList = nullptr, size_t tokenCount = 0) { return true; }
			/**
			 * \brief Uninitializes the kernel context
			 * \return \e true in case of success
			 * \return \e false in case of error
			 */
			virtual bool uninitialize() { return true; }

			/**
			 * \brief Gets a reference on the kernel's algorithm manager
			 * \return a reference on the kernel's algorithm manager
			 */
			virtual IAlgorithmManager& getAlgorithmManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's configuration manager
			 * \return a reference on the kernel's configuration manager
			 */
			virtual IConfigurationManager& getConfigurationManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's player manager
			 * \return a reference on the kernel's player manager
			 */
			virtual IPlayerManager& getPlayerManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's plugin manager
			 * \return a reference on the kernel's plugin manager
			 */
			virtual IPluginManager& getPluginManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's metabox manager
			 * \return a reference on the kernel's metabox manager
			 */
			virtual IMetaboxManager& getMetaboxManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's object factory
			 * \return a reference on the kernel's object factory
			 */
			virtual IKernelObjectFactory& getKernelObjectFactory() const = 0;
			/**
			 * \brief Gets a reference on the kernel's scenario manager
			 * \return a reference on the kernel's scenario manager
			 */
			virtual IScenarioManager& getScenarioManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's type manager
			 * \return a reference on the kernel's type manager
			 */
			virtual ITypeManager& getTypeManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's log manager
			 * \return a reference on the kernel's log manager
			 */
			virtual ILogManager& getLogManager() const = 0;
			/**
			 * \brief Gets a reference on the kernel's error manager
			 * \return a reference on the kernel's error manager
			 */
			virtual IErrorManager& getErrorManager() const = 0;


			// backward compatibility
			virtual IKernelObjectFactory& getObjectFactory() const { return getKernelObjectFactory(); }

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_KernelContext)
		};
	} // namespace Kernel
} // namespace OpenViBE

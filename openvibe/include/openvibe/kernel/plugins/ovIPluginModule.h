#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Plugins
	{
		class IPluginObjectDesc;
	}

	namespace Kernel
	{
		/**
		 * \class IPluginModule
		 * \brief Plugin module
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Plugins
		 * \ingroup Group_Kernel
		 *
		 * Each plugin module is exactly responsible for one module
		 * (ie DLL/so file) containing OpenViBE plugins. It is
		 * able to load/unload this file and to enumerate each of
		 * its plugin object descriptor.
		 */
		class OV_API IPluginModule : public IKernelObject
		{
		public:

			/**
			 * \brief Tries to load a file as an OpenViBE module
			 * \param name [in] : the name of the file to try to load
			 * \param pError [out] : an optional output string containing the error on load failure
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool load(const CString& name, CString* pError = nullptr) = 0;
			/**
			 * \brief Tries to unload the loaded OpenViBE module
			 * \param pError [out] : an optional output string containing the error on unload failure
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool unload(CString* pError = nullptr) = 0;
			/**
			 * \brief Gets the current filename associated with this plugin module
			 * \param rFileName [out] : the filename of this plugin module
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getFileName(CString& rFileName) const = 0;
			/**
			 * \brief Initializes this plugin module
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This function calls the onInitialize main function of the plugin module.
			 */
			virtual bool initialize() = 0;
			/**
			 * \brief Gets a specific plugin object descriptor
			 * \param index [in] : the index of the plugin object descriptor to get
			 * \param rpPluginObjectDescription [out] : a pointer on the associated plugin object descriptor
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This function should be called with \c index ranging from 0 to
			 * the number of plugin object descriptor - 1. The number of plugin object
			 * descriptor is known as soon as this function returns \e false meaning
			 * there are no more descriptors to return.
			 *
			 * This function calls the onGetPluginObjectDescription main function of the plugin module.
			 *
			 * \note \c rpPluginObjectDescription can be \c NULL even if the function returned \e true
			 * \note \c rpPluginObjectDescription IS \c NULL if the function returned \e false
			 * \note It is ok to call this function several times for a same index.
			 */
			virtual bool getPluginObjectDescription(uint32_t index, Plugins::IPluginObjectDesc*& rpPluginObjectDescription) = 0;
			/**
			 * \brief Uninitializes this plugin module
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This function calls the onUninitialize main function of the plugin module.
			 */
			virtual bool uninitialize() = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Plugins_PluginModule)
		};
	} // namespace Kernel
} // namespace OpenViBE

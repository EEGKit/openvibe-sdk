#pragma once

#include "../ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \brief Functionality enumeration in order to know what a plugin is capable of
		 */
		enum EPluginFunctionality
		{
			PluginFunctionality_Undefined,
			PluginFunctionality_Processing,
		};

		/**
		 * \brief License type enumeration in order to know what software license a plugin can be released with
		 */
		enum class ELicenseType
		{
			Unspecified,	///< Generic for unspecified license type
			Commercial,		///< Generic for commercial software
			AFL,			///< Academic Free License
			AL20,			///< Apache License, Version 2.0
			ASL,			///< Apache Software License
			APSL,			///< Apple Public Source Licenses
			BSD,			///< BSD License
			CPL,			///< Common Public License
			CeCILL,			///< Licence CEA CNRS INRIA Logiciel Libre
			CeCILLB,		///< Licence CEA CNRS INRIA Logiciel Libre B
			CeCILLC,		///< Licence CEA CNRS INRIA Logiciel Libre C
			EFL2,			///< Eiffel Forum License Version 2.0
			GPL,			///< GNU General Public License
			LGPL,			///< GNU Lesser General Public License
			IBMPL,			///< IBM Public License
			IOSL,			///< Intel Open Source License
			MPL10,			///< Mozilla Public License Version 1.0
			MPL11,			///< Mozilla Public License Version 1.1
			NPL10,			///< Netscape Public License Version 1.0
			NPL11,			///< Netscape Public License Version 1.1
			OSL,			///< Open Software License
			PHPL,			///< PHP License
			PAL,			///< Perl Artistic License
			CNRIPL,			///< Python License (CNRI Python License)
			PSFL,			///< Python Software Foundation License
			QPL,			///< Q Public Licence (QT)
			SL,				///< Sleepycat Software Product License
			SISSL,			///< Sun Industry Standards Source License
			SPL,			///< Sun Public License
			W3C,			///< W3C Software License
			WXWLL,			///< wxWindows Library License
			ZLL,			///< zlib/libpng License
			ZPL,			///< Zope Public License
		};
	}  // namespace Kernel

	namespace Plugins
	{
		class IPluginObject;

		/**
		 * \class IPluginObjectDesc
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-19
		 * \brief Base class for plugin descriptor
		 * \ingroup Group_Extend
		 *
		 * This class is the base class for all the plugin description classes. 
		 * It contains basic functions that could be used for each plugin description. 
		 * Derived plugin descriptions will be used as a prototype of what they can create.
		 *
		 * \sa IBoxAlgorithmDesc
		 * \sa IScenarioImporterDesc
		 * \sa IScenarioExporterDesc
		 * \sa IPluginObject
		 *
		 * \todo details about building new plugins
		 */
		class OV_API IPluginObjectDesc : public IObject
		{
		public:
			/** \name Memory management */
			//@{

			/**
			 * \brief Informs the plugin description it won't be used any more
			 *
			 * This is called by the OpenViBE platform to inform the plugin description it is not useful anymore.
			 * The plugin can chose whether to delete itself or to stay in memory until it decides it is ok to be deleted. 
			 * However, the OpenViBE platform will not call any of the plugin functions after release is called.
			 */
			virtual void release() = 0;

			//@}
			/** \name Creation process */
			//@{

			/**
			 * \brief Gives a tip on what this plugin descriptor is able to create
			 *
			 * This may inform the OpenViBE platform about what kind of plugin can be created using this plugin descriptor. 
			 * It should return the concrete class identifier of the plugin object itself.
			 */
			virtual CIdentifier getCreatedClass() const = 0;
			virtual CIdentifier getCreatedClassIdentifier() const { return this->getCreatedClass(); }
			/**
			 * \brief Creates the plugin object itself
			 * \return the created object.
			 *
			 * This method creates the plugin object itself and returns it with the lowest level interface. 
			 * The OpenVIBE platform then uses the IObject::isDerivedFromClass method to use the plugin correctly.
			 */
			virtual IPluginObject* create() = 0;

			//@}
			/** \name Textual plugin object description and information */
			//@{

			/**
			 * \brief Gets the plugin name
			 * \return The plugin name.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual CString getName() const { return CString("no name"); }
			/**
			 * \brief Gets the author name for this plugin
			 * \return The author name for this plugin.
			 *
			 * Default implementation simply returns "no name".
			 */
			virtual CString getAuthorName() const { return CString("unknown"); }
			/**
			 * \brief Gets the author company name for this plugin
			 * \return The author company name for this plugin.
			 *
			 * Default implementation simply returns "unknown".
			 */
			virtual CString getAuthorCompanyName() const { return CString("unknown"); }
			/**
			 * \brief Gets a short description of the plugin
			 * \return A short description of the plugin.
			 *
			 * Default implementation simply returns "unknown".
			 */
			virtual CString getShortDescription() const { return CString(""); }
			/**
			 * \brief Gets a detailed description of the plugin
			 * \return A detailed description of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 *
			 * \note You can use std::endl to have the description on several lines when needed.
			 */
			virtual CString getDetailedDescription() const { return CString(""); }
			/**
			 * \brief Gets a basic category of the plugin
			 * \return the category tokens of the plugin
			 *
			 * The tokens should be separated with '/' characters in order to create sub categories.
			 *
			 * Default implementation returns "unknown".
			 */
			virtual CString getCategory() const { return CString("unknown"); }
			/**
			 * \brief Gets the version of the plugin
			 * \return the version of the plugin.
			 *
			 * Default implementation simply returns "unknown".
			 */
			virtual CString getVersion() const { return CString("unknown"); }
			/**
			 * \brief Gets the component in which the plugin is defined
			 *
			 * OpenViBE SDK is used to build platforms that are composed of several components.
			 * Each component can come with its own set of plugins, and has its own version.
			 * The \ref getAddedSoftwareVersion and \ref getUpdatedSoftwareVersion will return versions of the component in which the plugin belongs.
			 *
			 * \return a string identifier of the component
			 * \retval "unknown" if not redefined
			 */
			virtual CString getSoftwareComponent() const { return CString("unknown"); }
			/**
			 * \brief Gets the version of the software in the one the plugin was added
			 * \return the version of the software in the one the plugin was added.
			 * \retval "unknown" if not redefined
			 *
			 * \sa getSoftwareComponent
			 */
			virtual CString getAddedSoftwareVersion() const { return CString("unknown"); }
			/**
			 * \brief Gets the version of the software in the one the plugin was last updated
			 * \return the version of the software in the one the plugin was last updated
			 * \retval "unknown" if not redefined
			 *
			 *  \sa getSoftwareComponent
			 */
			virtual CString getUpdatedSoftwareVersion() const { return CString("unknown"); }
			/**
			 * \brief Tests whether the plugin has a given functionality
			 * \param functionality [in] : functionality of interest
			 * \return \e true in case plugin has this functionality.
			 * \return \e false otherwise.
			 */
			virtual bool hasFunctionality(Kernel::EPluginFunctionality functionality) const { return false; }

			/**
			 * \brief Tests whether the plugin has a specific functionality identified by a CIdentifier
			 * \param functionality The identifier of the functionality to poll
			 * \retval true If the plugin has the demanded functionality.
			 * \retval false If the plugin does not have the demanded functionality.
			 */
			virtual bool hasFunctionality(const CIdentifier& functionality) const { return false; }
			//@}

			_IsDerivedFromClass_(IObject, OV_ClassId_Plugins_PluginObjectDesc)
		};
	} // namespace Plugins
} // namespace OpenViBE

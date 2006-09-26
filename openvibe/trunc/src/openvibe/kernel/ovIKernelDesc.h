#ifndef __OpenViBE_Kernel_IKernelDesc_H__
#define __OpenViBE_Kernel_IKernelDesc_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IKernel;

		class OV_API IKernelDesc : virtual public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Memory management */
			//@{

			/**
			 * \brief Informs the plugin description it won't be used any more
			 *
			 * This is called by the žOpenViBEž platform to inform
			 * the plugin description it is not usefull anymore.
			 * The plugin can chose whether to delete itself or
			 * to stay in memory until it decides it is ok to be
			 * deleted. However, the žOpenViBEž platform will not
			 * call any of the plugin functions after release is
			 * called.
			 */
			virtual void release(void)=0;

			//@}
			/** \name Creation process */
			//@{

			/**
			 * \brief Creates the kernel itself
			 * \return the created kernel.
			 *
			 * This method creates the kernel itself and returns it.
			 */
			virtual OpenViBE::Kernel::IKernel* create(void)=0;

			//@}
			/** \name Textual plugin object description and information */
			//@{

			/**
			 * \brief Gets the plugin name
			 * \return The plugin name.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getName(void);
			/**
			 * \brief Gets the author name for this plugin
			 * \return The author name for this plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getAuthorName(void);
			/**
			 * \brief Gets the author company name for this plugin
			 * \return The author company name for this plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getAuthorCompanyName(void);
			/**
			 * \brief Gets a short description of the plugin
			 * \return A short description of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getShortDescription(void);
			/**
			 * \brief Gets a detailed description of the plugin
			 * \return A detailed description of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 *
			 * \note You can use std::endl to have the description
			 *       on several lines when needed.
			 */
			virtual OpenViBE::CString getDetailedDescription(void);
			/**
			 * \brief Gets the version of the plugin
			 * \return the version of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getVersion(void);

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassIdentifier_KernelDesc)
		};
	};
};

#endif // __OpenViBE_Kernel_IKernelDesc_H__

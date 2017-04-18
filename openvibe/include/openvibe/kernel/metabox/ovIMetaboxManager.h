#ifndef __OpenViBE_Kernel_Metabox_IMetaboxManager_H__
#define __OpenViBE_Kernel_Metabox_IMetaboxManager_H__

#include "../ovIKernelObject.h"
#include "../../plugins/ovIBoxAlgorithmDesc.h"

namespace OpenViBE
{
//	namespace Plugins
//	{
//	//		class IPluginObject;
////		class IPluginObjectDesc;

//	//		class IBoxAlgorithm;
//		class IBoxAlgorithmDesc;

//	//		class IAlgorithm;
//	//		class IAlgorithmDesc;
//	};

	namespace Metabox
	{
		class OV_API IMetaboxObjectDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getMetaboxDescriptor(void) const = 0;
		};
//		class IPluginObject;
//		class IPluginObjectDesc;

//		class IBoxAlgorithm;
//		class IBoxAlgorithmDesc;

//		class IAlgorithm;
//		class IAlgorithmDesc;
	};



	namespace Kernel
	{
		// class IPluginModule;

		/**
		 * \class IMetaboxManager
		 * \brief Metabox manager
		 * \author Thierry Gaugry (INRIA/Mensia?)
		 * \date 2017-04-12
		 * \ingroup Group_Metabox
		 * \ingroup Group_Kernel
		 *
		 * The metabox manager is in charge of loading/unloading metaboxes
		 * modules (ie .mxb files) containing OpenViBE metaboxess.
		 * It also provides functions in order to list metabox descriptors,
		 * create or release metabox objects...
		 */
		class OV_API IMetaboxManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Metabox modules/descriptors management */
			//@{

			/**
			 * \brief Loads new .mxb metabox module file(s)
			 * \param rFileNameWildCard [in] : a wild card with the file(s) to search plugins in
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool addMetaboxFromFiles(const OpenViBE::CString& rFileNameWildCard)=0;

			/**
			 * \brief Gets next metabox object descriptor identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding metabox object descriptor
			 * \return The identifier of the next metabox object descriptor in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first metabox object
			 *       descriptor identifier.
			 */
			virtual OpenViBE::CIdentifier getNextMetaboxObjectDescIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			/**
			 * \brief Gets details on a specific metabox object descriptor
			 * \param rIdentifier [in] : the metabox object descriptor identifier which details should be returned
			 * \return the corresponding metabox object descriptor pointer.
			 */
			virtual const OpenViBE::Plugins::IPluginObjectDesc* getMetaboxObjectDesc(
				const OpenViBE::CIdentifier& rIdentifier) const=0;

			/**
			 * \brief Gets the path of the scenario of a specific metabox
			 * \param rIdentifier [in] : the metabox object descriptor identifier which path should be returned
			 * \return the path to the scenario file of the metabox.
			 */
			virtual OpenViBE::CString getMetaboxFilePath(const OpenViBE::CIdentifier& rClassIdentifier) const = 0;
			/**
			 * \brief Sets the path of the scenario of a specific metabox
			 * \param rIdentifier [in] : the metabox object descriptor identifier
			 * \param filePath [in] : the metabox scenario path
			 */
			virtual void setMetaboxFilePath(const OpenViBE::CIdentifier& rClassIdentifier, const OpenViBE::CString& filePath) = 0;

			/**
			 * \brief Gets the hash of the metabox
			 * \param rIdentifier [in] : the metabox object descriptor identifier which hash should be returned
			 * \return the hash of the metabox.
			 */
			virtual OpenViBE::CIdentifier getMetaboxHash(const OpenViBE::CIdentifier& rClassIdentifier) const = 0;
			/**
			 * \brief Sets the hash of the metabox
			 * \param rIdentifier [in] : the metabox object descriptor identifier
			 * \param hash [in] : the metabox hash
			 */
			virtual void setMetaboxHash(const OpenViBE::CIdentifier& rClassIdentifier, const OpenViBE::CIdentifier& hash) = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Metabox_MetaboxManager)

		};
	};
};

#endif // __OpenViBE_Kernel_Plugins_IPluginManager_H__

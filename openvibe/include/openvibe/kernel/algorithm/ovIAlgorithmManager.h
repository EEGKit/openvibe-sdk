#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Plugins
	{
		class IAlgorithmDesc;
	}

	namespace Kernel
	{
		class IAlgorithmProxy;

		/**
		 * \class IAlgorithmManager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-06
		 * \brief Manager for all kind of plugin algorithms
		 * \ingroup Group_Algorithm
		 * \ingroup Group_Kernel
		 * \sa OpenViBE::Plugins::IAlgorithm
		 */
		class OV_API IAlgorithmManager : public IKernelObject
		{
		public:

			/**
			 * \brief Creates a new algorithm
			 * \param algorithmClassID [out] : the class identifier of
			 *        the newly created algorithm
			 * \return \e true in case of success.
			 * \return \e OV_UndefinedIdentifier in case of error.
			 */
			virtual CIdentifier createAlgorithm(const CIdentifier& algorithmClassID) = 0;
			/**
			  * \brief Creates a new algorithm
			  * \param algorithmDesc [in] : the algorithm descriptor of
			  *        the algorithm to create
			  * \return \e identifier of the created algorithm
			  * \return \e OV_UndefinedIdentifier
			  */
			virtual CIdentifier createAlgorithm(const Plugins::IAlgorithmDesc& algorithmDesc) = 0;

			/**
			 * \brief Releases an existing algorithm
			 * \param rAlgorithmIdentifier [in] : the existing algorithm identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool releaseAlgorithm(const CIdentifier& rAlgorithmIdentifier) = 0;
			/**
			 * \brief Releases an existing algorithm
			 * \param rAlgorithm [in] : the existing algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool releaseAlgorithm(IAlgorithmProxy& rAlgorithm) = 0;
			/**
			 * \brief Gets details on a specific algorithm
			 * \param rAlgorithmIdentifier [in] : the algorithm identifier which details should be returned
			 * \return the corresponding algorithm reference.
			 * \warning Calling this function with a bad identifier causes a crash
			 */
			virtual IAlgorithmProxy& getAlgorithm(const CIdentifier& rAlgorithmIdentifier) = 0;
			/**
			 * \brief Gets next algorithm identifier
			 * \param previousID [in] : The identifier
			 *        for the preceeding algorithm
			 * \return The identifier of the next algorithm in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousID
			 *       will cause this function to return the first algorithm
			 *       identifier.
			 */
			virtual CIdentifier getNextAlgorithmIdentifier(const CIdentifier& previousID) const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Algorithm_AlgorithmManager)
		};
	} // namespace Kernel
} // namespace OpenViBE

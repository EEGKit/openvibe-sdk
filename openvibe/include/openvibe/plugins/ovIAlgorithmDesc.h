#pragma once

#include "ovIPluginObjectDesc.h"
#include "../kernel/algorithm/ovIAlgorithmProto.h"

namespace OpenViBE
{
	namespace Plugins
	{
		/**
		 * \class IAlgorithmDesc
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-06
		 * \brief Algorithm plugin descriptor
		 * \ingroup Group_Extend
		 *
		 * This class should be derived by any plugin developer in
		 * order to describe a specific OpenViBE algorithm.
		 *
		 * \sa OpenViBE::Plugins::IAlgorithm
		 *
		 * \todo details about building new plugins
		 */
		class OV_API IAlgorithmDesc : public IPluginObjectDesc
		{
		public:

			/**
			 * \brief Gets the prototype for this algorithm
			 * \param rAlgorithmPrototype [out] : the prototype to fill
			 * \return \e true in case of success \e false in other cases.
			 *
			 * When this function is called by the OpenViBE
			 * platform, the plugin descriptor should fill in
			 * the structure to let the OpenViBE platform know
			 * what the algorithm should look like
			 * (inputs/outputs/triggers).
			 *
			 * \sa OpenViBE::Plugins::IAlgorithmProto
			 */
			virtual bool getAlgorithmPrototype(Kernel::IAlgorithmProto& rAlgorithmPrototype) const = 0;

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObjectDesc, OV_ClassId_Plugins_AlgorithmDesc)
		};
	};
};



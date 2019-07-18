#ifndef __OpenViBE_Kernel_IObjectVisitorContext_H__
#define __OpenViBE_Kernel_IObjectVisitorContext_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IObjectVisitorContext
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2008-02-01
		 * \brief Exectution context for visitor objects
		 * \ingroup Group_Kernel
		 */
		class OV_API IObjectVisitorContext : public IKernelObject
		{
		public:

			/**
			 * \brief Gets a reference on the current algorithm manager
			 * \return a reference on the current algorithm manager
			 */
			virtual IAlgorithmManager& getAlgorithmManager(void) const = 0;
			/**
			 * \brief Gets a reference on the current configuration manager
			 * \return a reference on the current configuration manager
			 */
			virtual IConfigurationManager& getConfigurationManager(void) const = 0;
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

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_ObjectVisitorContext)
		};
	};
};

#endif // __OpenViBE_Kernel_ObjectVisitor_IObjectVisitorContext_H__

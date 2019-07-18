#ifndef __OpenViBEKernel_Kernel_CObjectVisitorContext_H__
#define __OpenViBEKernel_Kernel_CObjectVisitorContext_H__

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CObjectVisitorContext : public TKernelObject<IObjectVisitorContext>
		{
		public:

			explicit CObjectVisitorContext(const IKernelContext& rKernelContext);
			virtual ~CObjectVisitorContext(void);

			virtual IAlgorithmManager& getAlgorithmManager(void) const;
			virtual IConfigurationManager& getConfigurationManager(void) const;
			virtual ITypeManager& getTypeManager(void) const;
			virtual ILogManager& getLogManager(void) const;
			virtual IErrorManager& getErrorManager(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IObjectVisitorContext >, OVK_ClassId_Kernel_ObjectVisitorContext)
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CObjectVisitorContext_H__

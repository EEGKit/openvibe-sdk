#ifndef __OpenViBEKernel_Kernel_CObjectVisitorContext_H__
#define __OpenViBEKernel_Kernel_CObjectVisitorContext_H__

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CObjectVisitorContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IObjectVisitorContext>
		{
		public:

			explicit CObjectVisitorContext(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CObjectVisitorContext(void);

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const;
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const;
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const;
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const;
			virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IObjectVisitorContext >, OVK_ClassId_Kernel_ObjectVisitorContext)
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CObjectVisitorContext_H__

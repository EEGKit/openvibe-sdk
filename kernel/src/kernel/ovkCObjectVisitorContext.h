#pragma once

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



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
			virtual ~CObjectVisitorContext();

			virtual IAlgorithmManager& getAlgorithmManager() const;
			virtual IConfigurationManager& getConfigurationManager() const;
			virtual ITypeManager& getTypeManager() const;
			virtual ILogManager& getLogManager() const;
			virtual IErrorManager& getErrorManager() const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IObjectVisitorContext >, OVK_ClassId_Kernel_ObjectVisitorContext)
		};
	};
};



#pragma once

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CObjectVisitorContext : public TKernelObject<IObjectVisitorContext>
		{
		public:

			explicit CObjectVisitorContext(const IKernelContext& ctx);
			~CObjectVisitorContext() override;
			IAlgorithmManager& getAlgorithmManager() const override;
			IConfigurationManager& getConfigurationManager() const override;
			ITypeManager& getTypeManager() const override;
			ILogManager& getLogManager() const override;
			IErrorManager& getErrorManager() const override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IObjectVisitorContext >, OVK_ClassId_Kernel_ObjectVisitorContext)
		};
	} // namespace Kernel
} // namespace OpenViBE

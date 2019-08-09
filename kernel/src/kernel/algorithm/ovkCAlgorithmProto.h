#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy;

		class CAlgorithmProto : public TKernelObject<IAlgorithmProto>
		{
		public:

			CAlgorithmProto(const IKernelContext& rKernelContext, CAlgorithmProxy& rAlgorithmProxy);
			bool addInputParameter(const CIdentifier& rInputParameterIdentifier, const CString& sInputName, EParameterType eParameterType, const CIdentifier& rSubTypeIdentifier) override;
			bool addOutputParameter(const CIdentifier& rOutputParameterIdentifier, const CString& sOutputName, EParameterType eParameterType, const CIdentifier& rSubTypeIdentifier) override;
			bool addInputTrigger(const CIdentifier& rInputTriggerIdentifier, const CString& rInputTriggerName) override;
			bool addOutputTrigger(const CIdentifier& rOutputTriggerIdentifier, const CString& rOutputTriggerName) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProto >, OVK_ClassId_Kernel_Algorithm_AlgorithmProto)

		protected:

			CAlgorithmProxy& m_rAlgorithmProxy;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



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

			virtual bool addInputParameter(const CIdentifier& rInputParameterIdentifier, const CString& sInputName, EParameterType eParameterType, const CIdentifier& rSubTypeIdentifier);
			virtual bool addOutputParameter(const CIdentifier& rOutputParameterIdentifier, const CString& sOutputName, EParameterType eParameterType, const CIdentifier& rSubTypeIdentifier);
			virtual bool addInputTrigger(const CIdentifier& rInputTriggerIdentifier, const CString& rInputTriggerName); 
			virtual bool addOutputTrigger(const CIdentifier& rOutputTriggerIdentifier, const CString& rOutputTriggerName);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProto >, OVK_ClassId_Kernel_Algorithm_AlgorithmProto)

		protected:

			CAlgorithmProxy& m_rAlgorithmProxy;
		};
	};
};



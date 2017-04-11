#ifndef __OpenViBEKernel_Kernel_Scenario_CBoxProto_H__
#define __OpenViBEKernel_Kernel_Scenario_CBoxProto_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxProto : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxProto>
		{
		public:

			CBoxProto(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IBox& rBox);

			virtual bool addInput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier);

			virtual bool addOutput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier);

			/*
			virtual bool addSetting(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue);
				//*/

			virtual bool addSetting(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue,
				const bool bModifiable = false);

			virtual bool addFlag(const OpenViBE::Kernel::EBoxFlag eBoxFlag);
			virtual bool addFlag(const OpenViBE::CIdentifier& cIdentifierFlag);
			virtual bool addInputSupport(const OpenViBE::CIdentifier &rTypeIdentifier);
			virtual bool addOutputSupport(const OpenViBE::CIdentifier &rTypeIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IBoxProto, OVK_ClassId_Kernel_Scenario_BoxProto)

		protected:

			OpenViBE::Kernel::IBox& m_rBox;

		private:

			CBoxProto(void);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CBoxProto_H__

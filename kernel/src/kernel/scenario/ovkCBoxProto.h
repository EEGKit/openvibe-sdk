#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxProto : public TKernelObject<IBoxProto>
		{
		public:

			CBoxProto(const IKernelContext& rKernelContext, IBox& rBox);
			bool addInput(const CString& sName, const CIdentifier& rTypeIdentifier, const CIdentifier& oIdentifier = OV_UndefinedIdentifier, const bool bNotify = true) override;
			bool addOutput(const CString& sName, const CIdentifier& rTypeIdentifier, const CIdentifier& rIdentifier = OV_UndefinedIdentifier, const bool bNotify = true) override;

			//virtual bool addSetting(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CString& sDefaultValue); 
			bool addSetting(const CString& sName, const CIdentifier& rTypeIdentifier, const CString& sDefaultValue,
									const bool bModifiable = false, const CIdentifier& rIdentifier = OV_UndefinedIdentifier, const bool bNotify = true) override;
			bool addFlag(const EBoxFlag eBoxFlag) override;
			bool addFlag(const CIdentifier& cIdentifierFlag) override;
			bool addInputSupport(const CIdentifier& rTypeIdentifier) override;
			bool addOutputSupport(const CIdentifier& rTypeIdentifier) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IBoxProto, OVK_ClassId_Kernel_Scenario_BoxProto)

		protected:

			IBox& m_rBox;

		private:

			CBoxProto();
		};
	}  // namespace Kernel
}  // namespace OpenViBE

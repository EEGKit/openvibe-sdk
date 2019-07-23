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

			virtual bool addInput(const CString& sName, const CIdentifier& rTypeIdentifier, const CIdentifier& oIdentifier = OV_UndefinedIdentifier, bool bNotify = true);

			virtual bool addOutput(const CString& sName, const CIdentifier& rTypeIdentifier, const CIdentifier& rIdentifier = OV_UndefinedIdentifier, bool bNotify = true);

			//virtual bool addSetting(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CString& sDefaultValue); 

			virtual bool addSetting(const CString& sName, const CIdentifier& rTypeIdentifier, const CString& sDefaultValue,
									bool bModifiable = false, const CIdentifier& rIdentifier = OV_UndefinedIdentifier, bool bNotify = true);

			virtual bool addFlag(EBoxFlag eBoxFlag);
			virtual bool addFlag(const CIdentifier& cIdentifierFlag);
			virtual bool addInputSupport(const CIdentifier& rTypeIdentifier);
			virtual bool addOutputSupport(const CIdentifier& rTypeIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IBoxProto, OVK_ClassId_Kernel_Scenario_BoxProto)

		protected:

			IBox& m_rBox;

		private:

			CBoxProto();
		};
	}  // namespace Kernel
}  // namespace OpenViBE

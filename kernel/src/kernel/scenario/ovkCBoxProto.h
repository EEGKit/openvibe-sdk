#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxProto : public TKernelObject<IBoxProto>
		{
		public:

			CBoxProto(const IKernelContext& ctx, IBox& box);
			bool addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier = OV_UndefinedIdentifier,
						  const bool bNotify                                                              = true) override;
			bool addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier = OV_UndefinedIdentifier,
						   const bool bNotify                                                              = true) override;

			//virtual bool addSetting(const OpenViBE::CString& name, const OpenViBE::CIdentifier& typeID, const OpenViBE::CString& sDefaultValue); 
			bool addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue,
							const bool bModifiable = false, const CIdentifier& identifier = OV_UndefinedIdentifier, const bool bNotify = true) override;
			bool addFlag(const EBoxFlag eBoxFlag) override;
			bool addFlag(const CIdentifier& cIdentifierFlag) override;
			bool addInputSupport(const CIdentifier& typeID) override;
			bool addOutputSupport(const CIdentifier& typeID) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IBoxProto, OVK_ClassId_Kernel_Scenario_BoxProto)

		protected:

			IBox& m_rBox;

		private:

			CBoxProto();
		};
	} // namespace Kernel
} // namespace OpenViBE

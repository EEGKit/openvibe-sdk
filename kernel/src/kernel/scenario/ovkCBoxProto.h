#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxProto : public TKernelObject<IBoxProto>
		{
		public:

			CBoxProto(const IKernelContext& ctx, IBox& box) : TKernelObject<IBoxProto>(ctx), m_box(box) {}
			~CBoxProto() override = default;
			bool addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier = OV_UndefinedIdentifier,
						  const bool notify                                                             = true) override;
			bool addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier = OV_UndefinedIdentifier,
						   const bool notify                                                             = true) override;

			//virtual bool addSetting(const OpenViBE::CString& name, const OpenViBE::CIdentifier& typeID, const OpenViBE::CString& sDefaultValue); 
			bool addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue,
							const bool bModifiable = false, const CIdentifier& identifier = OV_UndefinedIdentifier, const bool notify = true) override;
			bool addFlag(const EBoxFlag eBoxFlag) override;
			bool addFlag(const CIdentifier& cIdentifierFlag) override;
			bool addInputSupport(const CIdentifier& typeID) override { return m_box.addInputSupport(typeID); }
			bool addOutputSupport(const CIdentifier& typeID) override { return m_box.addOutputSupport(typeID); }

			_IsDerivedFromClass_Final_(TKernelObject<IBoxProto>, OVK_ClassId_Kernel_Scenario_BoxProto)

		protected:

			IBox& m_box;

		private:

			CBoxProto() = delete;
		};
	} // namespace Kernel
} // namespace OpenViBE

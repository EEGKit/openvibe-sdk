#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmPlayerController final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_PlayerController)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_decoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_buffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_stimulationSet;

			uint64_t m_stimulationID = 0;
			uint64_t m_actionID      = 0;
		};

		class CBoxAlgorithmPlayerControllerDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Player Controller"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Controls the player execution"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Add some settings to configure the way you want to control the player");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stimulation"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_PlayerController; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmPlayerController; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Stimulations", OV_TypeId_Stimulations);
				prototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				prototype.addSetting("Action to perform", OV_TypeId_PlayerAction, OV_TypeId_PlayerAction_Pause.toString());

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_PlayerControllerDesc)
		};
	} // namespace Stimulation
} // namespace OpenViBEPlugins

#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_PlayerController     OpenViBE::CIdentifier(0x5F426DCE, 0x08456E13)
#define OVP_ClassId_BoxAlgorithm_PlayerControllerDesc OpenViBE::CIdentifier(0x2E4142D7, 0x3F957E25)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmPlayerController : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_PlayerController)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;

			uint64_t m_ui64StimulationIdentifier = 0;
			uint64_t m_ui64ActionIdentifier = 0;
		};

		class CBoxAlgorithmPlayerControllerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Player Controller"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Controls the player execution"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Add some settings to configure the way you want to control the player"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stimulation"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_PlayerController; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmPlayerController; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addInput("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Action to perform", OV_TypeId_PlayerAction, OV_TypeId_PlayerAction_Pause.toString());

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_PlayerControllerDesc)
		};
	};
};

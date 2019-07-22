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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_PlayerController)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;

			uint64_t m_ui64StimulationIdentifier;
			uint64_t m_ui64ActionIdentifier;
		};

		class CBoxAlgorithmPlayerControllerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Player Controller"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Controls the player execution"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Add some settings to configure the way you want to control the player"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_PlayerController; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmPlayerController; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
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



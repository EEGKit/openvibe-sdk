#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_StreamEndDetector     OpenViBE::CIdentifier(0x44F2725A, 0x8E922233)
#define OVP_ClassId_BoxAlgorithm_StreamEndDetectorDesc OpenViBE::CIdentifier(0x6DD8B6EA, 0xC581B3FC)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStreamEndDetector : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			static const OpenViBE::CIdentifier id_InputEBML() { return OpenViBE::CIdentifier(0x0, 0x1); }
			static const OpenViBE::CIdentifier id_OutputStimulations() { return OpenViBE::CIdentifier(0x1, 0x1); }
			static const OpenViBE::CIdentifier id_SettingStimulationName() { return OpenViBE::CIdentifier(0x2, 0x1); }
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamEndDetector)

		protected:

			OpenViBEToolkit::TStreamStructureDecoder<CBoxAlgorithmStreamEndDetector> m_StructureDecoder;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmStreamEndDetector> m_StimulationEncoder;

			uint64_t m_StimulationIdentifier = 0;
			uint64_t m_ActionIdentifier      = 0;

		private:
			enum class EEndState
			{
				WaitingForEnd,
				EndReceived,
				StimulationSent,
				Finished
			};

			uint64_t m_EndDate                 = 0;
			uint64_t m_CurrentChunkEndDate     = 0;
			uint64_t m_PreviousTime            = 0;
			uint32_t m_InputEBMLIndex          = 0;
			uint32_t m_OutputStimulationsIndex = 0;
			bool m_IsHeaderSent                = false;
			EEndState m_EndState;
		};

		class CBoxAlgorithmStreamEndDetectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stream End Detector"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jozef Legeny"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Sends a stimulation upon receiving an End chunk"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Sends a stimulation upon receiving an End chunk"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stimulation"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("2.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StreamEndDetector; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStreamEndDetector; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addInput("EBML Stream", OV_TypeId_EBMLStream, CBoxAlgorithmStreamEndDetector::id_InputEBML());
				rBoxAlgorithmPrototype.addOutput("Output Stimulations", OV_TypeId_Stimulations, CBoxAlgorithmStreamEndDetector::id_OutputStimulations());
				rBoxAlgorithmPrototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00", false, CBoxAlgorithmStreamEndDetector::id_SettingStimulationName());

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamEndDetectorDesc)
		};
	}
}

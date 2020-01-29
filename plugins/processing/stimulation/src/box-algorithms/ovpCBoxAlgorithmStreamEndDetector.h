#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStreamEndDetector final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			static OpenViBE::CIdentifier inputEBMLId() { return OpenViBE::CIdentifier(0x0, 0x1); }
			static OpenViBE::CIdentifier outputStimulationsID() { return OpenViBE::CIdentifier(0x1, 0x1); }
			static OpenViBE::CIdentifier settingStimulationNameID() { return OpenViBE::CIdentifier(0x2, 0x1); }
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamEndDetector)

		protected:

			OpenViBE::Toolkit::TStreamStructureDecoder<CBoxAlgorithmStreamEndDetector> m_decoder;
			OpenViBE::Toolkit::TStimulationEncoder<CBoxAlgorithmStreamEndDetector> m_encoder;

			uint64_t m_stimulationID = 0;
			uint64_t m_actionID      = 0;

		private:
			enum class EEndState
			{
				WaitingForEnd,
				EndReceived,
				StimulationSent,
				Finished
			};

			uint64_t m_endDate             = 0;
			uint64_t m_currentChunkEndDate = 0;
			uint64_t m_previousTime        = 0;
			size_t m_inputEBMLIdx          = 0;
			size_t m_outputStimulationsIdx = 0;
			bool m_isHeaderSent            = false;
			EEndState m_endState           = EEndState::WaitingForEnd;
		};

		class CBoxAlgorithmStreamEndDetectorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
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

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("EBML Stream", OV_TypeId_EBMLStream, CBoxAlgorithmStreamEndDetector::inputEBMLId());
				prototype.addOutput("Output Stimulations", OV_TypeId_Stimulations, CBoxAlgorithmStreamEndDetector::outputStimulationsID());
				prototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00", false,
									 CBoxAlgorithmStreamEndDetector::settingStimulationNameID());

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamEndDetectorDesc)
		};
	} // namespace Stimulation
} // namespace OpenViBEPlugins

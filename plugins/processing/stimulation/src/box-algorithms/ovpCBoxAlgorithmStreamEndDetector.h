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
		class CBoxAlgorithmStreamEndDetector : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			static const OpenViBE::CIdentifier id_InputEBML() { return OpenViBE::CIdentifier(0x0, 0x1); }
			static const OpenViBE::CIdentifier id_OutputStimulations() { return OpenViBE::CIdentifier(0x1, 0x1); }
			static const OpenViBE::CIdentifier id_SettingStimulationName() { return OpenViBE::CIdentifier(0x2, 0x1); }

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(OpenViBE::uint32 ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamEndDetector);

		protected:

			OpenViBEToolkit::TStreamStructureDecoder<CBoxAlgorithmStreamEndDetector> m_StructureDecoder;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmStreamEndDetector> m_StimulationEncoder;

			uint64_t m_StimulationIdentifier;
			uint64_t m_ActionIdentifier;

		private:
			enum class EEndState
			{
				WaitingForEnd,
				EndReceived,
				StimulationSent,
				Finished
			};

			uint64_t m_EndDate;
			uint64_t m_CurrentChunkEndDate;
			uint64_t m_PreviousTime;
			uint32_t m_InputEBMLIndex;
			uint32_t m_OutputStimulationsIndex;
			bool m_IsHeaderSent;
			EEndState m_EndState;
		};

		class CBoxAlgorithmStreamEndDetectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stream End Detector"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Jozef Legeny"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Mensia Technologies"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Sends a stimulation upon receiving an End chunk"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Sends a stimulation upon receiving an End chunk"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("2.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StreamEndDetector; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmStreamEndDetector; }

			virtual bool getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("EBML Stream", OV_TypeId_EBMLStream, CBoxAlgorithmStreamEndDetector::id_InputEBML());
				rBoxAlgorithmPrototype.addOutput("Output Stimulations", OV_TypeId_Stimulations, CBoxAlgorithmStreamEndDetector::id_OutputStimulations());
				rBoxAlgorithmPrototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00", false, CBoxAlgorithmStreamEndDetector::id_SettingStimulationName());

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamEndDetectorDesc);
		};
	};
};


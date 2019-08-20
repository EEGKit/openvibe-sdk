#pragma once

#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest                                                OpenViBE::CIdentifier(0x87D18C62, 0xF2DAF779)
#define OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTestDesc                                            OpenViBE::CIdentifier(0x95E27325, 0x6893A519)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEncoderAlgorithmTest : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			uint64_t getClockFrequency() override { return 1LL << 32; }
			bool initialize() override;
			virtual bool uininitialize();
			bool processClock(OpenViBE::Kernel::IMessageClock& rMessageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder[7];
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer[7];

			bool m_bHasSentHeader    = false;
			uint64_t m_ui64StartTime = 0;
			uint64_t m_ui64EndTime   = 0;

			OpenViBE::CMatrix* m_pMatrix1                = nullptr;
			OpenViBE::CMatrix* m_pMatrix2                = nullptr;
			OpenViBE::CMatrix* m_pMatrix3                = nullptr;
			OpenViBE::CStimulationSet* m_pStimulationSet = nullptr;
		};

		class CEncoderAlgorithmTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Encoder algorithm test"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Setups various streams and outputs them"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Note: Data sent in the streams does not change over time"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Tests/Algorithms"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CEncoderAlgorithmTest(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addOutput("Experiment information", OV_TypeId_ExperimentInformation);
				rPrototype.addOutput("Feature vector", OV_TypeId_FeatureVector);
				rPrototype.addOutput("Signal", OV_TypeId_Signal);
				rPrototype.addOutput("Spectrum", OV_TypeId_Spectrum);
				rPrototype.addOutput("Stimulation", OV_TypeId_Stimulations);
				rPrototype.addOutput("Streamed matrix", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTestDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins

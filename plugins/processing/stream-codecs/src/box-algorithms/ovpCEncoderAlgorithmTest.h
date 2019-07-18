#ifndef __OpenViBEPlugins_BoxAlgorithms_CEncoderAlgorithmTest_H__
#define __OpenViBEPlugins_BoxAlgorithms_CEncoderAlgorithmTest_H__

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

			virtual void release(void) { delete this; }

			virtual uint64_t getClockFrequency(void) { return 1LL << 32; }

			virtual bool initialize(void);
			virtual bool uininitialize(void);

			virtual bool processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder[7];
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer[7];

			bool m_bHasSentHeader;
			uint64_t m_ui64StartTime;
			uint64_t m_ui64EndTime;

			OpenViBE::CMatrix* m_pMatrix1;
			OpenViBE::CMatrix* m_pMatrix2;
			OpenViBE::CMatrix* m_pMatrix3;
			OpenViBE::CStimulationSet* m_pStimulationSet;
		};

		class CEncoderAlgorithmTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Encoder algorithm test"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Setups various streams and outputs them"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Note: Data sent in the streams does not change over time"); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Tests/Algorithms"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::StreamCodecs::CEncoderAlgorithmTest(); }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithms_CEncoderAlgorithmTest_H__

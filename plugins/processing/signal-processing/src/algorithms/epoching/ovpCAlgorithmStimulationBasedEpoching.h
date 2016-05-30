#ifndef __OpenViBEPlugins_Algorithm_StimulationBasedEpoching_H__
#define __OpenViBEPlugins_Algorithm_StimulationBasedEpoching_H__

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_Algorithm_StimulationBasedEpoching                                 OpenViBE::CIdentifier(0x421E3F46, 0x12003E6C)
#define OVP_ClassId_Algorithm_StimulationBasedEpochingDesc                             OpenViBE::CIdentifier(0x2EAF37FC, 0x63195EB8)

#define OVP_Algorithm_StimulationBasedEpoching_InputParameterId_InputSignal            OpenViBE::CIdentifier(0x0ED5C92B, 0xE16BEF25)
#define OVP_Algorithm_StimulationBasedEpoching_InputParameterId_OffsetSampleCount      OpenViBE::CIdentifier(0x7646CE65, 0xE128FC4E)
#define OVP_Algorithm_StimulationBasedEpoching_OutputParameterId_OutputSignal          OpenViBE::CIdentifier(0x00D331A2, 0xC13DF043)
#define OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_Reset                    OpenViBE::CIdentifier(0x6BA44128, 0x418CF901)
#define OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_PerformEpoching          OpenViBE::CIdentifier(0xD05579B5, 0x2649A4B2)
#define OVP_Algorithm_StimulationBasedEpoching_OutputTriggerId_EpochingDone            OpenViBE::CIdentifier(0x755BC3FE, 0x24F7B50F)
#define OVP_Algorithm_StimulationBasedEpoching_InputParameterId_EndTimeChunkToProcess  OpenViBE::CIdentifier(0x8B552604, 0x10CD1F94)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CAlgorithmStimulationBasedEpoching : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_StimulationBasedEpoching);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pInputSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OffsetSampleCount;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pOutputSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64EndTimeChunkToProcess;

			OpenViBE::uint64 m_ui64ReceivedSamples;
			OpenViBE::uint64 m_ui64SamplesToSkip;
			OpenViBE::uint64 m_ui64TimeLastProcessedChunk;
		};

		class CAlgorithmStimulationBasedEpochingDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stimulation based epoching"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Epoching"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_StimulationBasedEpoching; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CAlgorithmStimulationBasedEpoching; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_StimulationBasedEpoching_InputParameterId_InputSignal,       "Input signal",              OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_StimulationBasedEpoching_InputParameterId_OffsetSampleCount, "Offset sample count", OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_StimulationBasedEpoching_InputParameterId_EndTimeChunkToProcess, "End time of the chunk one wants to process", OpenViBE::Kernel::ParameterType_Integer);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StimulationBasedEpoching_OutputParameterId_OutputSignal,     "Output signal",             OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_Reset,               "Reset");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_PerformEpoching,     "Perform epoching");

				rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_StimulationBasedEpoching_OutputTriggerId_EpochingDone,       "Epoching done");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_StimulationBasedEpochingDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_StimulationBasedEpoching_H__

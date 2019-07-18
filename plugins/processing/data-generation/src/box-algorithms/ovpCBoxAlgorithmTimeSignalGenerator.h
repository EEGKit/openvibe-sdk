#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_TimeSignalGeneratorDesc                 OpenViBE::CIdentifier(0x57AD8655, 0x1966B4DC)

#define OVP_ClassId_BoxAlgorithm_TimeSignalGenerator                     OpenViBE::CIdentifier(0x28A5E7FF, 0x530095DE)

namespace OpenViBEPlugins
{
	namespace DataGeneration
	{
		class CBoxAlgorithmTimeSignalGenerator final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmTimeSignalGenerator(void);

			void release(void);

			uint64_t getClockFrequency(void);

			bool initialize(void);
			bool uninitialize(void);

			bool processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeSignalGenerator)

		protected:

			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmTimeSignalGenerator> m_oSignalEncoder;

			bool m_bHeaderSent;
			uint32_t m_ui32SamplingFrequency;
			uint32_t m_ui32GeneratedEpochSampleCount;
			uint32_t m_ui32SentSampleCount;
		};

		class CBoxAlgorithmTimeSignalGeneratorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			void release(void) { }
			OpenViBE::CString getName(void) const { return OpenViBE::CString("Time signal"); }
			OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Simple time signal generator (for use with DSP)"); }
			OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Data generation"); }
			OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.4.0"); }

			OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_TimeSignalGenerator; }
			OpenViBE::Plugins::IPluginObject* create(void) { return new CBoxAlgorithmTimeSignalGenerator(); }

			bool getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addOutput("Generated signal", OV_TypeId_Signal);

				rPrototype.addSetting("Sampling frequency", OV_TypeId_Integer, "512");
				rPrototype.addSetting("Generated epoch sample count", OV_TypeId_Integer, "32");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeSignalGeneratorDesc)
		};
	};
};

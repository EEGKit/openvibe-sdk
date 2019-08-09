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

			CBoxAlgorithmTimeSignalGenerator();

			void release() override;

			uint64_t getClockFrequency() override;

			bool initialize() override;
			bool uninitialize() override;

			bool processClock(OpenViBE::Kernel::IMessageClock& rMessageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeSignalGenerator)

		protected:

			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmTimeSignalGenerator> m_oSignalEncoder;

			bool m_bHeaderSent = false;
			uint32_t m_ui32SamplingFrequency = 0;
			uint32_t m_ui32GeneratedEpochSampleCount = 0;
			uint32_t m_ui32SentSampleCount = 0;
		};

		class CBoxAlgorithmTimeSignalGeneratorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Time signal"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Simple time signal generator (for use with DSP)"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Data generation"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.4.0"); }

			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TimeSignalGenerator; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmTimeSignalGenerator(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
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

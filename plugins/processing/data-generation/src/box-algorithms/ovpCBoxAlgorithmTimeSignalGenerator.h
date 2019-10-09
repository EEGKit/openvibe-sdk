#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>


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

			bool processClock(OpenViBE::Kernel::IMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeSignalGenerator)

		protected:

			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmTimeSignalGenerator> m_oSignalEncoder;

			bool m_headerSent                       = false;
			size_t m_samplingFrequency         = 0;
			size_t m_nGeneratedEpochSample = 0;
			size_t m_nSentSample           = 0;
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
	} // namespace DataGeneration
} // namespace OpenViBEPlugins

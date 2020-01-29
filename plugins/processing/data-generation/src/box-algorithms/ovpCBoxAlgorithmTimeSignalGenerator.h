#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>


namespace OpenViBEPlugins
{
	namespace DataGeneration
	{
		class CBoxAlgorithmTimeSignalGenerator final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmTimeSignalGenerator();

			void release() override;

			uint64_t getClockFrequency() override { return 128LL << 32; }

			bool initialize() override;
			bool uninitialize() override;

			bool processClock(OpenViBE::Kernel::IMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeSignalGenerator)

		protected:

			OpenViBE::Toolkit::TSignalEncoder<CBoxAlgorithmTimeSignalGenerator> m_encoder;

			bool m_headerSent              = false;
			size_t m_sampling              = 0;
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

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addOutput("Generated signal", OV_TypeId_Signal);

				prototype.addSetting("Sampling frequency", OV_TypeId_Integer, "512");
				prototype.addSetting("Generated epoch sample count", OV_TypeId_Integer, "32");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeSignalGeneratorDesc)
		};
	} // namespace DataGeneration
} // namespace OpenViBEPlugins

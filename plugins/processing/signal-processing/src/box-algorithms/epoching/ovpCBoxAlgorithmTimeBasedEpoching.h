#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmTimeBasedEpoching final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeBasedEpoching)

		private:

			OpenViBE::Toolkit::TSignalDecoder<CBoxAlgorithmTimeBasedEpoching> m_decoder;
			OpenViBE::Toolkit::TSignalEncoder<CBoxAlgorithmTimeBasedEpoching> m_encoder;

			double m_duration = 0;
			double m_interval = 0;

			size_t m_sampling             = 0;
			size_t m_oNSample             = 0;
			size_t m_oNSampleBetweenEpoch = 0;
			size_t m_oSampleIdx           = 0;
			size_t m_oChunkIdx            = 0;
			uint64_t m_lastInputEndTime   = 0;
			uint64_t m_referenceTime      = 0;
		};

		class CBoxAlgorithmTimeBasedEpochingDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Time based epoching"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Quentin Barthelemy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Generates signal 'slices' or 'blocks' having a specified duration and interval"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Interval can be used to control the overlap of epochs"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Epoching"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("2.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-cut"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TimeBasedEpoching; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmTimeBasedEpoching(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Epoched signal", OV_TypeId_Signal);
				prototype.addSetting("Epoch duration (in sec)", OV_TypeId_Float, "1");
				prototype.addSetting("Epoch intervals (in sec)", OV_TypeId_Float, "0.5");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeBasedEpochingDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

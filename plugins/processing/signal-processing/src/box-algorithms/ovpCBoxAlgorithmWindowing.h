#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>

#include <vector>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmWindowing final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Windowing)

		protected:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmWindowing> m_decoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmWindowing> m_encoder;

			uint64_t m_windowMethod = 0;
			std::vector<double> m_windowCoefficients;
		};

		class CBoxAlgorithmWindowingDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Windowing"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Laurent Bonnet"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Applies a windowing function to the signal."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Applies a windowing function to the signal."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-execute"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Windowing; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmWindowing(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rPrototype.addSetting("Window method", OVP_TypeId_WindowMethod, "Hamming");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_WindowingDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

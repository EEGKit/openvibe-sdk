#pragma once

#include <toolkit/ovtk_all.h>

#include <vector>

#define OVP_ClassId_Windowing         OpenViBE::CIdentifier(0x002034AE, 0x6509FD8F)
#define OVP_ClassId_WindowingDesc     OpenViBE::CIdentifier(0x602CF89F, 0x65BA6DA0)

// Windowing: ID names and values taken from the signal-processing-gpl project
#define OVP_TypeId_WindowMethod               OpenViBE::CIdentifier(0x0A430FE4, 0x4F318280)
#define OVP_TypeId_WindowMethod_None          OpenViBE::CIdentifier(0x01DD2ACC, 0x347E581E)
#define OVP_TypeId_WindowMethod_Hamming       OpenViBE::CIdentifier(0x3A9FF7F1, 0x54E79D67)
#define OVP_TypeId_WindowMethod_Hanning       OpenViBE::CIdentifier(0x660DA3E7, 0x7BD87719)
#define OVP_TypeId_WindowMethod_Hann          OpenViBE::CIdentifier(0x9BBBDC65, 0xFDFEF7A4)
#define OVP_TypeId_WindowMethod_Blackman      OpenViBE::CIdentifier(0x9D5937A4, 0xE43A9E3D)
#define OVP_TypeId_WindowMethod_Triangular    OpenViBE::CIdentifier(0xE652A852, 0xFE3CBC46)
#define OVP_TypeId_WindowMethod_SquareRoot    OpenViBE::CIdentifier(0xFE80BE5C, 0xAE59A27A)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmWindowing : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_Windowing)

		protected:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmWindowing> m_Decoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmWindowing> m_Encoder;

			uint64_t m_WindowMethod;
			std::vector<double> m_WindowCoefficients;
		};

		class CBoxAlgorithmWindowingDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
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
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins

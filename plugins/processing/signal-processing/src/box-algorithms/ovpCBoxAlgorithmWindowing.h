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

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_Windowing)

		protected:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmWindowing> m_Decoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmWindowing> m_Encoder;

			OpenViBE::uint64 m_WindowMethod;
			std::vector<double> m_WindowCoefficients;
		};

		class CBoxAlgorithmWindowingDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Windowing"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Applies a windowing function to the signal."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Applies a windowing function to the signal."); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getStockItemName(void) const { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Windowing; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmWindowing(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);

				rPrototype.addOutput("Output signal", OV_TypeId_Signal);

				rPrototype.addSetting("Window method", OVP_TypeId_WindowMethod, "Hamming");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_WindowingDesc)
		};
	}
}

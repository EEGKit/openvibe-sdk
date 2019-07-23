#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_ReferenceChannel     OpenViBE::CIdentifier(0x444721AD, 0x78342CF5)
#define OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc OpenViBE::CIdentifier(0x42856103, 0x45B125AD)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmReferenceChannel : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ReferenceChannel)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmReferenceChannel> m_oDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmReferenceChannel> m_oEncoder;
			uint32_t m_ui32ReferenceChannelIndex;
		};

		class CBoxAlgorithmReferenceChannelDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Reference Channel"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Subtracts the value of the reference channel from all other channels"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Reference channel must be specified as a parameter for the box"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Channels"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_ReferenceChannel; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmReferenceChannel; }
			// virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const               { return new CBoxAlgorithmReferenceChannelListener; }
			// virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Channel", OV_TypeId_String, "Ref_Nose");
				rBoxAlgorithmPrototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, OVP_TypeId_MatchMethod_Smart.toString());
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins

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
		class CBoxAlgorithmReferenceChannel final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ReferenceChannel)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmReferenceChannel> m_oDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmReferenceChannel> m_oEncoder;
			uint32_t m_ui32ReferenceChannelIndex = 0;
		};

		class CBoxAlgorithmReferenceChannelDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Reference Channel"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Subtracts the value of the reference channel from all other channels");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Reference channel must be specified as a parameter for the box");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Channels"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ReferenceChannel; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmReferenceChannel; }
			// virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const               { return new CBoxAlgorithmReferenceChannelListener; }
			// virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const { delete listener; }
			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Output signal", OV_TypeId_Signal);
				prototype.addSetting("Channel", OV_TypeId_String, "Ref_Nose");
				prototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, OVP_TypeId_MatchMethod_Smart.toString());
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

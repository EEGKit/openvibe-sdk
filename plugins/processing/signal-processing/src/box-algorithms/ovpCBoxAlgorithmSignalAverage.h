#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>

#include <string>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		/**
		*/
		class CBoxAlgorithmSignalAverage final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmSignalAverage() {}
			void release() override {}
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SignalAverage)

			void computeAverage();

			// Needed to read the input and write the output
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalAverage> m_Decoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalAverage> m_Encoder;
		};

		/**
		* Description of the channel selection plugin
		*/
		class CSignalAverageDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Signal average"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Bruno Renier"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Computes the average of each input buffer."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Averaging"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.5"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalAverage; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSignalAverage(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addOutput("Filtered signal", OV_TypeId_Signal);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalAverageDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

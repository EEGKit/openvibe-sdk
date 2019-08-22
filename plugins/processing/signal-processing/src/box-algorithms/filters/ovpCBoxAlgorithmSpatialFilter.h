#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>

#define OVP_ClassId_BoxAlgorithm_SpatialFilter     OpenViBE::CIdentifier(0xDD332C6C, 0x195B4FD4)
#define OVP_ClassId_BoxAlgorithm_SpatialFilterDesc OpenViBE::CIdentifier(0x72A01C92, 0xF8C1FA24)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpatialFilter : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SpatialFilter)

		protected:

			OpenViBEToolkit::TDecoder<CBoxAlgorithmSpatialFilter>* m_pStreamDecoder = nullptr;
			OpenViBEToolkit::TEncoder<CBoxAlgorithmSpatialFilter>* m_pStreamEncoder = nullptr;

			OpenViBE::CMatrix m_oFilterBank;

		private:
			// Loads the m_vCoefficient vector (representing a matrix) from the given string. c1 and c2 are separator characters between floats.
			uint32_t loadCoefficients(const OpenViBE::CString& rCoefficients, char c1, char c2, uint32_t nRows, uint32_t nCols);
		};

		class CBoxAlgorithmSpatialFilterListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(0, l_oTypeIdentifier);
				rBox.setOutputType(0, l_oTypeIdentifier);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				rBox.setInputType(0, l_oTypeIdentifier);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmSpatialFilterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spatial Filter"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard, Jussi T. Lindgren"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Maps M inputs to N outputs by multiplying the each input vector with a matrix"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("The applied coefficient matrix must be specified as a box parameter. The filter processes each sample independently of the past samples."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SpatialFilter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSpatialFilter; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSpatialFilterListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addInput("Input Signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output Signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Spatial Filter Coefficients", OV_TypeId_String, "1;0;0;0;0;1;0;0;0;0;1;0;0;0;0;1");
				rBoxAlgorithmPrototype.addSetting("Number of Output Channels", OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Number of Input Channels", OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Filter matrix file", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpatialFilterDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

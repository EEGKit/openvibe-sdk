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
		class CBoxAlgorithmSpatialFilter final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
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

		class CBoxAlgorithmSpatialFilterListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(0, typeID);
				box.setOutputType(0, typeID);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);
				box.setInputType(0, typeID);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmSpatialFilterDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spatial Filter"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard, Jussi T. Lindgren"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Maps M inputs to N outputs by multiplying the each input vector with a matrix");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"The applied coefficient matrix must be specified as a box parameter. The filter processes each sample independently of the past samples.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SpatialFilter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSpatialFilter; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSpatialFilterListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input Signal", OV_TypeId_Signal);
				prototype.addOutput("Output Signal", OV_TypeId_Signal);
				prototype.addSetting("Spatial Filter Coefficients", OV_TypeId_String, "1;0;0;0;0;1;0;0;0;0;1;0;0;0;0;1");
				prototype.addSetting("Number of Output Channels", OV_TypeId_Integer, "4");
				prototype.addSetting("Number of Input Channels", OV_TypeId_Integer, "4");
				prototype.addSetting("Filter matrix file", OV_TypeId_Filename, "");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_Spectrum);
				prototype.addInputSupport(OV_TypeId_Signal);

				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				prototype.addOutputSupport(OV_TypeId_Spectrum);
				prototype.addOutputSupport(OV_TypeId_Signal);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpatialFilterDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

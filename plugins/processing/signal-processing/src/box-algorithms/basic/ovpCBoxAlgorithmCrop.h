#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_Crop                                                  OpenViBE::CIdentifier(0x7F1A3002, 0x358117BA)
#define OVP_ClassId_BoxAlgorithm_CropDesc                                              OpenViBE::CIdentifier(0x64D619D7, 0x26CC42C9)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmCrop : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Crop)

		protected:

			OpenViBE::IMatrix* m_pMatrix                        = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder = nullptr;
			double m_f64MinCropValue                            = 0;
			double m_f64MaxCropValue                            = 0;
			uint64_t m_ui64CropMethod                           = 0;
		};

		class CBoxAlgorithmCropListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(index, l_oTypeIdentifier);
				rBox.setOutputType(index, l_oTypeIdentifier);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getOutputType(index, l_oTypeIdentifier);
				rBox.setInputType(index, l_oTypeIdentifier);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmCropDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Crop"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Truncates signal values to a specified range"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Minimum or maximum or both limits can be specified"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Basic"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Crop; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmCrop; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmCropListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input matrix", OV_TypeId_StreamedMatrix);
				prototype.addOutput("Output matrix", OV_TypeId_StreamedMatrix);
				prototype.addSetting("Crop method", OVP_TypeId_CropMethod, OVP_TypeId_CropMethod_MinMax.toString());
				prototype.addSetting("Min crop value", OV_TypeId_Float, "-1");
				prototype.addSetting("Max crop value", OV_TypeId_Float, "1");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Spectrum);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_FeatureVector);

				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Spectrum);
				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				prototype.addOutputSupport(OV_TypeId_FeatureVector);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CropDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

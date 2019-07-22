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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Crop)

		protected:

			OpenViBE::IMatrix* m_pMatrix;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			double m_f64MinCropValue;
			double m_f64MaxCropValue;
			uint64_t m_ui64CropMethod;
		};

		class CBoxAlgorithmCropListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				return true;
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmCropDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Crop"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Truncates signal values to a specified range"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Minimum or maximum or both limits can be specified"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_Crop; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmCrop; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmCropListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput("Output matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Crop method", OVP_TypeId_CropMethod, OVP_TypeId_CropMethod_MinMax.toString());
				rBoxAlgorithmPrototype.addSetting("Min crop value", OV_TypeId_Float, "-1");
				rBoxAlgorithmPrototype.addSetting("Max crop value", OV_TypeId_Float, "1");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_FeatureVector);

				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_FeatureVector);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CropDesc)
		};
	}
}



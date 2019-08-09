#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_EpochAverage                                          OpenViBE::CIdentifier(0x21283D9F, 0xE76FF640)
#define OVP_ClassId_BoxAlgorithm_EpochAverageDesc                                      OpenViBE::CIdentifier(0x95F5F43E, 0xBE629D82)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmEpochAverage : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EpochAverage)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pMatrixAverage = nullptr;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64MatrixCount;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64AveragingMethod;
		};

		class CBoxAlgorithmEpochAverageListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmEpochAverageDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Epoch average"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Averages matrices among time, this can be used to enhance ERPs"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("This box can average matrices of different types including signal, spectrum or feature vectors"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Averaging"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_EpochAverage; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmEpochAverage(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmEpochAverageListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input epochs", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput("Averaged epochs", OV_TypeId_StreamedMatrix);
				rPrototype.addSetting("Averaging type", OVP_TypeId_EpochAverageMethod, OVP_TypeId_EpochAverageMethod_MovingAverage.toString());
				rPrototype.addSetting("Epoch count", OV_TypeId_Integer, "4");
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				rPrototype.addInputSupport(OV_TypeId_Signal);
				rPrototype.addInputSupport(OV_TypeId_Spectrum);
				rPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				rPrototype.addInputSupport(OV_TypeId_FeatureVector);
				rPrototype.addInputSupport(OV_TypeId_TimeFrequency);

				rPrototype.addOutputSupport(OV_TypeId_Signal);
				rPrototype.addOutputSupport(OV_TypeId_Spectrum);
				rPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				rPrototype.addOutputSupport(OV_TypeId_FeatureVector);
				rPrototype.addOutputSupport(OV_TypeId_TimeFrequency);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EpochAverageDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins

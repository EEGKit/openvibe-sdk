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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EpochAverage)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pMatrixAverage;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64MatrixCount;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64AveragingMethod;
		};

		class CBoxAlgorithmEpochAverageListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				return true;
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
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

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Epoch average"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Averages matrices among time, this can be used to enhance ERPs"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("This box can average matrices of different types including signal, spectrum or feature vectors"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Averaging"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_EpochAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmEpochAverage(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmEpochAverageListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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

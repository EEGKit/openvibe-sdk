#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Eigen>

#define OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer                                        OpenViBE::CIdentifier(0x27542F6E, 0x14AA3548)
#define OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc                                    OpenViBE::CIdentifier(0x128A6013, 0x370B5C2C)

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmXDAWNTrainer : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			CBoxAlgorithmXDAWNTrainer();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer)

		protected:

			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmXDAWNTrainer> m_StimDecoder;
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmXDAWNTrainer> m_SignalDecoder[2];
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmXDAWNTrainer> m_StimEncoder;

			uint64_t m_TrainStimulationId = 0;
			OpenViBE::CString m_FilterFilename;
			uint32_t m_FilterDimension = 0;
			bool m_SaveAsBoxConfig     = false;
		};

		class CBoxAlgorithmXDAWNTrainerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override {}
			OpenViBE::CString getName() const override { return OpenViBE::CString("xDAWN Trainer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Spatial Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-zoom-100"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmXDAWNTrainer; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Stimulations", OV_TypeId_Stimulations);
				prototype.addInput("Session signal", OV_TypeId_Signal);
				prototype.addInput("Evoked potential epochs", OV_TypeId_Signal);

				prototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

				prototype.addSetting("Train stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
				prototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
				prototype.addSetting("Filter dimension", OV_TypeId_Integer, "4");
				prototype.addSetting("Save as box config", OV_TypeId_Boolean, "true");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

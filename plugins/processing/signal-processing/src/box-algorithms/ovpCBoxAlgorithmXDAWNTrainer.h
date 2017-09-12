#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Eigen>

#define OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer                                        OpenViBE::CIdentifier(0x27542F6E, 0x14AA3548)
#define OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc                                    OpenViBE::CIdentifier(0x128A6013, 0x370B5C2C)

typedef Eigen::Matrix< double , Eigen::Dynamic , Eigen::Dynamic, Eigen::RowMajor > MatrixXdRowMajor;

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmXDAWNTrainer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			CBoxAlgorithmXDAWNTrainer(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer);

		protected:

			OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmXDAWNTrainer > m_StimDecoder;
			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmXDAWNTrainer > m_SignalDecoder[2];
			OpenViBEToolkit::TStimulationEncoder < CBoxAlgorithmXDAWNTrainer > m_StimEncoder;

			uint64_t m_TrainStimulationId;
			OpenViBE::CString m_FilterFilename;
			uint32_t m_FilterDimension;
			bool m_SaveAsBoxConfig;			
		};

		class CBoxAlgorithmXDAWNTrainerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) {}

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("xDAWN Trainer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Spatial Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-zoom-100"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmXDAWNTrainer; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Session signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Evoked potential epochs", OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Train stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
				rBoxAlgorithmPrototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Filter dimension", OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Save as box config", OV_TypeId_Boolean, "true");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc);
		};
	};
};

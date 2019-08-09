#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "../../algorithms/basic/ovpCAlgorithmOnlineCovariance.h"

#define OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainer      OpenViBE::CIdentifier(0x2EC14CC0, 0x428C48BD)
#define OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainerDesc  OpenViBE::CIdentifier(0x02205F54, 0x733C51EE)

#include <Eigen/Eigenvalues>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;


namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmRegularizedCSPTrainer : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			CBoxAlgorithmRegularizedCSPTrainer();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

		protected:

			virtual bool updateCov(uint32_t index);
			virtual bool outclassCovAverage(uint32_t skipIndex, const std::vector<Eigen::MatrixXd>& cov, Eigen::MatrixXd& covAvg);
			virtual bool computeCSP(const std::vector<Eigen::MatrixXd>& vov, std::vector<Eigen::MatrixXd>& sortedEigenVectors,
									std::vector<Eigen::VectorXd>& sortedEigenValues);

			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmRegularizedCSPTrainer> m_StimulationDecoder;

			std::vector<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmRegularizedCSPTrainer>> m_SignalDecoders;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmRegularizedCSPTrainer> m_StimulationEncoder;

			uint64_t m_StimulationIdentifier;
			OpenViBE::CString m_SpatialFilterConfigurationFilename;
			uint32_t m_FiltersPerClass;
			bool m_SaveAsBoxConf;
			bool m_HasBeenInitialized;

			double m_Tikhonov;

			struct IncrementalCovarianceProxy
			{
				IncrementalCovarianceProxy() : incrementalCov(nullptr), numBuffers(0), numSamples(0) {}
				OpenViBE::Kernel::IAlgorithmProxy* incrementalCov;
				uint64_t numBuffers;
				uint64_t numSamples;
			};

			std::vector<IncrementalCovarianceProxy> m_IncCovarianceProxies;

			uint32_t m_NumClasses;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainer)
		};

		class CBoxAlgorithmRegularizedCSPTrainerListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override
			{
				std::stringstream l_sName;

				l_sName << "Signal condition " << ui32Index;

				rBox.setInputName(ui32Index, l_sName.str().c_str());

				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmRegularizedCSPTrainerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Regularized CSP Trainer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jussi T. Lindgren"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Computes Common Spatial Pattern filters with regularization"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainer; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmRegularizedCSPTrainer; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmRegularizedCSPTrainerListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addInput("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Signal condition 1", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Signal condition 2", OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addSetting("Train Trigger", OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Session");
				rBoxAlgorithmPrototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Filters per condition", OV_TypeId_Integer, "2");
				rBoxAlgorithmPrototype.addSetting("Save filters as box config", OV_TypeId_Boolean, "false");

				// Params of the cov algorithm; would be better to poll the params from the algorithm, however this is not straightforward to do
				rBoxAlgorithmPrototype.addSetting("Covariance update", OVP_TypeId_OnlineCovariance_UpdateMethod, OVP_TypeId_OnlineCovariance_UpdateMethod_ChunkAverage.toString());
				rBoxAlgorithmPrototype.addSetting("Trace normalization", OV_TypeId_Boolean, "false");
				rBoxAlgorithmPrototype.addSetting("Shrinkage coefficient", OV_TypeId_Float, "0.0");
				rBoxAlgorithmPrototype.addSetting("Tikhonov coefficient", OV_TypeId_Float, "0.0");

				rBoxAlgorithmPrototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainerDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins

#endif // TARGET_HAS_ThirdPartyEIGEN

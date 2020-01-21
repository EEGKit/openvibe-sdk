#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "../../algorithms/basic/ovpCAlgorithmOnlineCovariance.h"

#include <Eigen/Eigenvalues>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmRegularizedCSPTrainer final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			CBoxAlgorithmRegularizedCSPTrainer();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

		protected:

			bool updateCov(size_t index);
			bool outclassCovAverage(size_t skipIndex, const std::vector<Eigen::MatrixXd>& cov, Eigen::MatrixXd& covAvg);
			bool computeCSP(const std::vector<Eigen::MatrixXd>& cov, std::vector<Eigen::MatrixXd>& sortedEigenVectors,
							std::vector<Eigen::VectorXd>& sortedEigenValues);

			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmRegularizedCSPTrainer> m_stimDecoder;
			std::vector<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmRegularizedCSPTrainer>> m_signalDecoders;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmRegularizedCSPTrainer> m_encoder;

			uint64_t m_stimID = 0;
			OpenViBE::CString m_configFilename;
			size_t m_filtersPerClass  = 0;
			bool m_saveAsBoxConf      = false;
			bool m_hasBeenInitialized = false;

			double m_tikhonov = 0;

			struct SIncrementalCovarianceProxy
			{
				OpenViBE::Kernel::IAlgorithmProxy* cov = nullptr;
				size_t nBuffers                        = 0;
				size_t nSamples                        = 0;
			};

			std::vector<SIncrementalCovarianceProxy> m_covProxies;

			size_t m_nClasses = 0;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainer)
		};

		class CBoxAlgorithmRegularizedCSPTrainerListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				box.setInputName(index, ("Signal condition " + std::to_string(index)).c_str());
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmRegularizedCSPTrainerDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
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

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Stimulations", OV_TypeId_Stimulations);
				prototype.addInput("Signal condition 1", OV_TypeId_Signal);
				prototype.addInput("Signal condition 2", OV_TypeId_Signal);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);

				prototype.addSetting("Train Trigger", OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Session");
				prototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
				prototype.addSetting("Filters per condition", OV_TypeId_Integer, "2");
				prototype.addSetting("Save filters as box config", OV_TypeId_Boolean, "false");

				// Params of the cov algorithm; would be better to poll the params from the algorithm, however this is not straightforward to do
				prototype.addSetting("Covariance update", OVP_TypeId_OnlineCovariance_UpdateMethod, "Chunk average");
				prototype.addSetting("Trace normalization", OV_TypeId_Boolean, "false");
				prototype.addSetting("Shrinkage coefficient", OV_TypeId_Float, "0.0");
				prototype.addSetting("Tikhonov coefficient", OV_TypeId_Float, "0.0");

				prototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainerDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins

#endif // TARGET_HAS_ThirdPartyEIGEN

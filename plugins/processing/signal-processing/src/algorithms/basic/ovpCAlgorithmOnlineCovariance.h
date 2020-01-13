/*
 *
 * Incremental covariance estimators with shrinkage
 *
 */
#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Dense>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CAlgorithmOnlineCovariance final : virtual public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_OnlineCovariance)

		protected:
			// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
			static void dumpMatrix(OpenViBE::Kernel::ILogManager& mgr, const MatrixXdRowMajor& mat, const OpenViBE::CString& desc);

			// These are non-normalized estimates for the corresp. statistics
			Eigen::MatrixXd m_cov;
			Eigen::MatrixXd m_mean;

			// The divisor for the above estimates to do the normalization
			uint64_t m_n = 0;
		};

		class CAlgorithmOnlineCovarianceDesc final : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Online Covariance"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jussi T. Lindgren"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Incrementally computes covariance with shrinkage."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Regularized covariance output is computed as (diag*shrink + cov)");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.5"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_OnlineCovariance; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmOnlineCovariance; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(
					OVP_Algorithm_OnlineCovariance_InputParameterId_Shrinkage, "Shrinkage", OpenViBE::Kernel::ParameterType_Float);
				prototype.addInputParameter(
					OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors, "Input vectors", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(
					OVP_Algorithm_OnlineCovariance_InputParameterId_UpdateMethod, "Cov update method", OpenViBE::Kernel::ParameterType_Enumeration,
					OVP_TypeId_OnlineCovariance_UpdateMethod);
				prototype.addInputParameter(
					OVP_Algorithm_OnlineCovariance_InputParameterId_TraceNormalization, "Trace normalization", OpenViBE::Kernel::ParameterType_Boolean);

				// The algorithm returns these outputs
				prototype.addOutputParameter(
					OVP_Algorithm_OnlineCovariance_OutputParameterId_Mean, "Mean vector", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addOutputParameter(
					OVP_Algorithm_OnlineCovariance_OutputParameterId_CovarianceMatrix, "Covariance matrix", OpenViBE::Kernel::ParameterType_Matrix);

				prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Reset, "Reset the algorithm");
				prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Update, "Append a chunk of data");
				prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCov, "Get the current regularized covariance matrix & mean");
				prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCovRaw, "Get the current covariance matrix & mean");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_OnlineCovarianceDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins


#endif // TARGET_HAS_ThirdPartyEIGEN

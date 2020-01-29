#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Dense>

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmConditionedCovariance final : virtual public OpenViBE::Toolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override { return true; }
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ConditionedCovariance)

		protected:
			// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
			static void dumpMatrix(OpenViBE::Kernel::ILogManager& mgr, const MatrixXdRowMajor& mat, const OpenViBE::CString& desc);
		};

		class CAlgorithmConditionedCovarianceDesc final : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Conditioned Covariance"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jussi T. Lindgren"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Computes covariance with shrinkage."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"Shrinkage: {<0 = auto-estimate, [0,1] balance between prior and sample cov}. The conditioned covariance matrix may allow better accuracies with models that rely on inverting the cov matrix, in cases where the regular cov matrix is close to singular.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ConditionedCovariance; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmConditionedCovariance; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(
					OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage, "Shrinkage (-1 == auto)", OpenViBE::Kernel::ParameterType_Float);
				prototype.addInputParameter(
					OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet, "Feature vectors", OpenViBE::Kernel::ParameterType_Matrix);

				// The algorithm returns these outputs
				prototype.addOutputParameter(
					OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean, "Mean vector", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addOutputParameter(
					OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix, "Covariance matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ConditionedCovarianceDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins


#endif // TARGET_HAS_ThirdPartyEIGEN

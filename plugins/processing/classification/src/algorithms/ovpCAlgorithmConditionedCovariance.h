#if defined TARGET_HAS_ThirdPartyEIGEN

#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Dense>

#define OVP_ClassId_Algorithm_ConditionedCovariance                                           OpenViBE::CIdentifier(0x0F3B77A6, 0x0301518A)
#define OVP_ClassId_Algorithm_ConditionedCovarianceDesc                                       OpenViBE::CIdentifier(0x18D15C41, 0x70545A66)

#define OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage                        OpenViBE::CIdentifier(0x54B90EA7, 0x600A4ACC)
#define OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet                 OpenViBE::CIdentifier(0x2CF30E42, 0x051F3996)

#define OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean                            OpenViBE::CIdentifier(0x0C671FB7, 0x550B01B3)
#define OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix                OpenViBE::CIdentifier(0x19F07FB4, 0x084E273B)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmConditionedCovariance final : virtual public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ConditionedCovariance)

		protected:
			// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
			void dumpMatrix(OpenViBE::Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const OpenViBE::CString& desc);
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

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage, "Shrinkage (-1 == auto)", OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet, "Feature vectors", OpenViBE::Kernel::ParameterType_Matrix);

				// The algorithm returns these outputs
				rAlgorithmPrototype.addOutputParameter(
					OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean, "Mean vector", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addOutputParameter(
					OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix, "Covariance matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ConditionedCovarianceDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins


#endif // TARGET_HAS_ThirdPartyEIGEN

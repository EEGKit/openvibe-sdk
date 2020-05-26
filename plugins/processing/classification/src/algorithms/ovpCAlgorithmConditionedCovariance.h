#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Dense>

namespace OpenViBE {
namespace Plugins {
namespace Classification {
class CAlgorithmConditionedCovariance final : virtual public Toolkit::TAlgorithm<IAlgorithm>
{
	typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override { return true; }
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TAlgorithm<IAlgorithm>, OVP_ClassId_Algorithm_ConditionedCovariance)

protected:
	// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
	static void dumpMatrix(Kernel::ILogManager& mgr, const MatrixXdRowMajor& mat, const CString& desc);
};

class CAlgorithmConditionedCovarianceDesc final : virtual public IAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Conditioned Covariance"); }
	CString getAuthorName() const override { return CString("Jussi T. Lindgren"); }
	CString getAuthorCompanyName() const override { return CString("Inria"); }
	CString getShortDescription() const override { return CString("Computes covariance with shrinkage."); }

	CString getDetailedDescription() const override
	{
		return CString(
			"Shrinkage: {<0 = auto-estimate, [0,1] balance between prior and sample cov}. The conditioned covariance matrix may allow better accuracies with models that rely on inverting the cov matrix, in cases where the regular cov matrix is close to singular.");
	}

	CString getCategory() const override { return CString(""); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ConditionedCovariance; }
	IPluginObject* create() override { return new CAlgorithmConditionedCovariance; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		prototype.addInputParameter(
			OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage, "Shrinkage (-1 == auto)", Kernel::ParameterType_Float);
		prototype.addInputParameter(
			OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet, "Feature vectors", Kernel::ParameterType_Matrix);

		// The algorithm returns these outputs
		prototype.addOutputParameter(
			OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean, "Mean vector", Kernel::ParameterType_Matrix);
		prototype.addOutputParameter(
			OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix, "Covariance matrix", Kernel::ParameterType_Matrix);

		return true;
	}

	_IsDerivedFromClass_Final_(IAlgorithmDesc, OVP_ClassId_Algorithm_ConditionedCovarianceDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE


#endif // TARGET_HAS_ThirdPartyEIGEN

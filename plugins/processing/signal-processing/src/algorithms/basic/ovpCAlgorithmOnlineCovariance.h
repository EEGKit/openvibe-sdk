/*
 *
 * Incremental covariance estimators with shrinkage
 *
 */
#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Dense>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CAlgorithmOnlineCovariance final : virtual public Toolkit::TAlgorithm<IAlgorithm>
{
	typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TAlgorithm<IAlgorithm>, OVP_ClassId_Algorithm_OnlineCovariance)

protected:
	// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
	static void dumpMatrix(Kernel::ILogManager& mgr, const MatrixXdRowMajor& mat, const CString& desc);

	// These are non-normalized estimates for the corresp. statistics
	Eigen::MatrixXd m_cov;
	Eigen::MatrixXd m_mean;

	// The divisor for the above estimates to do the normalization
	uint64_t m_n = 0;
};

class CAlgorithmOnlineCovarianceDesc final : virtual public IAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Online Covariance"; }
	CString getAuthorName() const override { return "Jussi T. Lindgren"; }
	CString getAuthorCompanyName() const override { return "Inria"; }
	CString getShortDescription() const override { return "Incrementally computes covariance with shrinkage."; }
	CString getDetailedDescription() const override { return "Regularized covariance output is computed as (diag*shrink + cov)"; }
	CString getCategory() const override { return ""; }
	CString getVersion() const override { return "0.5"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_OnlineCovariance; }
	IPluginObject* create() override { return new CAlgorithmOnlineCovariance; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		prototype.addInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_Shrinkage, "Shrinkage", Kernel::ParameterType_Float);
		prototype.addInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors, "Input vectors", Kernel::ParameterType_Matrix);
		prototype.addInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_UpdateMethod, "Cov update method", Kernel::ParameterType_Enumeration,
									OVP_TypeId_OnlineCovariance_UpdateMethod);
		prototype.addInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_TraceNormalization, "Trace normalization", Kernel::ParameterType_Boolean);

		// The algorithm returns these outputs
		prototype.addOutputParameter(OVP_Algorithm_OnlineCovariance_OutputParameterId_Mean, "Mean vector", Kernel::ParameterType_Matrix);
		prototype.addOutputParameter(OVP_Algorithm_OnlineCovariance_OutputParameterId_CovarianceMatrix, "Covariance matrix", Kernel::ParameterType_Matrix);

		prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Reset, "Reset the algorithm");
		prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Update, "Append a chunk of data");
		prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCov, "Get the current regularized covariance matrix & mean");
		prototype.addInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCovRaw, "Get the current covariance matrix & mean");

		return true;
	}

	_IsDerivedFromClass_Final_(IAlgorithmDesc, OVP_ClassId_Algorithm_OnlineCovarianceDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE


#endif // TARGET_HAS_ThirdPartyEIGEN

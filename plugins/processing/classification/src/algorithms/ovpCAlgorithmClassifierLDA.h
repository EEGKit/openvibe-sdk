#pragma once

#include "../ovp_defines.h"
#include "ovpCAlgorithmLDADiscriminantFunction.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <stack>

#include <Eigen/Dense>

namespace OpenViBE {
namespace Plugins {
namespace Classification {
class CAlgorithmLDADiscriminantFunction;

int LDAClassificationCompare(CMatrix& first, CMatrix& second);

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

class CAlgorithmClassifierLDA final : public Toolkit::CAlgorithmClassifier
{
public:
	bool initialize() override;
	bool uninitialize() override;
	bool train(const Toolkit::IFeatureVectorSet& dataset) override;
	bool classify(const Toolkit::IFeatureVector& sample, double& classId, Toolkit::IVector& distance, Toolkit::IVector& probability) override;
	XML::IXMLNode* saveConfig() override;
	bool loadConfig(XML::IXMLNode* configNode) override;
	size_t getNProbabilities() override { return m_discriminantFunctions.size(); }
	size_t getNDistances() override { return m_discriminantFunctions.size(); }

	_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierLDA)

protected:
	// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
	static void dumpMatrix(Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const CString& desc);

	std::vector<double> m_labels;
	std::vector<CAlgorithmLDADiscriminantFunction> m_discriminantFunctions;

	Eigen::MatrixXd m_coefficients;
	Eigen::MatrixXd m_weights;
	double m_biasDistance = 0;
	double m_w0           = 0;

	size_t m_nCols    = 0;
	size_t m_nClasses = 0;

	Kernel::IAlgorithmProxy* m_covAlgorithm = nullptr;

private:
	void loadClassesFromNode(XML::IXMLNode* node);
	void loadCoefsFromNode(XML::IXMLNode* node);

	size_t getClassCount() const { return m_nClasses; }
};

class CAlgorithmClassifierLDADesc final : public Toolkit::CAlgorithmClassifierDesc
{
public:
	void release() override { }

	CString getName() const override { return "LDA Classifier"; }
	CString getAuthorName() const override { return "Jussi T. Lindgren / Guillaume Serrière"; }
	CString getAuthorCompanyName() const override { return "Inria / Loria"; }
	CString getShortDescription() const override { return "Estimates LDA using regularized or classic covariances"; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return ""; }
	CString getVersion() const override { return "2.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierLDA; }
	IPluginObject* create() override { return new CAlgorithmClassifierLDA; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		prototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage, "Use shrinkage", Kernel::ParameterType_Boolean);
		prototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov, "Shrinkage: Force diagonal cov (DDA)",
									Kernel::ParameterType_Boolean);
		prototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage, "Shrinkage coefficient (-1 == auto)", Kernel::ParameterType_Float);

		CAlgorithmClassifierDesc::getAlgorithmPrototype(prototype);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierLDADesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE

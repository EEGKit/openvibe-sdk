#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../ovp_defines.h"
#include "ovpCAlgorithmLDADiscriminantFunction.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <stack>

#include <Eigen/Dense>

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmLDADiscriminantFunction;

		int LDAClassificationCompare(OpenViBE::IMatrix& first, OpenViBE::IMatrix& second);

		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

		class CAlgorithmClassifierLDA final : public OpenViBE::Toolkit::CAlgorithmClassifier
		{
		public:
			bool initialize() override;
			bool uninitialize() override;
			bool train(const OpenViBE::Toolkit::IFeatureVectorSet& dataset) override;
			bool classify(const OpenViBE::Toolkit::IFeatureVector& sample, double& classId, OpenViBE::Toolkit::IVector& distance,
						  OpenViBE::Toolkit::IVector& probability) override;
			XML::IXMLNode* saveConfig() override;
			bool loadConfig(XML::IXMLNode* configNode) override;
			size_t getNProbabilities() override { return m_discriminantFunctions.size(); }
			size_t getNDistances() override { return m_discriminantFunctions.size(); }

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierLDA)

		protected:
			// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
			static void dumpMatrix(OpenViBE::Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const OpenViBE::CString& desc);

			std::vector<double> m_labels;
			std::vector<CAlgorithmLDADiscriminantFunction> m_discriminantFunctions;

			Eigen::MatrixXd m_coefficients;
			Eigen::MatrixXd m_weights;
			double m_biasDistance = 0;
			double m_w0           = 0;

			size_t m_nCols    = 0;
			size_t m_nClasses = 0;

			OpenViBE::Kernel::IAlgorithmProxy* m_covAlgorithm = nullptr;

		private:
			void loadClassesFromNode(XML::IXMLNode* node);
			void loadCoefsFromNode(XML::IXMLNode* node);

			size_t getClassCount() const { return m_nClasses; }
		};

		class CAlgorithmClassifierLDADesc final : public OpenViBE::Toolkit::CAlgorithmClassifierDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("LDA Classifier"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jussi T. Lindgren / Guillaume Serrière"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria / Loria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Estimates LDA using regularized or classic covariances"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("2.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierLDA; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmClassifierLDA; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage, "Use shrinkage", OpenViBE::Kernel::ParameterType_Boolean);
				prototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov, "Shrinkage: Force diagonal cov (DDA)", OpenViBE::Kernel::ParameterType_Boolean);
				prototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage, "Shrinkage coefficient (-1 == auto)", OpenViBE::Kernel::ParameterType_Float);

				CAlgorithmClassifierDesc::getAlgorithmPrototype(prototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierLDADesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins


#endif // TARGET_HAS_ThirdPartyEIGEN

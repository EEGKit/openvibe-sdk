#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../ovp_defines.h"
#include "ovpCAlgorithmLDADiscriminantFunction.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <stack>

#include <Eigen/Dense>

#define OVP_ClassId_Algorithm_ClassifierLDA                                        OpenViBE::CIdentifier(0x2BA17A3C, 0x1BD46D84)
#define OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable                      OpenViBE::CIdentifier(0x79146976, 0xD7F01A25)
#define OVP_ClassId_Algorithm_ClassifierLDADesc                                    OpenViBE::CIdentifier(0x78FE2929, 0x644945B4)

#define OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage                  OpenViBE::CIdentifier(0x01357534, 0x028312A0)
#define OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage                     OpenViBE::CIdentifier(0x01357534, 0x028312A1)
#define OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov                   OpenViBE::CIdentifier(0x067E45C5, 0x15285CC7)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmLDADiscriminantFunction;

		int LDAClassificationCompare(OpenViBE::IMatrix& firstClassificationValue, OpenViBE::IMatrix& secondClassificationValue);

		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

		class CAlgorithmClassifierLDA final : public OpenViBEToolkit::CAlgorithmClassifier
		{
		public:
			bool initialize() override;
			bool uninitialize() override;
			bool train(const OpenViBEToolkit::IFeatureVectorSet& featureVectorSet) override;
			bool classify(const OpenViBEToolkit::IFeatureVector& featureVector, double& classId, OpenViBEToolkit::IVector& rDistanceValue,
						  OpenViBEToolkit::IVector& rProbabilityValue) override;
			XML::IXMLNode* saveConfiguration() override;
			bool loadConfiguration(XML::IXMLNode* pConfigurationNode) override;
			uint32_t getOutputProbabilityVectorLength() override;
			uint32_t getOutputDistanceVectorLength() override;

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierLDA)

		protected:
			// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
			void dumpMatrix(OpenViBE::Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const OpenViBE::CString& desc);

			std::vector<double> m_vLabelList;
			std::vector<CAlgorithmLDADiscriminantFunction> m_vDiscriminantFunctions;

			Eigen::MatrixXd m_oCoefficients;
			Eigen::MatrixXd m_oWeights;
			double m_f64BiasDistance = 0;
			double m_f64w0           = 0;

			uint32_t m_ui32NumCols    = 0;
			uint32_t m_ui32NumClasses = 0;

			OpenViBE::Kernel::IAlgorithmProxy* m_pCovarianceAlgorithm = nullptr;

		private:
			void loadClassesFromNode(XML::IXMLNode* pNode);
			void loadCoefficientsFromNode(XML::IXMLNode* pNode);

			uint32_t getClassCount();
		};

		class CAlgorithmClassifierLDADesc final : public OpenViBEToolkit::CAlgorithmClassifierDesc
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

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage, "Use shrinkage", OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov, "Shrinkage: Force diagonal cov (DDA)", OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage, "Shrinkage coefficient (-1 == auto)", OpenViBE::Kernel::ParameterType_Float);


				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierLDADesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins


#endif // TARGET_HAS_ThirdPartyEIGEN

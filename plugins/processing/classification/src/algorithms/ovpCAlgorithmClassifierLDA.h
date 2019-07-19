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

		int32_t LDAClassificationCompare(OpenViBE::IMatrix& rFirstClassificationValue, OpenViBE::IMatrix& rSecondClassificationValue);

		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

		class CAlgorithmClassifierLDA : public OpenViBEToolkit::CAlgorithmClassifier
		{
		public:

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual bool classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector
											   , double& rf64Class
											   , OpenViBEToolkit::IVector& rDistanceValue
											   , OpenViBEToolkit::IVector& rProbabilityValue);

			virtual XML::IXMLNode* saveConfiguration();
			virtual bool loadConfiguration(XML::IXMLNode* pConfigurationNode);

			virtual uint32_t getOutputProbabilityVectorLength();
			virtual uint32_t getOutputDistanceVectorLength();

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierLDA);

		protected:
			// Debug method. Prints the matrix to the logManager. May be disabled in implementation.
			void dumpMatrix(OpenViBE::Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const OpenViBE::CString& desc);

			std::vector<double> m_vLabelList;
			std::vector<CAlgorithmLDADiscriminantFunction> m_vDiscriminantFunctions;

			Eigen::MatrixXd m_oCoefficients;
			Eigen::MatrixXd m_oWeights;
			double m_f64BiasDistance;
			double m_f64w0;

			uint32_t m_ui32NumCols;
			uint32_t m_ui32NumClasses;

			OpenViBE::Kernel::IAlgorithmProxy* m_pCovarianceAlgorithm;

		private:
			void loadClassesFromNode(XML::IXMLNode* pNode);
			void loadCoefficientsFromNode(XML::IXMLNode* pNode);

			uint32_t getClassCount();
		};

		class CAlgorithmClassifierLDADesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("LDA Classifier"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Jussi T. Lindgren / Guillaume Serrière"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Inria / Loria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Estimates LDA using regularized or classic covariances"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_ClassifierLDA; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmClassifierLDA; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage, "Use shrinkage", OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov, "Shrinkage: Force diagonal cov (DDA)", OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage, "Shrinkage coefficient (-1 == auto)", OpenViBE::Kernel::ParameterType_Float);


				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierLDADesc);
		};
	};
};



#endif // TARGET_HAS_ThirdPartyEIGEN

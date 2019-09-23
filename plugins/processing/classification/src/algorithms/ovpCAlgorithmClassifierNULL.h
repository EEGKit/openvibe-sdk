#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#define OVP_ClassId_Algorithm_ClassifierNULL                                        OpenViBE::CIdentifier(0x043D09AB, 0xCB5E4859)
#define OVP_ClassId_Algorithm_ClassifierNULLDesc                                    OpenViBE::CIdentifier(0x3B365233, 0x812C47DD)

#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1                    OpenViBE::CIdentifier(0x6DA99952, 0x7E72C143)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2                    OpenViBE::CIdentifier(0xEAC5694A, 0x56CFEF02)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3                    OpenViBE::CIdentifier(0x72F6222D, 0x375BAE2C)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmClassifierNULL final : public OpenViBEToolkit::CAlgorithmClassifier
		{
		public:

			CAlgorithmClassifierNULL() { }
			bool initialize() override;
			bool train(const OpenViBEToolkit::IFeatureVectorSet& featureVectorSet) override;
			bool classify(const OpenViBEToolkit::IFeatureVector& featureVector, double& classId, OpenViBEToolkit::IVector& distance,
						  OpenViBEToolkit::IVector& probability) override;
			XML::IXMLNode* saveConfiguration() override { return nullptr; }
			bool loadConfiguration(XML::IXMLNode* /*configurationNode*/) override { return true; }
			uint32_t getOutputProbabilityVectorLength() override { return 1; }
			uint32_t getOutputDistanceVectorLength() override { return 1; }

			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierNULL)
		};

		class CAlgorithmClassifierNULLDesc final : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("NULL Classifier (does nothing)"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Samples"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierNULL; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmClassifierNULL; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1, "Parameter 1", OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2, "Parameter 2", OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3, "Parameter 3", OpenViBE::Kernel::ParameterType_Enumeration,
					OV_TypeId_Stimulation);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierNULLDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins

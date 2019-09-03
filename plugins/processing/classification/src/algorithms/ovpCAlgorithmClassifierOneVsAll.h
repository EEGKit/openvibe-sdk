#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <vector>

#define OVP_ClassId_Algorithm_ClassifierOneVsAll                                        OpenViBE::CIdentifier(0xD7183FC6, 0xBD74F297)
#define OVP_ClassId_Algorithm_ClassifierOneVsAllDesc                                    OpenViBE::CIdentifier(0xD42D5449, 0x7A28DDB0)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmClassifierOneVsAll final : public OpenViBEToolkit::CAlgorithmPairingStrategy
		{
		public:
			bool initialize() override;
			bool uninitialize() override;
			bool train(const OpenViBEToolkit::IFeatureVectorSet& featureVectorSet) override;
			bool classify(const OpenViBEToolkit::IFeatureVector& featureVector, double& classId, OpenViBEToolkit::IVector& distanceValue,
						  OpenViBEToolkit::IVector& probabilityValue) override;
			bool designArchitecture(const OpenViBE::CIdentifier& id, uint32_t nClass) override;
			XML::IXMLNode* saveConfiguration() override;
			bool loadConfiguration(XML::IXMLNode* configurationNode) override;
			uint32_t getOutputProbabilityVectorLength() override;
			uint32_t getOutputDistanceVectorLength() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsAll)


		private:
			XML::IXMLNode* getClassifierConfiguration(OpenViBE::Kernel::IAlgorithmProxy* classifier);
			bool addNewClassifierAtBack();
			void removeClassifierAtBack();
			bool setSubClassifierIdentifier(const OpenViBE::CIdentifier& id);
			uint32_t getClassCount() const;

			bool loadSubClassifierConfiguration(XML::IXMLNode* subClassifiersNode);

			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_oSubClassifierList;
			fClassifierComparison m_fAlgorithmComparison = nullptr;
		};

		class CAlgorithmClassifierOneVsAllDesc final : public OpenViBEToolkit::CAlgorithmPairingStrategyDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("OneVsAll pairing classifier"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Guillaume Serriere"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/Loria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierOneVsAll; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmClassifierOneVsAll; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsAllDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins

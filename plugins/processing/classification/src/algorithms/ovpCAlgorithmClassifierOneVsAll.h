#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <vector>


namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmClassifierOneVsAll final : public OpenViBEToolkit::CAlgorithmPairingStrategy
		{
		public:
			bool initialize() override;
			bool uninitialize() override;
			bool train(const OpenViBEToolkit::IFeatureVectorSet& dataset) override;
			bool classify(const OpenViBEToolkit::IFeatureVector& sample, double& classId, OpenViBEToolkit::IVector& distance,
						  OpenViBEToolkit::IVector& probability) override;
			bool designArchitecture(const OpenViBE::CIdentifier& id, const size_t nClass) override;
			XML::IXMLNode* saveConfig() override;
			bool loadConfig(XML::IXMLNode* configNode) override;
			size_t getNProbabilities() override { return m_subClassifiers.size(); }
			size_t getNDistances() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsAll)


		private:
			static XML::IXMLNode* getClassifierConfig(OpenViBE::Kernel::IAlgorithmProxy* classifier);
			bool addNewClassifierAtBack();
			void removeClassifierAtBack();
			bool setSubClassifierIdentifier(const OpenViBE::CIdentifier& id);
			size_t getClassCount() const { return m_subClassifiers.size(); }

			bool loadSubClassifierConfig(XML::IXMLNode* node);

			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_subClassifiers;
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

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(prototype);

				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsAllDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins

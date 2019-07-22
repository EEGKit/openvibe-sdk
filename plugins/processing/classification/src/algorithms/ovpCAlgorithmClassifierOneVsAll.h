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
		class CAlgorithmClassifierOneVsAll : public OpenViBEToolkit::CAlgorithmPairingStrategy
		{
		public:

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual bool classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector
								  , double& rf64Class
								  , OpenViBEToolkit::IVector& rDistanceValue
								  , OpenViBEToolkit::IVector& rProbabilityValue);
			virtual bool designArchitecture(const OpenViBE::CIdentifier& rId, uint32_t rClassCount);

			virtual XML::IXMLNode* saveConfiguration();
			virtual bool loadConfiguration(XML::IXMLNode* pConfigurationNode);

			virtual uint32_t getOutputProbabilityVectorLength();
			virtual uint32_t getOutputDistanceVectorLength();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsAll)

		protected:


		private:
			XML::IXMLNode* getClassifierConfiguration(OpenViBE::Kernel::IAlgorithmProxy* classifier);
			bool addNewClassifierAtBack();
			void removeClassifierAtBack();
			bool setSubClassifierIdentifier(const OpenViBE::CIdentifier& rId);
			uint32_t getClassCount() const;

			bool loadSubClassifierConfiguration(XML::IXMLNode* pSubClassifiersNode);

			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_oSubClassifierList;
			fClassifierComparison m_fAlgorithmComparison;
		};

		class CAlgorithmClassifierOneVsAllDesc : public OpenViBEToolkit::CAlgorithmPairingStrategyDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("OneVsAll pairing classifier"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Guillaume Serriere"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/Loria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_ClassifierOneVsAll; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmClassifierOneVsAll; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsAllDesc)
		};
	}
}

#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <map>

#define OVP_ClassId_BoxAlgorithm_ClassifierProcessor       OpenViBE::CIdentifier(0x5FE23D17, 0x95B0452C)
#define OVP_ClassId_BoxAlgorithm_ClassifierProcessorDesc   OpenViBE::CIdentifier(0x29B66B00, 0xB4683D49)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmClassifierProcessor final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClassifierProcessor)

		protected:
			virtual bool loadClassifier(const char* sFilename);

		private:

			OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierProcessor> m_oFeatureVectorDecoder;
			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmClassifierProcessor> m_oStimulationDecoder;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmClassifierProcessor> m_oLabelsEncoder;
			OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmClassifierProcessor> m_oHyperplaneValuesEncoder;
			OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmClassifierProcessor> m_oProbabilityValuesEncoder;

			OpenViBE::Kernel::IAlgorithmProxy* m_pClassifier = nullptr;

			std::map<double, uint64_t> m_vStimulation;
		};

		class CBoxAlgorithmClassifierProcessorDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Classifier processor"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard, Guillaume Serriere"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Generic classification, relying on several box algorithms"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Classifies incoming feature vectors using a previously learned classifier.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Classification"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("2.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ClassifierProcessor; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmClassifierProcessor; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Features", OV_TypeId_FeatureVector);
				prototype.addInput("Commands", OV_TypeId_Stimulations);
				prototype.addOutput("Labels", OV_TypeId_Stimulations);
				prototype.addOutput("Hyperplane distance", OV_TypeId_StreamedMatrix);
				prototype.addOutput("Probability values", OV_TypeId_StreamedMatrix);

				//We load everything in the save filed
				prototype.addSetting("Filename to load configuration from", OV_TypeId_Filename, "");
				return true;
			}

			// virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmCommonClassifierListener(5); }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierProcessorDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins

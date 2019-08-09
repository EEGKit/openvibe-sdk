#pragma once

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>
#include <queue>
#include <cstdio>


#define OVP_ClassId_BoxAlgorithm_FeatureAggregatorDesc                             OpenViBE::CIdentifier(0x00B5B638, 0x25821BAF)
#define OVP_ClassId_BoxAlgorithm_FeatureAggregator                                 OpenViBE::CIdentifier(0x00682417, 0x453635F9)

namespace OpenViBEPlugins
{
	namespace FeatureExtraction
	{
		/**
		 * Main plugin class of the feature aggregator plugins.
		 * Aggregates the features received in a feature vector then outputs it.
		 * */
		class CBoxAlgorithmFeatureAggregator : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmFeatureAggregator();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_FeatureAggregator)

		public:
			//codecs
			OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmFeatureAggregator>* m_pFeatureVectorEncoder;
			std::vector<OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>*> m_pStreamedMatrixDecoder;

			// contains the labels for each dimension for each input
			std::vector<std::vector<std::vector<std::string>>> m_oFeatureNames;

			// contains the dimension size for each dimension of each input
			std::vector<std::vector<uint32_t>> m_oDimensionSize;

			// contains the input buffer's total size for each input
			std::vector<uint64_t> m_oInputBufferSizes;

			//start time and end time of the last arrived chunk
			uint64_t m_ui64LastChunkStartTime = 0;
			uint64_t m_ui64LastChunkEndTime = 0;

			// number of inputs
			uint32_t m_ui32NumberOfInput = 0;

			// current input (usesful in the EBML callbacks)
			uint32_t m_ui32CurrentInput = 0;

			// the feature vector
			double* m_pVectorBuffer = nullptr;
			// the feature vector size
			uint32_t m_ui32VectorSize = 0;

			bool m_bHeaderSent = false;
		};

		class CBoxAlgorithmFeatureAggregatorListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];

				for (uint32_t i = 0; i < rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i + 1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override { return this->check(rBox); }
			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override { return this->check(rBox); };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		/**
		* Plugin's description
		*/
		class CBoxAlgorithmFeatureAggregatorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			OpenViBE::CString getName() const override { return OpenViBE::CString("Feature aggregator"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Bruno Renier"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Aggregates input to feature vectors"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Each chunk of input will be catenated into one feature vector."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Feature extraction"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			void release() override { }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_FeatureAggregator; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmFeatureAggregator(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmFeatureAggregatorListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input stream 1", OV_TypeId_StreamedMatrix);
				// rPrototype.addInput("Input stream 2", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput("Feature vector stream", OV_TypeId_FeatureVector);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FeatureAggregatorDesc)
		};
	};
};

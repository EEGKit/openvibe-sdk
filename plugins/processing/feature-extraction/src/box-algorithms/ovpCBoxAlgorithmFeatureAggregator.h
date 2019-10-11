#pragma once

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>
#include <queue>

namespace OpenViBEPlugins
{
	namespace FeatureExtraction
	{
		/**
		 * Main plugin class of the feature aggregator plugins.
		 * Aggregates the features received in a feature vector then outputs it.
		 * */
		class CBoxAlgorithmFeatureAggregator final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmFeatureAggregator() { }
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_FeatureAggregator)

			//codecs
			OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmFeatureAggregator>* m_pFeatureVectorEncoder = nullptr;
			std::vector<OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>*> m_pStreamedMatrixDecoder;

			// contains the labels for each dimension for each input
			std::vector<std::vector<std::vector<std::string>>> m_oFeatureNames;

			// contains the dimension size for each dimension of each input
			std::vector<std::vector<size_t>> m_oDimensionSize;

			// contains the input buffer's total size for each input
			std::vector<size_t> m_oInputBufferSizes;

			//start time and end time of the last arrived chunk
			uint64_t m_lastChunkStartTime = 0;
			uint64_t m_lastChunkEndTime   = 0;

			// number of inputs
			uint32_t m_nInput = 0;

			bool m_headerSent = false;
		};

		class CBoxAlgorithmFeatureAggregatorListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box)
			{
				for (uint32_t i = 0; i < box.getInputCount(); ++i)
				{
					box.setInputName(i, ("Input stream " + std::to_string(i + 1)).c_str());
					box.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override { return this->check(box); }
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override { return this->check(box); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		/**
		* Plugin's description
		*/
		class CBoxAlgorithmFeatureAggregatorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			OpenViBE::CString getName() const override { return OpenViBE::CString("Feature aggregator"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Bruno Renier"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Aggregates input to feature vectors"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Each chunk of input will be catenated into one feature vector.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Feature extraction"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			void release() override { }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_FeatureAggregator; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmFeatureAggregator(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmFeatureAggregatorListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

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
	} // namespace FeatureExtraction
} // namespace OpenViBEPlugins

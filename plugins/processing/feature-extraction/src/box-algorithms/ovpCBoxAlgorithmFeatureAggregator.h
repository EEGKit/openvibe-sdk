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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool processInput(const uint32_t ui32InputIndex);

			virtual bool process();

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
			uint64_t m_ui64LastChunkStartTime;
			uint64_t m_ui64LastChunkEndTime;

			// number of inputs
			uint32_t m_ui32NumberOfInput;

			// current input (usesful in the EBML callbacks)
			uint32_t m_ui32CurrentInput;

			// the feature vector
			double* m_pVectorBuffer;
			// the feature vector size
			uint32_t m_ui32VectorSize;

			bool m_bHeaderSent;
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

			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return this->check(rBox); }
			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return this->check(rBox); };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		/**
		* Plugin's description
		*/
		class CBoxAlgorithmFeatureAggregatorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Feature aggregator"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Aggregates input to feature vectors"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Each chunk of input will be catenated into one feature vector."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Feature extraction"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual void release() { }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_FeatureAggregator; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmFeatureAggregator(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmFeatureAggregatorListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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

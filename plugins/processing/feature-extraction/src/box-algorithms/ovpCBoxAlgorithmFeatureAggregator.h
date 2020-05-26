#pragma once

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>
#include <queue>

namespace OpenViBE {
namespace Plugins {
namespace FeatureExtraction {

/**
 * Main plugin class of the feature aggregator plugins.
 * Aggregates the features received in a feature vector then outputs it.
 * */
class CBoxAlgorithmFeatureAggregator final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:

	CBoxAlgorithmFeatureAggregator() { }
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_FeatureAggregator)

protected:
	//codecs
	Toolkit::TFeatureVectorEncoder<CBoxAlgorithmFeatureAggregator>* m_encoder = nullptr;
	std::vector<Toolkit::TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>*> m_decoder;

	// contains the labels for each dimension for each input
	std::vector<std::vector<std::vector<std::string>>> m_featureNames;

	// contains the dimension size for each dimension of each input
	std::vector<std::vector<size_t>> m_dimSize;

	// contains the input buffer's total size for each input
	std::vector<size_t> m_iBufferSizes;

	//start time and end time of the last arrived chunk
	CTime m_lastChunkStartTime = 0;
	CTime m_lastChunkEndTime   = 0;

	// number of inputs
	size_t m_nInput = 0;

	bool m_headerSent = false;
};

class CBoxAlgorithmFeatureAggregatorListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:

	bool check(Kernel::IBox& box) const
	{
		for (size_t i = 0; i < box.getInputCount(); ++i)
		{
			box.setInputName(i, ("Input stream " + std::to_string(i + 1)).c_str());
			box.setInputType(i, OV_TypeId_StreamedMatrix);
		}

		return true;
	}

	bool onInputRemoved(Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }
	bool onInputAdded(Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
};

/**
* Plugin's description
*/
class CBoxAlgorithmFeatureAggregatorDesc final : public IBoxAlgorithmDesc
{
public:
	CString getName() const override { return CString("Feature aggregator"); }
	CString getAuthorName() const override { return CString("Bruno Renier"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString("Aggregates input to feature vectors"); }

	CString getDetailedDescription() const override { return CString("Each chunk of input will be catenated into one feature vector."); }

	CString getCategory() const override { return CString("Feature extraction"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	void release() override { }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_FeatureAggregator; }
	IPluginObject* create() override { return new CBoxAlgorithmFeatureAggregator(); }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmFeatureAggregatorListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input stream 1", OV_TypeId_StreamedMatrix);
		// prototype.addInput("Input stream 2", OV_TypeId_StreamedMatrix);
		prototype.addOutput("Feature vector stream", OV_TypeId_FeatureVector);
		prototype.addFlag(Kernel::BoxFlag_CanAddInput);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FeatureAggregatorDesc)
};

}  // namespace FeatureExtraction
}  // namespace Plugins
}  // namespace OpenViBE

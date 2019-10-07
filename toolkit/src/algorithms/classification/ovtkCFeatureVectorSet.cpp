#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

// ____________________________________________________________________________________________________________________________________
//

CFeatureVectorSet::CFeatureVectorSet(const IMatrix& rMatrix) : m_matrix(rMatrix)
{
	if (rMatrix.getDimensionCount() != 2) { throw std::runtime_error("Fetaure vector set matrix must be 2 dimensions"); }

	for (uint32_t i = 0; i < rMatrix.getDimensionSize(0); ++i)
	{
		m_features[i].m_Matrix      = &rMatrix;
		m_features[i].m_DimensionIdx = i;
		m_features[i].m_Size         = rMatrix.getDimensionSize(1) - 1;
		m_features[i].m_Buffer      = rMatrix.getBuffer() + i * rMatrix.getDimensionSize(1);
	}
}

IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32_t index)
{
	auto itFeatureVector = m_features.find(index);
	return itFeatureVector->second;
}

const IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32_t index) const
{
	const auto itFeatureVector = m_features.find(index);
	return itFeatureVector->second;
}

uint32_t CFeatureVectorSet::getLabelCount() const
{
	std::map<double, bool> labels;
	for (auto itFeatureVector = m_features.begin(); itFeatureVector != m_features.end(); ++
		 itFeatureVector) { labels[itFeatureVector->second.getLabel()] = true; }
	return uint32_t(labels.size());
}

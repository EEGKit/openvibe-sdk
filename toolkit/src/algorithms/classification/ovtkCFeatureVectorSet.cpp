#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

// ____________________________________________________________________________________________________________________________________
//

CFeatureVectorSet::CFeatureVectorSet(const IMatrix& matrix) : m_matrix(matrix)
{
	if (matrix.getDimensionCount() != 2) { throw std::runtime_error("Fetaure vector set matrix must be 2 dimensions"); }

	for (size_t i = 0; i < matrix.getDimensionSize(0); ++i)
	{
		m_features[i].m_Matrix       = &matrix;
		m_features[i].m_DimensionIdx = i;
		m_features[i].m_Size         = matrix.getDimensionSize(1) - 1;
		m_features[i].m_Buffer       = matrix.getBuffer() + i * matrix.getDimensionSize(1);
	}
}

IFeatureVector& CFeatureVectorSet::getFeatureVector(const size_t index)
{
	auto itFeatureVector = m_features.find(index);
	return itFeatureVector->second;
}

const IFeatureVector& CFeatureVectorSet::getFeatureVector(const size_t index) const
{
	const auto itFeatureVector = m_features.find(index);
	return itFeatureVector->second;
}

size_t CFeatureVectorSet::getLabelCount() const
{
	std::map<double, bool> labels;
	for (auto itFeatureVector = m_features.begin(); itFeatureVector != m_features.end(); ++
		 itFeatureVector) { labels[itFeatureVector->second.getLabel()] = true; }
	return labels.size();
}

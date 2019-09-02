#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

// ____________________________________________________________________________________________________________________________________
//

CFeatureVectorSet::CFeatureVectorSet(const IMatrix& rMatrix) : m_rMatrix(rMatrix)
{
	if (rMatrix.getDimensionCount() != 2) { throw std::runtime_error("Fetaure vector set matrix must be 2 dimensions"); }

	for (uint32_t i = 0; i < rMatrix.getDimensionSize(0); i++)
	{
		m_vFeatureVector[i].m_pMatrix      = &rMatrix;
		m_vFeatureVector[i].m_dimensionIdx = i;
		m_vFeatureVector[i].m_size         = rMatrix.getDimensionSize(1) - 1;
		m_vFeatureVector[i].m_pBuffer      = rMatrix.getBuffer() + i * rMatrix.getDimensionSize(1);
	}
}

IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32_t index)
{
	std::map<uint32_t, CInternalFeatureVector>::iterator itFeatureVector = m_vFeatureVector.find(index);
	return itFeatureVector->second;
}

const IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32_t index) const
{
	const auto itFeatureVector = m_vFeatureVector.find(index);
	return itFeatureVector->second;
}

uint32_t CFeatureVectorSet::getLabelCount() const
{
	std::map<double, bool> labels;
	for (std::map<uint32_t, CInternalFeatureVector>::const_iterator itFeatureVector = m_vFeatureVector.begin(); itFeatureVector != m_vFeatureVector.end(); ++
		 itFeatureVector) { labels[itFeatureVector->second.getLabel()] = true; }
	return uint32_t(labels.size());
}

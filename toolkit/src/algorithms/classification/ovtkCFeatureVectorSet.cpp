#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

// ____________________________________________________________________________________________________________________________________
//

CInternalFeatureVector::CInternalFeatureVector()
	: m_pMatrix(nullptr), m_ui32DimensionIndex(0), m_ui32BufferElementCount(0), m_pBuffer(nullptr) {}

uint32_t CInternalFeatureVector::getSize() const { return m_ui32BufferElementCount; }
bool CInternalFeatureVector::setSize(const uint32_t ui32Size) { return false; }
double* CInternalFeatureVector::getBuffer() { return nullptr; }
const double* CInternalFeatureVector::getBuffer() const { return m_pBuffer; }

const char* CInternalFeatureVector::getElementLabel(const uint32_t ui32Index) const
{
	return m_pMatrix->getDimensionLabel(m_ui32DimensionIndex, ui32Index);
}

bool CInternalFeatureVector::setElementLabel(const uint32_t ui32Index, const char* sElementLabel) { return false; }

double CInternalFeatureVector::getLabel() const
{
	return m_pBuffer[m_ui32BufferElementCount];
}

bool CInternalFeatureVector::setLabel(const double f64Label) { return false; }

// ____________________________________________________________________________________________________________________________________
//

CFeatureVectorSet::CFeatureVectorSet(const IMatrix& rMatrix)
	: m_rMatrix(rMatrix)
{
	if (rMatrix.getDimensionCount() != 2)
	{
		throw std::runtime_error("Fetaure vector set matrix must be 2 dimensions");
	}

	for (uint32_t i = 0; i < rMatrix.getDimensionSize(0); i++)
	{
		m_vFeatureVector[i].m_pMatrix                = &rMatrix;
		m_vFeatureVector[i].m_ui32DimensionIndex     = i;
		m_vFeatureVector[i].m_ui32BufferElementCount = rMatrix.getDimensionSize(1) - 1;
		m_vFeatureVector[i].m_pBuffer                = rMatrix.getBuffer() + i * rMatrix.getDimensionSize(1);
	}
}

uint32_t CFeatureVectorSet::getFeatureVectorCount() const
{
	return m_rMatrix.getDimensionSize(0);
}

bool CFeatureVectorSet::setFeatureVectorCount(const uint32_t ui32FeatureVectorCount) { return false; }

bool CFeatureVectorSet::addFeatureVector(const IFeatureVector& rFeatureVector) { return false; }

IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32_t ui32Index)
{
	std::map<uint32_t, CInternalFeatureVector>::iterator itFeatureVector = m_vFeatureVector.find(ui32Index);
	return itFeatureVector->second;
}

const IFeatureVector& CFeatureVectorSet::getFeatureVector(const uint32_t ui32Index) const
{
	std::map<uint32_t, CInternalFeatureVector>::const_iterator itFeatureVector = m_vFeatureVector.find(ui32Index);
	return itFeatureVector->second;
}

uint32_t CFeatureVectorSet::getLabelCount() const
{
	std::map<double, bool> l_vLabel;
	for (std::map<uint32_t, CInternalFeatureVector>::const_iterator itFeatureVector = m_vFeatureVector.begin(); itFeatureVector != m_vFeatureVector.end(); ++itFeatureVector)
	{
		l_vLabel[itFeatureVector->second.getLabel()] = true;
	}
	return static_cast<uint32_t>(l_vLabel.size());
}

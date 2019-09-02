#define PKPD_DEBUG 0
#include "ovpCAlgorithmPairwiseStrategyPKPD.h"

#include <iostream>


#include <xml/IXMLNode.h>
#include <xml/IXMLHandler.h>

namespace
{
	const char* const TYPE_NODE_NAME = "PairwiseDecision_PKDP";
}


using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

bool CAlgorithmPairwiseStrategyPKPD::initialize() { return true; }

bool CAlgorithmPairwiseStrategyPKPD::uninitialize() { return true; }

bool CAlgorithmPairwiseStrategyPKPD::parameterize()
{
	TParameterHandler<uint64_t> ip_pClassCount(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	m_ui32ClassCount = uint32_t(ip_pClassCount);

	OV_ERROR_UNLESS_KRF(m_ui32ClassCount >= 2, "Pairwise decision PKPD algorithm needs at least 2 classes [" << m_ui32ClassCount << "] found",
						OpenViBE::Kernel::ErrorType::BadInput);

	return true;
}

bool CAlgorithmPairwiseStrategyPKPD::compute(std::vector<SClassificationInfo>& pClassificationValueList, IMatrix* pProbabilityVector)
{
	OV_ERROR_UNLESS_KRF(m_ui32ClassCount >= 2, "Pairwise decision PKPD algorithm needs at least 2 classes [" << m_ui32ClassCount << "] found",
						OpenViBE::Kernel::ErrorType::BadInput);

	double* l_pProbabilityMatrix = new double[m_ui32ClassCount * m_ui32ClassCount];

	//First we set the diagonal to 0
	for (size_t i = 0; i < m_ui32ClassCount; ++i) { l_pProbabilityMatrix[i * m_ui32ClassCount + i] = 0.; }

	for (size_t i = 0; i < pClassificationValueList.size(); ++i)
	{
		SClassificationInfo& l_rTemp                                                = pClassificationValueList[i];
		const uint32_t l_f64FirstIndex                                              = uint32_t(l_rTemp.m_f64FirstClass);
		const uint32_t l_f64SecondIndex                                             = uint32_t(l_rTemp.m_f64SecondClass);
		const double* l_pValues                                                     = l_rTemp.m_pClassificationValue->getBuffer();
		l_pProbabilityMatrix[l_f64FirstIndex * m_ui32ClassCount + l_f64SecondIndex] = l_pValues[0];
		l_pProbabilityMatrix[l_f64SecondIndex * m_ui32ClassCount + l_f64FirstIndex] = 1 - l_pValues[0];
	}

#if PKPD_DEBUG
	for(uint32_t i = 0 ; i< m_ui32ClassCount ; ++i){

		for(uint32_t j = 0 ; j<m_ui32ClassCount ; ++j){
			std::cout << l_pProbabilityMatrix[i*m_ui32ClassCount + j] << " ";
		}
		std::cout << std::endl;
	}
#endif

	double* l_pProbVector   = new double[m_ui32ClassCount];
	double l_pProbVectorSum = 0;
	for (uint32_t l_ui32ClassIndex = 0; l_ui32ClassIndex < m_ui32ClassCount; ++l_ui32ClassIndex)
	{
		double l_pTempSum = 0;
		for (uint32_t l_ui32SecondClass = 0; l_ui32SecondClass < m_ui32ClassCount; ++l_ui32SecondClass)
		{
			if (l_ui32SecondClass != l_ui32ClassIndex) { l_pTempSum += 1 / l_pProbabilityMatrix[m_ui32ClassCount * l_ui32ClassIndex + l_ui32SecondClass]; }
		}
		l_pProbVector[l_ui32ClassIndex] = 1 / (l_pTempSum - (m_ui32ClassCount - 2));
		l_pProbVectorSum += l_pProbVector[l_ui32ClassIndex];
	}

	for (uint32_t i = 0; i < m_ui32ClassCount; ++i) { l_pProbVector[i] /= l_pProbVectorSum; }

#if PKPD_DEBUG
	for(uint32_t i = 0; i<m_ui32ClassCount ; ++i) { std::cout << l_pProbVector[i] << " "; }
	std::cout << std::endl;
#endif

	pProbabilityVector->setDimensionCount(1);
	pProbabilityVector->setDimensionSize(0, m_ui32ClassCount);

	for (uint32_t i = 0; i < m_ui32ClassCount; ++i) { pProbabilityVector->getBuffer()[i] = l_pProbVector[i]; }

	delete[] l_pProbabilityMatrix;
	delete[] l_pProbVector;
	return true;
}

XML::IXMLNode* CAlgorithmPairwiseStrategyPKPD::saveConfiguration()
{
	XML::IXMLNode* l_pRootNode = XML::createNode(TYPE_NODE_NAME);
	return l_pRootNode;
}

bool CAlgorithmPairwiseStrategyPKPD::loadConfiguration(XML::IXMLNode& rNode) { return true; }
